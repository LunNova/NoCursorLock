// NoCursorLock.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include <tlhelp32.h>
#include <chrono>
#include <thread>
#include <iostream>

// sleep ms
const int sleepTime = 13337; // actually a prime number
using namespace std;

const wchar_t *GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

std::string GetLastErrorAsString() {
	DWORD errorMessageID = GetLastError();
	if (errorMessageID == 0)
		return "No error";

	LPSTR messageBuffer = NULL;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);
	LocalFree(messageBuffer);

	return message;
}

const wchar_t* target = GetWC("nvstreamer.exe");

typedef HINSTANCE(*fpLoadLibrary)(char*);

void inject(HANDLE process) {
	char* dllPath = _fullpath(NULL, "NoCursorLockDLL.dll", 0);
	if (!dllPath) {
		cout << "Failed to get DLL path, error: " << GetLastErrorAsString().c_str() << "\n";
		return;
	}
	HINSTANCE hDll = LoadLibrary(_T("KERNEL32"));
	fpLoadLibrary LoadLibraryAddr = (fpLoadLibrary)GetProcAddress(hDll, "LoadLibraryA");
	void* paramAddr = VirtualAllocEx(process, 0, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	bool memoryWritten = WriteProcessMemory(process, paramAddr, dllPath, strlen(dllPath) + 1, NULL);
	if (!CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryAddr, paramAddr, 0, 0)) {
		cout << "Failed to inject into process, error: " << GetLastErrorAsString().c_str() << "\n";
	}
	free(dllPath);
}

void acquireDebugPrivileges() {
	HANDLE token;
	TOKEN_PRIVILEGES tp;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (AdjustTokenPrivileges(token, false, &tp, sizeof(tp), NULL, NULL)) {
			cout << "Enabled debug privileges\n";
		}
		else {
			cout << "Failed to enable debug privileges, error: " << GetLastErrorAsString().c_str() << "\n";
		}
		CloseHandle(token);
	}
}

int main() {
	acquireDebugPrivileges();
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	int lastPid = 0;
	while (1) {
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry) == TRUE) {
			while (Process32Next(snapshot, &entry) == TRUE) {
				if (entry.th32ProcessID == lastPid)
					break;

				if (_wcsicmp(entry.szExeFile, target) == 0) {
					lastPid = entry.th32ProcessID;
					cout << "Injecting nvstreamer, pid " << entry.th32ProcessID << "\n";
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

					inject(hProcess);

					DWORD exitCode = 259;
					while (GetExitCodeProcess(hProcess, &exitCode)) {
						if (exitCode != STILL_ACTIVE) {
							break;
						}
						WaitForSingleObject(hProcess, 999999);
						Sleep(sleepTime);
					}

					CloseHandle(hProcess);
				}
			}
		}

		CloseHandle(snapshot);

		Sleep(sleepTime);
	}

	return 0;
}