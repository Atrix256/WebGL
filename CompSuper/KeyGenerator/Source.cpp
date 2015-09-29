#include <stdio.h>
#include <algorithm>
#include <array>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include "Shared/Shared.h"

#define USEPRIMES 0 // if 1, will use primes from file. else will calcuate co-primes

#define PRIMENUMBERSTARTINDEX 26 // this is the first 3 digit prime number: 101
#define SMALLESTKEYVALUE 10      // if not using prime numbers, this will be the smallest key

//typedef int64_t TINT;
//typedef boost::multiprecision::int128_t TINT;
typedef boost::multiprecision::cpp_int TINT;

std::vector<TINT> g_primeNumbers;

//=================================================================================
TINT ExtendedEuclidianAlgorithm (TINT smaller, TINT larger, TINT &s, TINT &t)
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
    std::array<TINT, 2> remainders = { larger, smaller };
    std::array<TINT, 2> ss = { 1, 0 };
    std::array<TINT, 2> ts = { 0, 1 };
    size_t indexNeg2 = 0;
    size_t indexNeg1 = 1;

    // loop
    while (1)
    {
        // calculate our new quotient and remainder
        TINT newQuotient = remainders[indexNeg2] / remainders[indexNeg1];
        TINT newRemainder = remainders[indexNeg2] - newQuotient * remainders[indexNeg1];

        // if our remainder is zero we are done.
        if (newRemainder == 0)
        {
            // return our s and t values as well as the quotient as the GCD
            s = ss[indexNeg1];
            t = ts[indexNeg1];
            if (swapped)
                std::swap(s, t);

            // if t < 0, add the modulus divisor to it, to make it positive
            if (t < 0)
                t += smaller;
            return remainders[indexNeg1];
        }

        // calculate this round's s and t
        TINT newS = ss[indexNeg2] - newQuotient * ss[indexNeg1];
        TINT newT = ts[indexNeg2] - newQuotient * ts[indexNeg1];

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
    #if USEPRIMES
    FILE *file = fopen("primes-to-100k.txt", "rt");
    if (file)
    {
        TINT value;
        while (fscanf(file, "%I64d", &value) == 1)
            g_primeNumbers.push_back(value);
        fclose(file);
    }
    #endif
}

//=================================================================================
TINT CalculateBit (size_t bitIndex, const std::vector<TINT> &keys, const std::vector<TINT> &coefficients, TINT keysLCM)
{
    TINT ret = 0;
    const TINT bitMask = 1 << bitIndex;

    // now figure out how much to multiply each coefficient by to make it have the specified modulus residue (remainder)
    for (size_t i = 0, c = keys.size(); i < c; ++i)
    {
        // we either want this term to be 0 or 1 mod the key.  if zero, we can multiply by zero, and
        // not add anything into the bit value!
        if ((i & bitMask) == 0)
            continue;

        TINT s, t;
        ExtendedEuclidianAlgorithm(coefficients[i], keys[i], s, t);
        ret = (ret + ((coefficients[i] * t) % keysLCM)) % keysLCM;
    }

    return ret;
}

//=================================================================================
bool KeyIsCoprime (std::vector<TINT> &keys, size_t keyIndex, TINT& value)
{
    for (size_t index = 0; index < keyIndex; ++index)
    {
        TINT s, t;
        TINT gcd = ExtendedEuclidianAlgorithm(value, keys[index], s, t);
        if (gcd != 1)
            return false;
    }

    return true;
}

//=================================================================================
void MakeKey (std::vector<TINT> &keys, size_t keyIndex)
{
#if USEPRIMES
    keys[keyIndex] = g_primeNumbers[PRIMENUMBERSTARTINDEX + keyIndex];
#else
    TINT nextNumber = keyIndex > 0 ? (keys[keyIndex - 1] + 1) : TINT(SMALLESTKEYVALUE);
    while (1)
    {
        if (KeyIsCoprime(keys, keyIndex, nextNumber))
        {
            keys[keyIndex] = nextNumber;
            return;
        }
        nextNumber++;
    }
#endif
}

//=================================================================================
void CalculateBitsAndKeys (int numBits, std::vector<TINT> &superPositionedBits, std::vector<TINT> &keys, TINT &keysLCM)
{
    // size our arrays
    superPositionedBits.resize(size_t(numBits));
    keys.resize(size_t(1) << superPositionedBits.size());

    // set our keys to prime numbers that aren't super tiny.  The smallest key
    // determines how much error we can tolerate building up, just like FHE over integers.
    keysLCM = 1;
    for (size_t i = 0, c = keys.size(); i < c; ++i)
    {
        MakeKey(keys, i);
        keysLCM *= keys[i];
    }

    // calculate our co-efficients for each term, for the chinese remainder theorem.
    // We do it here because it can be re-used for each x value we are solving for
    // with these keys.
    const size_t c_numKeys = keys.size();
    std::vector<TINT> coefficients;
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
bool TestResults (const std::vector<TINT> &superPositionedBits, const std::vector<TINT> &keys)
{
    bool ret = true;
    for (size_t bitIndex = 0, bitCount = superPositionedBits.size(); bitIndex < bitCount; ++bitIndex)
    {
        const TINT bitMask = 1 << bitIndex;
        for (size_t keyIndex = 0, keyCount = keys.size(); keyIndex < keyCount; ++keyIndex)
        {
            TINT desiredResult = ((keyIndex & bitMask) != 0) ? 1 : 0;
            
            if (superPositionedBits[bitIndex] % keys[keyIndex] != desiredResult)
            {
                ret = false;
                std::cout << "ERROR: bit " << bitIndex << " (" << superPositionedBits[bitIndex] << ") "
                          << "mod key " << keyIndex << " (" << keys[keyIndex] << ") "
                          << "did not equal " << desiredResult << "!\n";
            }
        }
    }
    return ret;
}

//=================================================================================
int main (int argc, char **argv)
{
    std::cout << "--KeyGenerator--\n\nGenerates superpositioned bit values and keys for use in superpositional\ncomputation using HE over the integers.\n\n";

    // Load the prime numbers
    LoadPrimeNumbers();

    // find out how many bits to generate for
    std::cout << "How many input bits are there in your circuit?\n";
    int numBits;
    scanf("%i", &numBits);

    // calculate superpositioned bits and keys
    std::cout << "\nCalculating Values...\n";
    std::vector<TINT> superPositionedBits;
    std::vector<TINT> keys;
    TINT keysLCM;
    CalculateBitsAndKeys(numBits, superPositionedBits, keys, keysLCM);

    // Verify our results
    std::cout << "Done.\n\nVerifying results...\n";
    if (!TestResults(superPositionedBits, keys))
        std::cout << "Results Invalid!!\n";
    std::cout << "Done.\n";

    // Write results to results.txt
    WriteBitsKeys("results.txt", superPositionedBits, keys);
    std::cout << "\nResults written to results.txt\n";

    // done
    WaitForEnter();
    return 0;
}