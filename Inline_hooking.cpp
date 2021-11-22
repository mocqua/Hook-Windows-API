#include <Windows.h>
#include <stdio.h>
FARPROC messAddress;
DWORD bytesRead;
DWORD bytesWritten;
BYTE messByte[6] = {  };
int __stdcall HookedMessageBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {

	printf("Text: %s  \nCaption: %s \n", (LPCSTR)lpText, (LPCSTR)lpCaption);

	WriteProcessMemory(GetCurrentProcess(), (LPVOID)messAddress, messByte, sizeof(messByte), &bytesWritten);

	return MessageBoxA(NULL, lpText, lpCaption, uType);
}

int main()
{
	MessageBoxA(0, "hello", "hello", MB_OK);
	messAddress = GetProcAddress(LoadLibraryA("user32.dll"), "MessageBoxA");

	ReadProcessMemory(GetCurrentProcess(), messAddress, messByte, 6, &bytesRead);
	void* hookedMessageBoxAddress = &HookedMessageBox;
	char patch[6] = { 0 };
	memcpy_s(patch, 1, "\x68", 1);
	memcpy_s(patch + 1, 4, &hookedMessageBoxAddress, 4);
	memcpy_s(patch + 5, 1, "\xC3", 1);
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)messAddress, patch, sizeof(patch), &bytesWritten);

	MessageBoxA(NULL, "hi", "hi", MB_OK);
	return 0;
}