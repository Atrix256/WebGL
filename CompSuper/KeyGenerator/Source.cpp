#include <stdio.h>
#include <algorithm>
#include <array>
#include <vector>
#include <stdint.h>
#include <inttypes.h>
#include <boost/multiprecision/cpp_int.hpp>

#define PRIMENUMBERSTARTINDEX 26 // this is the first 3 digit prime number: 101

typedef int64_t int64;

std::vector<int64> g_primeNumbers;

//=================================================================================
int64 ExtendedEuclidianAlgorithm (int64 smaller, int64 larger, int64 &s, int64 &t)
{
    // make sure A <= B before starting
    bool swapped = false;
    if (larger < smaller)
    {
        swapped = true;
        std::swap(smaller, larger);
    }

    // set up our storage for the loop.  We only need the last two values so will
    // just use a 2 entry circular buffer for each data item
    std::array<int64, 2> remainders = { larger, smaller };
    std::array<int64, 2> ss = { 1, 0 };
    std::array<int64, 2> ts = { 0, 1 };
    size_t indexNeg2 = 0;
    size_t indexNeg1 = 1;

    // loop
    while (1)
    {
        // calculate our new quotient and remainder
        int64 newQuotient = remainders[indexNeg2] / remainders[indexNeg1];
        int64 newRemainder = remainders[indexNeg2] - newQuotient * remainders[indexNeg1];

        // if our remainder is zero we are done.
        if (newRemainder == 0)
        {
            // return our s and t values as well as the quotient as the GCD
            s = ss[indexNeg1];
            t = ts[indexNeg1];
            if (swapped)
                std::swap(s, t);
            return remainders[indexNeg1];
        }

        // calculate this round's s and t
        int64 newS = ss[indexNeg2] - newQuotient * ss[indexNeg1];
        int64 newT = ts[indexNeg2] - newQuotient * ts[indexNeg1];

        // store our values for the next iteration
        remainders[indexNeg2] = newRemainder;
        ss[indexNeg2] = newS;
        ts[indexNeg2] = newT;

        // move to the next iteration
        std::swap(indexNeg1, indexNeg2);
    }
}

//=================================================================================
void LoadPrimeNumbers ()
{
    FILE *file = fopen("primes-to-100k.txt", "rt");
    if (file)
    {
        int64 value;
        while (fscanf(file, "%I64d", &value) == 1)
            g_primeNumbers.push_back(value);
        fclose(file);
    }
}

//=================================================================================
void WaitForEnter ()
{
    printf("\nPress Enter to quit");
    fflush(stdin);
    getchar();
}

//=================================================================================
int64 CalculateBit (size_t bitIndex, const std::vector<int64> &keys, const std::vector<int64> &coefficients, int64 keysLCM)
{
    int64 ret = 0;
    const int64 bitMask = 1 << bitIndex;

    // now figure out how much to multiply each coefficient by to make it have the specified modulus residue (remainder)
    int result = 0;
    for (size_t i = 0, c = keys.size(); i < c; ++i)
    {
        // we either want this term to be 0 or 1 mod the key.  if zero, we can multiply by zero, and
        // not add anything into the bit value!
        if ((i & bitMask) == 0)
            continue;

        int64 s, t;
        ExtendedEuclidianAlgorithm(coefficients[i], keys[i], s, t);
        ret = (ret + ((coefficients[i] * t) % keysLCM)) % keysLCM;
    }

    return ret;
}

//=================================================================================
void CalculateBitsAndKeys (int64 numBits, std::vector<int64> &superPositionedBits, std::vector<int64> &keys, int64 &keysLCM)
{
    // size our arrays
    superPositionedBits.resize(size_t(numBits));
    keys.resize(1 << numBits);

    // set our keys to prime numbers that aren't super tiny.  The smallest key
    // determines how much error we can tolerate building up, just like FHE over integers.
    keysLCM = 1;
    for (size_t i = 0, c = keys.size(); i < c; ++i)
    {
        keys[i] = g_primeNumbers[PRIMENUMBERSTARTINDEX + i];
        keysLCM *= g_primeNumbers[PRIMENUMBERSTARTINDEX + i];
    }

    // calculate our co-efficients for each term, for the chinese remainder theorem.
    // We do it here because it can be re-used for each x value we are solving for
    // with these keys.
    const size_t c_numKeys = keys.size();
    std::vector<int64> coefficients;
    coefficients.resize(c_numKeys);
    fill(coefficients.begin(), coefficients.end(), 1);
    for (size_t i = 0; i < c_numKeys; ++i)
    {
        for (size_t j = 0; j < c_numKeys; ++j)
        {
            if (i != j)
                coefficients[i] *= keys[j];
        }
    }

    // calculate each x value
    for (size_t i = 0, c = superPositionedBits.size(); i < c; ++i)
        superPositionedBits[i] = CalculateBit(i, keys, coefficients, keysLCM);
}

//=================================================================================
bool TestResults (const std::vector<int64> &superPositionedBits, const std::vector<int64> &keys)
{
    bool ret = true;
    for (size_t bitIndex = 0, bitCount = superPositionedBits.size(); bitIndex < bitCount; ++bitIndex)
    {
        const int64 bitMask = 1 << bitIndex;
        for (size_t keyIndex = 0, keyCount = keys.size(); keyIndex < keyCount; ++keyIndex)
        {
            int64 desiredResult = ((keyIndex & bitMask) != 0) ? 1 : 0;
            
            if (superPositionedBits[bitIndex] % keys[keyIndex] != desiredResult)
            {
                ret = false;
                printf("ERROR: bit %i (%"PRId64") mod key %i (%"PRId64") did not equal %"PRId64"!\n",
                    bitIndex,
                    superPositionedBits[bitIndex],
                    keyIndex,
                    keys[keyIndex],
                    desiredResult
                );
            }
        }
    }
    return ret;
}

//=================================================================================
void WriteResults (const std::vector<int64> &superPositionedBits, const std::vector<int64> &keys, int64 keysLCM)
{
    FILE *file = fopen("results.txt", "w+t");
    if (file)
    {
        // write the bits
        fprintf(file, "bits = [");
        for (size_t i = 0, c = superPositionedBits.size(); i < c; ++i)
        {
            if (i > 0)
                fprintf(file, ", ");
            fprintf(file, "%"PRId64, superPositionedBits[i]);
        }
        fprintf(file, "] (mod %"PRId64")\n", keysLCM);

        // write the keys
        fprintf(file, "keys = [");
        for (size_t i = 0, c = keys.size(); i < c; ++i)
        {
            if (i > 0)
                fprintf(file, ", ");
            fprintf(file, "%"PRId64, keys[i]);
        }
        fprintf(file, "]\n");
        fclose(file);
    }
}

//=================================================================================
void BoostTest()
{
    using namespace boost::multiprecision;

    int128_t v = 1;

    // Do some fixed precision arithmetic:
    for (unsigned i = 1; i <= 20; ++i)
        v *= i;

    std::cout << v << std::endl; // prints 20!

    // Repeat at arbitrary precision:
    cpp_int u = 1;
    for (unsigned i = 1; i <= 100; ++i)
        u *= i;

    std::cout << u << std::endl; // prints 100!
}

//=================================================================================
int main (int argc, char **argv)
{
    printf("--KeyGenerator--\n\nGenerates superpositioned bit values and keys for use in superpositional\ncomputation using HE over the integers.\n\n");

    // Load the prime numbers
    LoadPrimeNumbers();

    // find out how many bits to generate for
    printf("How many input bits are there in your circuit?\n");
    int64 numBits;
    scanf("%I64d", &numBits);

    // calculate superpositioned bits and keys
    printf("\nCalculating Values...\n");
    std::vector<int64> superPositionedBits;
    std::vector<int64> keys;
    int64 keysLCM;
    CalculateBitsAndKeys(numBits, superPositionedBits, keys, keysLCM);

    // Verify our results
    printf("Done.\n\nVerifying results...\n");
    if (!TestResults(superPositionedBits, keys))
    {
        // TODO: assert or something
    }
    printf("Done.\n");

    // Write results to results.txt
    WriteResults(superPositionedBits, keys, keysLCM);
    printf("\nResults written to results.txt\n");

    // TODO: temp
    BoostTest();

    // done
    WaitForEnter();
    return 0;
}

/*

TODO:
* make the math use multi precision library

* update the document with this method and examples etc
* make projects that do stuff with the keys (like... N bit adder program, but others too!)
 * need to figure out bootstrapping, or modulus switching at some point to make it fully homomorphic, instead of just leveled
* if this is slow with higher numbers of bits, could multithread it! so far, very fast though.
* clean up code (long lines, comments, etc)
? do i need to have the PRI whatever macros for the fscanf too? i think perhaps so
* assert or something if the values don't pass the test in TestResults.

* should we use relatively prime numbers instead of prime numbers? input could be lowest key value, and then find other relatively prime numbers?
 * prime numbers might be fine though

!! PRIMENUMBERSTARTINDEX of 6 doesn't work when generating 2 bits. why??

*/