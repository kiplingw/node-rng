var rng = require('./build/Release/rng');

if(!rng.isAvailable())
{
  throw new Error("Hardware RNG is not available");
}

module.exports = rng;
