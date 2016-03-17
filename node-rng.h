/*
    node-rng, a library for accessing a true hardware random number generator
    Copyright (C) 2016 Cherit.ee Inc
*/

// Includes...

    // Node.js
    #include <node.h>

// Module version...
#define NODE_RNG_VERSION_MAJOR  0
#define NODE_RNG_VERSION_MINOR  1
#define NODE_RNG_VERSION_PATCH  1
#define NODE_RNG_VERSION_STRING "0.1.1"

// Module is initializing...
void OnLoad(v8::Local<v8::Object> Exports);

// Module is cleaning up...
void OnUnload(void *);

