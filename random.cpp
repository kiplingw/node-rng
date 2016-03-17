/*
    node-rng, a library for accessing a true hardware random number generator
    Copyright (C) 2016 Cherit.ee Inc
*/

// Includes...

    // Ours...
    #include "random.h"

    // Standard C++
    #include <cstdint>
    #include <cstdlib>

// Using the standard namespace...
using namespace std;

// Default constructor...
RandomNumberGenerator::RandomNumberGenerator()
    : m_Corrections(0),
      m_SourceType(None)
{
    // Allocate mutex...
  ::uv_mutex_init(&m_Mutex);

    // Verify CPU supports CPUID instruction...

        // Storage for EFLAGS register...
        uint64_t    EFlagsBefore    = 0;
        uint64_t    EFlagsAfter     = 0;

        // Get EFLAGS register...
        asm volatile(
            "pushfq\n"
            "pop %0\n"
            : "=r"(EFlagsBefore));

        // Set EFLAGS' ID bit. This bit is modifiable only when the CPUID
        //  instruction is supported...
        asm volatile(
            "push %0\n"
            "popfq\n"
            :: "r"(EFlagsBefore ^ 0x200000)); /* Input operands */

        // Get the EFLAGS register again...
        asm volatile(
            "pushfq\n"
            "pop %0\n"
            : "=r"(EFlagsAfter));   /* Output operands */

        // Not supported if the bit isn't still set...
        if(!(EFlagsAfter & 0x200000))
            return;

    // Query CPU for capabilities...

        // Output registers...
        uint32_t EAX = 0;
        uint32_t EBX = 0;
        uint32_t ECX = 0;
        uint32_t EDX = 0;

        // Query...
        asm volatile(
            "cpuid"
            : "=a" (EAX), "=b" (EBX), "=c" (ECX), "=d" (EDX) /* Output operands */
            : "a" (1)); /* Input operands */

        // Check for RdRand instruction...
        if(ECX & (1 << 30))
            m_SourceType = IntelSecureKey;
}

// Retrieve a 32-bit unsigned random number...
uint32_t RandomNumberGenerator::GetRandom32(bool &CorrectionDetected)
{
    // Reset correction flag...
    CorrectionDetected = false;

    // Not supported...
    if(!IsAvailable())
        return 0;

    // Query for a 64-bit unsigned integer...
    uint64_t Random = GetRandom64(CorrectionDetected);

    // Split into two words...
    //uint32_t High   = static_cast<uint32_t>(Random >> 32U);
    uint32_t Low    = static_cast<uint32_t>(Random);

    // Return only one of them...
    return Low;
}

// Retrieve a 64-bit unsigned random number...
uint64_t RandomNumberGenerator::GetRandom64(bool &CorrectionDetected)
{
    // Location for result and carry flag...
    uint64_t    Result  = 0;
    uint8_t     Valid   = false;

    // Reset correction flag...
    CorrectionDetected = false;

    // Not supported...
    if(!IsAvailable())
        return 0;

    // Keep trying as long as the carry flag indicates a bad result...
    do
    {
        // Perform query and retrieve carry flag...
        asm volatile(
            "rdrand %0\n"
            "setc %1"
            : "=r" (Result), "=qm"(Valid)
            :
            : "rdx");

        // Remember failed attempts...
        if(!Valid)
        {
            CorrectionDetected = true;
          ::uv_mutex_lock(&m_Mutex);
          ++m_Corrections;
          ::uv_mutex_unlock(&m_Mutex);
        }
    }
    while(!Valid);

    // Return the new random number to caller...
    return Result;
}

// Retrieve a 32-bit random number within the given inclusive range...
int32_t RandomNumberGenerator::GetRandomRange32(
    const int32_t Lower, const int32_t Upper, bool &CorrectionDetected)
{
    // Not supported...
    if(!IsAvailable())
        return 0;

    // Calculate the range...
    const uint32_t Range = ::abs(Upper - Lower) + 1;

    // Get a random number...
    const uint32_t Random = GetRandom32(CorrectionDetected);

    // Scale and return within the inclusive range...
    return static_cast<int32_t>((Random % Range) + (Lower));
}

// Check if a random number generator is available...
bool RandomNumberGenerator::IsAvailable() const
{
    return (m_SourceType != None);
}

// Deconstructor...
RandomNumberGenerator::~RandomNumberGenerator()
{
    // Cleanup mutex...
  ::uv_mutex_destroy(&m_Mutex);
}


