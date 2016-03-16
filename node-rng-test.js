/*
    node-rng, a library for accessing a true hardware random number generator
    Copyright (C) 2016 Cherit.ee Inc
*/

// Try to load the module...
var rng = require('./build/Debug/rng');
var fs = require('fs');

// Verify random number generator is available...
if(!rng.isAvailable())
    return 1;

// Open standard out device...
var fd = fs.openSync('/dev/stdout', 'w');

// Output raw integers to stdout...
for(;;)
{
    // Get a random integer...
    var result = rng.getRandom();

    // Load in raw binary form into a buffer...
    var buffer = new Buffer(4);
    buffer.writeUInt32LE(result, 0);

    // Write out...
    fs.writeSync(fd, buffer, 0, 4);

    delete buffer;
}

fs.closeSync(fd);

