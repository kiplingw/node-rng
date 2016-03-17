/*
    node-rng, a library for accessing a true hardware random number generator
    Copyright (C) 2016 Cherit.ee Inc
*/

// Try to load the module...
rng = require('./build/Debug/rng');

// Show the version...
console.log("Version: ", rng.getVersion());

// Verify random number generator is available...
console.log("Is available?: ", rng.isAvailable());

// Get some sample output for random numbers synchronously...
for(i = 0; i < 10; ++i)
    console.log("Synchronous random number =", rng.getRandom());

// Get some sample output for random numbers in a range asynchronously...
for(i = 0; i < 10; ++i)
{
    // Pick a random number in the range of -10 to 10 inclusive...
    var lower = rng.getRandomRange(-10, 10);

    // Pick another random number above the first...
    var upper = lower + rng.getRandomRange(1, 10);

    // Pick some random number within the inclusive range of the previous two,
    //  but asynchronously...
    rng.getRandomRangeAsync(lower, upper, function(err, result) {

        if(err)
            console.log(err);

        console.log("Asynchronous random number =", result);
    });
}

// Get some sample output for random numbers unbound asynchronously...
for(i = 0; i < 10; ++i)
{
    // Pick some other random number asynchronously...
    rng.getRandomAsync(function(err, result) {

        if(err)
            console.log(err);

        console.log("Asynchronous random number =", result);
    });
}

console.log("Number of corrections required:", rng.getCorrections());

