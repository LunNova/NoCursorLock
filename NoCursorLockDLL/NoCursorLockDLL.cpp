// NoCursorLockDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NoCursorLockDLL.h"


// This is an example of an exported variable
NOCURSORLOCKDLL_API int nNoCursorLockDLL = 0;

// This is an example of an exported function.
NOCURSORLOCKDLL_API int fnNoCursorLockDLL(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see NoCursorLockDLL.h for the class definition
CNoCursorLockDLL::CNoCursorLockDLL()
{
	return;
}
