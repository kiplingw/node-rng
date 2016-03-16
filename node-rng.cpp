/*
    node-rng, a library for accessing a true hardware random number generator
    Copyright (C) 2016 Cherit.ee Inc
*/

// Includes...

    // Our headers......
    #include "bindings.h"
    #include "node-rng.h"
    #include "random.h"

    // Standard C++...
    #include <cstdlib>

// Import namespaces...

    // Standard C++...
    using namespace std;

    // V8...
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;

// Module is initializing...
void OnLoad(Local<Object> Exports)
{
    // Initialize random number generator...
    RandomNumberGenerator::CreateSingleton();

    // Export our JavaScript method callbacks...
    NODE_SET_METHOD(Exports, "isAvailable",         rng::isAvailable);
    NODE_SET_METHOD(Exports, "getCorrections",      rng::getCorrections);
    NODE_SET_METHOD(Exports, "getRandom",           rng::getRandom);
    NODE_SET_METHOD(Exports, "getRandomAsync",      rng::getRandomAsync);
    NODE_SET_METHOD(Exports, "getRandomRange",      rng::getRandomRange);
    NODE_SET_METHOD(Exports, "getRandomRangeAsync", rng::getRandomRangeAsync);
    NODE_SET_METHOD(Exports, "getVersion",          rng::getVersion);

    // On de-initialization...
    node::AtExit(OnUnload);
}

// Module is cleaning up...
void OnUnload(void *)
{
    // Cleanup the random number generator singleton instance...
    RandomNumberGenerator::DestroySingleton();
}

// Export our initialization function...
NODE_MODULE(rng, OnLoad)

