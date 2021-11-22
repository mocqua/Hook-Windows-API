#undef UNICODE
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tlHelp32.h>

BOOL SetWindowsHookEx3(char* dllPath, DWORD processId) {
	HMODULE hModDll = LoadLibraryA(dllPath);
	HOOKPROC procAddress = (HOOKPROC)GetProcAddress(hModDll, "_ReflectiveLoader@4");
	HANDLE threadHandleSnap = INVALID_HANDLE_VALUE;
	threadHandleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);
	DWORD threadId = 0;
	do {
		if (te32.th32OwnerProcessID == processId) {

			threadId = te32.th32ThreadID;
			HANDLE threadHandle = OpenThread(READ_CONTROL, FALSE, te32.th32ThreadID);

			if (threadHandle) {
				printf("Setting hook in thread with ID %d\n", threadId);
				HHOOK hookHandle = SetWindowsHookExA(WH_KEYBOARD, procAddress, hModDll, (DWORD)threadId);

				if (!hookHandle) {
					return false;
				}
				else
				{
					UnhookWindowsHookEx(hookHandle);
					return true;
				}
			}
		}
	} while (Thread32Next(threadHandleSnap, &te32));
}
int main()
{
	DWORD processID;
	char dllPath[MAX_PATH];
	GetFullPathNameA(("testdll32.dll"), MAX_PATH, (LPSTR)dllPath, NULL);
	printf_s("DLL %s \n", dllPath);
	char processName[100];
	printf("Process Name: ");
	scanf_s("%99s", processName, 99);
	processID = GetProcessId((LPSTR)processName);
	printf("Process PID: %d \n", processID);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	SetWindowsHookEx3((char*)dllPath, processID);

	return 0;
}