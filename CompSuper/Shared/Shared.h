#include <vector>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <algorithm>

void WaitForEnter ();

//=================================================================================
template <typename T>
bool Decrypt (T key, T value)
{
    return ((T % T) % 2) == 1;
}

//=================================================================================
template <typename T>
T Encrypt (T key, bool value)
{
    T ret = key + (value ? 1 : 0);
    return ret;
}

//=================================================================================
template <typename T>
T XOR (T A, T B)
{
    return A + B;
}

//=================================================================================
template <typename T>
T AND (T A, T B)
{
    return A * B;
}

//=================================================================================
template <typename T>
T FullAdder (T A, T B, T &carryBit)
{
    // homomorphically add the encrypted bits A and B
    // return the single bit sum, and put the carry bit into carryBit
    // From http://en.wikipedia.org/w/index.php?title=Adder_(electronics)&oldid=381607326#Full_adder
    T sumBit = XOR(XOR(A, B), carryBit);
    carryBit = XOR(AND(A, B), AND(carryBit, XOR(A, B)));
    return sumBit;
}

//=================================================================================
template <typename T>
bool WriteBitsKeys (const char *fileName, const std::vector<T> &superPositionedBits, const std::vector<T> &keys)
{
    std::ofstream file;
    file.open("results.txt", std::ios::out | std::ios::trunc);

    if (!file.is_open())
        return false;

    // write the number of bits
    file << superPositionedBits.size() << "\n";

    // write the bits
    for (const T &v : superPositionedBits)
        file << v << "\n";

    // write the keys
    for (const T &v : keys)
        file << v << "\n";

    file.close();
    return true;
}

//=================================================================================
template <typename T>
bool ReadBitsKeys (const char *fileName, std::vector<T> &superPositionedBits, std::vector<T> &keys)
{
    std::ifstream file;
    file.open("results.txt");

    if (!file.is_open())
        return false;

    do
    {
        // read the number of bits
        size_t numBits;
        file >> numBits;
        if (file.fail())
            break;

        // resize the data arrays
        superPositionedBits.resize(numBits);
        keys.resize(1 << numBits);

        // read the bits
        for (T &v : superPositionedBits)
            file >> v;

        // read the keys
        for (T &v : keys)
            file >> v;
    }
    while (0);

    bool ret = !file.fail();
    file.close();
    return ret;
}