// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Windows.h>
#include "MinHook.h"

/*
#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif
*/

typedef int (WINAPI *MESSAGEBOXW)(HWND, LPCWSTR, LPCWSTR, UINT);
typedef int (WINAPI *CLIPCURSOR)(RECT*);
CLIPCURSOR fpClipCursor = NULL;
static bool enable = false;

int WINAPI DetourClipCursor(RECT* rect)
{
	return fpClipCursor(NULL);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if (!enable) {
		enable = true;
		if (MH_Initialize() != MH_OK)
		{
			MessageBoxA(NULL, "Failed to initialise MH", "", 0);
			return FALSE;
		}
		if (MH_CreateHook(&ClipCursor, &DetourClipCursor,
			reinterpret_cast<LPVOID*>(&fpClipCursor)) != MH_OK)
		{
			MessageBoxA(NULL, "Failed to create MH", "", 0);
			return FALSE;
		}
		if (MH_EnableHook(&ClipCursor) != MH_OK)
		{
			MessageBoxA(NULL, "Failed to enable MH", "", 0);
			return FALSE;
		}
	}
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

