/*
    node-rng, a library for accessing a true hardware random number generator
    Copyright (C) 2016 Cherit.ee Inc
*/

// Includes...

    // Our headers...
    #include "singleton.h"

    // Libuv...
    #include <uv.h>

    // Standard C++...
    #include <string>

// Console explicit singleton class...
class RandomNumberGenerator : public ExplicitSingleton<RandomNumberGenerator>
{
    // Because we are a singleton, only ExplicitSingleton can control our
    //  creation...
    friend class ExplicitSingleton<RandomNumberGenerator>;

    // Public methods...
    public:

        // Number of times random number generator detected an internal problem
        //  that it had to correct before re-supplying a random number...
        uint32_t GetCorrections() const { return m_Corrections; }

        // Retrieve a 32-bit unsigned random number...
        uint32_t GetRandom32(bool &CorrectionDetected);

        // Retrieve a 64-bit unsigned random number...
        uint64_t GetRandom64(bool &CorrectionDetected);

        // Retrieve a 32-bit random number within the given inclusive range...
        int32_t GetRandomRange32(
            const int32_t Lower, const int32_t Upper, bool &CorrectionDetected);

        // Check if a random number generator is available...
        bool IsAvailable() const;

    // Private methods...
    private:

        // Default constructor...
        RandomNumberGenerator();

        // Deconstructor...
       ~RandomNumberGenerator();

    // Protected attributes...
    protected:

        // Type of random number generator to use..
        typedef enum
        {
            None        = 0,
            IntelSecureKey  /* Intel hardware random number generator. */

        }SourceType;

    // Protected methods...
    protected:

        // Number of times random number generator detected an internal problem
        //  that it had to correct before re-supplying a random number...
        uint32_t    m_Corrections;

        // For thread safety...
        uv_mutex_t  m_Mutex;

        // Type of random number generator to use...
        SourceType  m_SourceType;
};

