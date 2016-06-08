// ldr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

#pragma comment(linker,"/subsystem:\"Windows\" /entry:\"mainCRTStartup\"")
LPVOID MemoryMyAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect, void* userdata)
{
	UNREFERENCED_PARAMETER(userdata);
	if ((LPVOID)0x400000 == address){
		static LPVOID pMemEXE = 0;
		if (!pMemEXE){
			pMemEXE = VirtualAlloc(address, size, allocationType, protect);
			if (pMemEXE)
				return pMemEXE;
		}
		else
			return pMemEXE;
	}
	return VirtualAlloc(address, size, allocationType, protect);
}
unsigned char *g_pMemEXE = 0;
int MyInit()
{
	g_pMemEXE = (unsigned char *)MemoryMyAlloc((LPVOID)(0x400000), 0x100000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE, 0);
	if (!g_pMemEXE)
	{
		MessageBoxA(0, "MemAlloc Failed !", "ff", 0);
		return 0;
	}
	return 0;
}
typedef int cb(void);
#pragma data_seg(".CRT$XIU")
static cb *autostart[] = { MyInit };
#pragma data_seg()

static int holdExeMem[1024 * 1024 * 5];//保持5M内存
int _tmain(int argc, _TCHAR* argv[])
{
	holdExeMem[0] = 0xFF;
	MessageBoxA(0, 0, 0, 0);

	typedef int(*PF_test)(LPVOID);
	PF_test pf_test = 0;
	HINSTANCE hMod = LoadLibraryA("patchframe.dll");
	pf_test = (PF_test)::GetProcAddress(hMod, "test");
	pf_test(g_pMemEXE);

	return 0;
}

