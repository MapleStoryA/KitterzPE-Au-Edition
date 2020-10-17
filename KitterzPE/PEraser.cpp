#include <Windows.h>
#include "PEraser.h"




void ErasePEHeader(HINSTANCE hModule)
{
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery((LPCVOID)hModule, &mbi, sizeof(mbi));
	VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect);
	ZeroMemory((PVOID)hModule, 4096);
	VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, NULL);
	FlushInstructionCache(GetCurrentProcess(), (LPCVOID)mbi.BaseAddress, mbi.RegionSize);
}

void EraseHeaders(HINSTANCE hModule)
{
	PIMAGE_DOS_HEADER pDoH;
	PIMAGE_NT_HEADERS pNtH;
	DWORD i, errSize, protect;
	if (!hModule) return;

	pDoH = (PIMAGE_DOS_HEADER)(hModule);
	pNtH = (PIMAGE_NT_HEADERS)((LONG)hModule + ((PIMAGE_DOS_HEADER)hModule)->e_lfanew);
	errSize = sizeof(IMAGE_DOS_HEADER);
	if (VirtualProtect(pDoH, errSize, PAGE_READWRITE, &protect))
	{
		for (i = 0; i < errSize; i++)
			*(BYTE*)((BYTE*)pDoH + i) = 0;
	}
	errSize = sizeof(IMAGE_NT_HEADERS);
	if (pNtH && VirtualProtect(pNtH, errSize, PAGE_READWRITE, &protect))
	{
		for (i = 0; i < errSize; i++)
			*(BYTE*)((BYTE*)pNtH + i) = 0;
	}
	return;
}



void PEraser::Erase(void * base) {
	EraseHeaders((HMODULE)base);
	ErasePEHeader((HMODULE)base);
}