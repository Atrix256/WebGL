#include "shared.h"

//=================================================================================
void WaitForEnter ()
{
    std::cout << "\nPress Enter to quit";
    fflush(stdin);
    getchar();
}