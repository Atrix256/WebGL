#include "Shared/Shared.h"
#include <boost/multiprecision/cpp_int.hpp>

typedef boost::multiprecision::cpp_int TINT;

//=================================================================================
int main(int argc, char **argv)
{
    // read in the bits and keys if we can
    std::vector<TINT> superPositionedBits;
    std::vector<TINT> keys;
    std::cout << "Loading results.txt\n";
    if (ReadBitsKeys("results.txt", superPositionedBits, keys))
    {
        // we need an even number of bits to do adding!
        if (superPositionedBits.size() % 2 == 0)
        {
            std::cout << "file loaded!\n";
            const size_t numBitsAdding = superPositionedBits.size() / 2;

            // do our multi bit addition!
            // Note that we can initialize our carry bit to the value 0.  we don't
            // need to initialize it to a superpositioned bit value!
            TINT carryBit = 0;
            std::vector<TINT> adderResults;
            adderResults.resize(numBitsAdding);
            for (size_t i = 0; i < numBitsAdding; ++i)
                adderResults[i] = FullAdder(superPositionedBits[i], superPositionedBits[i+numBitsAdding], carryBit);

            // show superpositional result
            std::cout << "\nSuperpositional Adder Result:\n\n";
            for (size_t i = 0; i < numBitsAdding; ++i)
                std::cout << i << ": " << adderResults[i] << "\n\n";

            // Permute through results, make sure truth tables add up
            printf("Truth Tables Test:\n");
            const size_t maxValue = (1 << numBitsAdding) - 1;
            for (size_t a = 0; a <= maxValue; ++a)
            {
                for (size_t b = 0; b <= maxValue; ++b)
                {
                    // get the index of our key for this specific set of inputs
                    size_t keyIndex = (a << numBitsAdding) | b;

                    // decode result for this specific key index
                    size_t result = 0;
                    for (size_t i = 0; i < numBitsAdding; ++i)
                        result = result | (size_t((adderResults[i] % keys[keyIndex]) % 2) << i);

                    // show the result
                    std::cout << "  [" << keyIndex << "]  " << a << " + " << b << " = " << result << "\n";

                    // verify the result
                    if (result != ((a + b) & maxValue))
                    {
                        std::cout << "ERROR! incorrect value detected!";
                        // TODO: assert or something
                    }
                }
            }
        }
        else
        {
            std::cout << "file did not have an even number of bits!\n";
        }
    }
    else
    {
        std::cout << "could not load file!\n";
    }

    WaitForEnter();
    return 0;
}

/*
TODO:

* Assert if the error gets too large?

* show error levels? perhaps as percent of smaller key? maybe investigate error levels a bit to see what they are for each key.

* could have another program permute the results, to show it's independent of circuit that created the answer? i dunno.

* make keygen take command line params for number of bits (and prime or not / min value?) instead of asking user for input

! no, maybe more like, it does the adding, and then prompts you for numbers to add, and then does so only by decrypting with the specific keys
 * or maybe it'll take however many bits it was given and brute force check that it's correct for all numbers?

! NEXT: something more time consuming or impressive.  Like shor's algorithm or who knows what else.  CORDIC math perhaps to calculate sine since it's branchless?

*/