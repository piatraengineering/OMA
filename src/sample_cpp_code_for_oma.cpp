/*
Sample of how to incorporate C++ code into OMA
*/

#include <iostream>
#include "impdefs.h"

extern "C"

int subincpp()
{
	int omaprintf(char*);
     std::cout << "Hello World\n";	// output to terminal
     printf("hello World again.\n");
     return(1);
}
