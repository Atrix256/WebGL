#include "Shared/Shared.h"
#include <boost/multiprecision/cpp_int.hpp>

//=================================================================================
int main(int argc, char **argv)
{
    WaitForEnter();
    return 0;
}

/*
TODO:
* make this program read in results.txt formatted file

* verify that there are an even number of bits?

* break the bits in half and put them through the adder.

* Assert if the error gets too large?

* spit out the resulting values and keys

* could have another program permute the results, to show it's independant? i dunno.

* make keygen take command line params for number of bits (and prime or not / min value?) instead of asking user for input

! no, maybe more like, it does the adding, and then prompts you for numbers to add, and then does so only by decrypting with the specific keys
 * or maybe it'll take however many bits it was given and brute force check that it's correct for all numbers?

! NEXT: something more time consuming or impressive.  Like shor's algorithm or who knows what else.  CORDIC math perhaps since it's branchless?

*/