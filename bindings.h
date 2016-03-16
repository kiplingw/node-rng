/*
    node-rng, a library for accessing a true hardware random number generator
    Copyright (C) 2016 Cherit.ee Inc
*/

// Includes...

    // Node.js and libuv...
    #include <node.h>
    #include <uv.h>

    // Our headers...
    #include "node-rng.h"

    // Standard C++...
    #include <cstdint>
    #include <vector>

// Work structure for asynchronous calls to be stored on heap...
template <typename ResultType_t>
struct Work
{
    Work() : Lower(0), Upper(0), Result(0) {}

    uv_work_t Request;
    v8::Persistent<v8::Function> Callback;

    int32_t         Lower;
    int32_t         Upper;
    ResultType_t    Result;
};

// Callbacks for exported methods...
namespace rng
{
    // Callback implementing JavaScript rng.getCorrections()...
    void getCorrections(const v8::FunctionCallbackInfo<v8::Value> &Arguments);

    // Callback implementing JavaScript rng.getRandom()...
    void getRandom(const v8::FunctionCallbackInfo<v8::Value> &Arguments);

    // Callback implementing JavaScript
    //  rng.getRandomAsync(function(error, result))...
    void getRandomAsync(const v8::FunctionCallbackInfo<v8::Value> &Arguments);

    // Callback implementing JavaScript rng.getRandomRange(lower, upper)...
    void getRandomRange(const v8::FunctionCallbackInfo<v8::Value> &Arguments);

    // Callback implementing JavaScript
    //  rng.getRandomRangeAsync(lower, upper, function(error, result))...
    void getRandomRangeAsync(const v8::FunctionCallbackInfo<v8::Value> &Arguments);

    // rng.getVersion()...
    void getVersion(const v8::FunctionCallbackInfo<v8::Value> &Arguments);

    // Callback implementing JavaScript rng.isAvailable()...
    void isAvailable(const v8::FunctionCallbackInfo<v8::Value> &Arguments);
}


