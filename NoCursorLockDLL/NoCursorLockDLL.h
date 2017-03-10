// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NOCURSORLOCKDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NOCURSORLOCKDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef NOCURSORLOCKDLL_EXPORTS
#define NOCURSORLOCKDLL_API __declspec(dllexport)
#else
#define NOCURSORLOCKDLL_API __declspec(dllimport)
#endif

// This class is exported from the NoCursorLockDLL.dll
class NOCURSORLOCKDLL_API CNoCursorLockDLL {
public:
	CNoCursorLockDLL(void);
	// TODO: add your methods here.
};

extern NOCURSORLOCKDLL_API int nNoCursorLockDLL;

NOCURSORLOCKDLL_API int fnNoCursorLockDLL(void);
