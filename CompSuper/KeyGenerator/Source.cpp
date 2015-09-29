#include <stdio.h>
#include <algorithm>
#include <array>
#include <vector>
#include <stdint.h>
#include <inttypes.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>
#include <fstream>

#define PRIMENUMBERSTARTINDEX 26 // this is the first 3 digit prime number: 101

typedef boost::multiprecision::cpp_int TINT;

std::vector<TINT> g_primeNumbers;

//=================================================================================
TINT ExtendedEuclidianAlgorithm(TINT smaller, TINT larger, TINT &s, TINT &t)
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
    FILE *file = fopen("primes-to-100k.txt", "rt");
    if (file)
    {
        TINT value;
        while (fscanf(file, "%I64d", &value) == 1)
            g_primeNumbers.push_back(value);
        fclose(file);
    }
}

//=================================================================================
void WaitForEnter ()
{
    std::cout << "\nPress Enter to quit";
    fflush(stdin);
    getchar();
}

//=================================================================================
TINT CalculateBit(size_t bitIndex, const std::vector<TINT> &keys, const std::vector<TINT> &coefficients, TINT keysLCM)
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
void CalculateBitsAndKeys(int numBits, std::vector<TINT> &superPositionedBits, std::vector<TINT> &keys, TINT &keysLCM)
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
bool TestResults(const std::vector<TINT> &superPositionedBits, const std::vector<TINT> &keys)
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
void WriteResults(const std::vector<TINT> &superPositionedBits, const std::vector<TINT> &keys, TINT keysLCM)
{
    std::ofstream file;
    file.open("results.txt", std::ios::out | std::ios::trunc);

    if (!file.is_open())
        return;

    // write the bits
    file << "bits = [";
    for (size_t i = 0, c = superPositionedBits.size(); i < c; ++i)
    {
        if (i > 0)
            file << ", ";
        file << superPositionedBits[i];
    }
    file <<  "] (mod " << keysLCM << ")\n";

    // write the keys
    file << "keys = [";
    for (size_t i = 0, c = keys.size(); i < c; ++i)
    {
        if (i > 0)
            file << ", ";
        file << keys[i];
    }
    file << "]\n";

    file.close();
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
    {
        // TODO: assert or something
    }
    std::cout << "Done.\n";

    // Write results to results.txt
    WriteResults(superPositionedBits, keys, keysLCM);
    std::cout << "\nResults written to results.txt\n";

    // done
    WaitForEnter();
    return 0;
}

/*

TODO:

* switch to using coprime sets, instead of prime numbers.  there are more numbers available, which means smaller numbers for same results

* update the document with this method and examples etc
* make projects that do stuff with the keys (like... N bit adder program, but others too!)
 * need to figure out bootstrapping, or modulus switching at some point to make it fully homomorphic, instead of just leveled
* if this is slow with higher numbers of bits, could multithread it! so far, very fast though.
* clean up code (long lines, comments, etc)
* assert or something if the values don't pass the test in TestResults.

*/