/*
    node-rng, a library for accessing a true hardware random number generator
    Copyright (C) 2016 Cherit.ee Inc
*/

// Includes...

    // Our headers......
    #include "bindings.h"
    #include "random.h"

    // Standard C++...
    #include <cstdlib>

// Import namespaces...

    // Standard C++...
    using namespace std;

    // V8...
    using v8::Exception;
    using v8::Function;
    using v8::FunctionCallbackInfo;
    using v8::Handle;
    using v8::HandleScope;
    using v8::Int32;
    using v8::Integer;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Uint32;
    using v8::Value;

// Local declarations of implementations for asynchronous variations of the
//  public interface...
namespace rng
{
    // Thread implementing JavaScript's rng.getRandomAsync(function(result)) and
    //  its corresponding completion function...
    static void getRandomThread(uv_work_t *Request);
    static void getRandomThreadComplete(uv_work_t *Request, int Status);

    // Thread implementing JavaScript's rng.getRandomRangeAsync(lower, upper,
    //  function(lower, upper, result)) and its corresponding completion
    //  function...
    static void getRandomRangeThread(uv_work_t *Request);
    static void getRandomRangeThreadComplete(uv_work_t *Request, int Status);
}

// All methods exported, unless marked static, to the VM...
namespace rng {

// Callback implementing JavaScript rng.getCorrections()...
void getCorrections(const FunctionCallbackInfo<Value> &Arguments)
{
    // Get the random number generator...
    RandomNumberGenerator &Generator = RandomNumberGenerator::GetInstance();

    // Pass random number back to caller...
    Arguments.GetReturnValue().Set(Generator.GetCorrections());
}

// Callback implementing JavaScript rng.getRandomAsync(function(result))...
void getRandomAsync(const FunctionCallbackInfo<Value> &Arguments)
{
    // Retrieve virtual machine state...
    Isolate *isolate = Arguments.GetIsolate();

    // Validate arguments...

        // Insufficient in number...
        if(Arguments.Length() != 1)
        {
            // Throw a JavaScript exception within the virtual machine...
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "expected one argument")));
            return;
        }

        // Incorrect type...
        if(!Arguments[0]->IsFunction())
        {
            // Throw a JavaScript exception within the virtual machine...
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "expected a function argument")));
            return;
        }

    // Prepare and initialize the work object to store inputs and outputs of
    //  this call on the heap...
    Work<uint32_t> *work = new Work<uint32_t>();
    work->Request.data = work;

    // Remember the location of the callback that was provided in the VM...
    Local<Function> callback = Local<Function>::Cast(Arguments[0]);
    work->Callback.Reset(isolate, callback);

    // Initiate worker thread...
    uv_queue_work(
        uv_default_loop(),
       &work->Request,
        rng::getRandomThread,
        rng::getRandomThreadComplete);

    // Caller should not expect anything returned when invoked asynchronously...
    Arguments.GetReturnValue().Set(Undefined(isolate));
}

// Thread implementing JavaScript's rng.getRandomAsync(function(result))...
static void getRandomThread(uv_work_t *Request)
{
    // Retrieve the work object from the heap...
    Work<uint32_t> *work = static_cast<Work<uint32_t> *>(Request->data);

    // Get the actual random number...
    const uint32_t Random =
        RandomNumberGenerator::GetInstance().GetRandom32();

    // Store result for user's callback...
    work->Result = Random;
}

// Callback invoked upon getRandomThread completing execution...
static void getRandomThreadComplete(uv_work_t *Request, int Status)
{
    // Retrieve virtual machine state...
    Isolate *isolate = Isolate::GetCurrent();

    // This is required for Node 4.x...
    HandleScope handleScope(isolate);

    // Retrieve the work object from the heap...
    Work<uint32_t> *work = static_cast<Work<uint32_t> *>(Request->data);

    // Store the result for the caller...
    Local<Integer> CallbackResultParameter = Uint32::NewFromUnsigned(isolate, work->Result);
    Handle<Value> CallbackArguments[] = { CallbackResultParameter };

    // Execute the caller's callback...
    Local<Function>::New(isolate, work->Callback)->
        Call(isolate->GetCurrentContext()->Global(), 1, CallbackArguments);

    // Cleanup persistent function callback and the worker bookkeeping memory...
    work->Callback.Reset();
    delete work;
}

// Callback implementing JavaScript rng.getRandom()...
void getRandom(const FunctionCallbackInfo<Value> &Arguments)
{
    // Get the random number generator...
    RandomNumberGenerator &Generator = RandomNumberGenerator::GetInstance();

    // Pass random number back to caller...
    Arguments.GetReturnValue().Set(Generator.GetRandom32());
}

// Callback implementing JavaScript
//  rng.getRandomRangeAsync(lower, upper, function(lower, upper, result))...
void getRandomRangeAsync(const FunctionCallbackInfo<Value> &Arguments)
{
    // Retrieve virtual machine state...
    Isolate *isolate = Arguments.GetIsolate();

    // Validate arguments...

        // Insufficient in number...
        if(Arguments.Length() != 3)
        {
            // Throw a JavaScript exception within the virtual machine...
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "expected three arguments")));
            return;
        }

        // Incorrect type...
        if(!Arguments[0]->IsInt32() || !Arguments[1]->IsInt32() || !Arguments[2]->IsFunction())
        {
            // Throw a JavaScript exception within the virtual machine...
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "expected two integers and a function")));
            return;
        }

        // Get arguments...
        const int32_t Lower = Arguments[0]->ToInt32()->Value();
        const int32_t Upper = Arguments[1]->ToInt32()->Value();

        // Invalid values...
        if(Lower >= Upper)
        {
            // Throw a JavaScript exception within the virtual machine...
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "first argument must be less than second")));
            return;
        }

    // Prepare and initialize the work object to store inputs and outputs of
    //  this call on the heap...
    Work<int32_t> *work = new Work<int32_t>();
    work->Request.data = work;

    // Load the work object with the caller's inputs and outputs...
    work->Lower = Lower;
    work->Upper = Upper;

    // Remember the location of the callback that was provided in the VM...
    Local<Function> callback = Local<Function>::Cast(Arguments[2]);
    work->Callback.Reset(isolate, callback);

    // Initiate worker thread...
    uv_queue_work(
        uv_default_loop(),
       &work->Request,
        rng::getRandomRangeThread,
        rng::getRandomRangeThreadComplete);

    // Caller should not expect anything returned when invoked asynchronously...
    Arguments.GetReturnValue().Set(Undefined(isolate));
}

// Thread implementing JavaScript's
//  rng.getRandomRangeAsync(lower, upper, function(lower, upper, result))...
static void getRandomRangeThread(uv_work_t *Request)
{
    // Retrieve the work object from the heap...
    Work<int32_t> *work = static_cast<Work<int32_t> *>(Request->data);

    // Get arguments...
    const int32_t Lower = work->Lower;
    const int32_t Upper = work->Upper;

    // Get the actual random number...
    const int32_t Random =
        RandomNumberGenerator::GetInstance().GetRandomRange32(Lower, Upper);

    // Store result for user's callback...
    work->Result = Random;
}

// Callback invoked upon getRandomThread completing execution...
static void getRandomRangeThreadComplete(uv_work_t *Request, int Status)
{
    // Retrieve virtual machine state...
    Isolate *isolate = Isolate::GetCurrent();

    // This is required for Node 4.x...
    HandleScope handleScope(isolate);

    // Retrieve the work object from the heap...
    Work<int32_t> *work = static_cast<Work<int32_t> *>(Request->data);

    // Store the result for the caller...
    Local<Integer> CallbackLowerParameter = Int32::New(isolate, work->Lower);
    Local<Integer> CallbackUpperParameter = Int32::New(isolate, work->Upper);
    Local<Integer> CallbackResultParameter = Int32::New(isolate, work->Result);
    Handle<Value> CallbackArguments[] = {
        CallbackLowerParameter, CallbackUpperParameter, CallbackResultParameter };

    // Execute the caller's callback...
    Local<Function>::New(isolate, work->Callback)->
        Call(isolate->GetCurrentContext()->Global(), 3, CallbackArguments);

    // Cleanup persistent function callback and the worker bookkeeping memory...
    work->Callback.Reset();
    delete work;
}

// Callback implementing JavaScript rng.getRandom(lower, upper)...
void getRandomRange(const FunctionCallbackInfo<Value> &Arguments)
{
    // Retrieve virtual machine state...
    Isolate *isolate = Arguments.GetIsolate();

    // Validate arguments...

        // Insufficient in number...
        if(Arguments.Length() != 2)
        {
            // Throw a JavaScript exception within the virtual machine...
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "expected two arguments")));
            return;
        }

        // Incorrect type...
        if(!Arguments[0]->IsInt32() || !Arguments[1]->IsInt32())
        {
            // Throw a JavaScript exception within the virtual machine...
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "both arguments should be integers")));
            return;
        }

        // Get arguments...
        const int32_t Lower = Arguments[0]->ToInt32()->Value();
        const int32_t Upper = Arguments[1]->ToInt32()->Value();

        // Invalid values...
        if(Lower >= Upper)
        {
            // Throw a JavaScript exception within the virtual machine...
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "first argument must be less than second")));
            return;
        }

    // Get the random number generator...
    RandomNumberGenerator &Generator = RandomNumberGenerator::GetInstance();

    // Pass random number back to caller...
    Arguments.GetReturnValue().Set(Generator.GetRandomRange32(Lower, Upper));
}

// Callback implementing JavaScript rng.getVersionMajor()...
void getVersionMajor(const FunctionCallbackInfo<Value> &Arguments)
{
    Arguments.GetReturnValue().Set(NODE_RNG_VERSION_MAJOR);
}

// Callback implementing JavaScript rng.getVersionMinor()...
void getVersionMinor(const FunctionCallbackInfo<Value> &Arguments)
{
    Arguments.GetReturnValue().Set(NODE_RNG_VERSION_MINOR);
}

// Callback implementing JavaScript rng.isAvailable()...
void isAvailable(const FunctionCallbackInfo<Value> &Arguments)
{
    // Get the random number generator...
    const RandomNumberGenerator &Generator = RandomNumberGenerator::GetInstance();

    // Query it's availability and return to JavaScript caller...
    Arguments.GetReturnValue().Set(Generator.IsAvailable());
}

}
