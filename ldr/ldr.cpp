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
// 	g_pMemEXE = (unsigned char *)MemoryMyAlloc((LPVOID)(0x400000), 0x100000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE, 0);
// 	if (!g_pMemEXE)
// 	{
// 		MessageBoxA(0, "MemAlloc Failed !", "ff", 0);
// 		return 0;
// 	}
	return 0;
}
typedef int cb(void);
#pragma data_seg(".CRT$XIU")
static cb *autostart[] = { MyInit };
#pragma data_seg()

static int holdExeMem[1024 * 1024 * 5];//保持5M内存
int _tmain(int argc, _TCHAR* argv[])
{
	int main_f();
	return main_f();
	//--------
	holdExeMem[0] = 0xFF;
	MessageBoxA(0, 0, 0, 0);

	typedef int(*PF_test)(LPVOID);
	PF_test pf_test = 0;
	HINSTANCE hMod = LoadLibraryA("patchframe.dll");
	pf_test = (PF_test)::GetProcAddress(hMod, "test");
	pf_test(g_pMemEXE);

	return 0;
}

extern "C" IMAGE_DOS_HEADER __ImageBase;
LPWSTR* arglist;
int argsCount;


#define ALIGN_VALUE_UP(value, alignment)    (((value) + (alignment) - 1) & ~((alignment) - 1))
#define GET_HEADER_DICTIONARY(module, idx)  &(module)->headers->OptionalHeader.DataDirectory[idx]
#ifndef IMAGE_SIZEOF_BASE_RELOCATION
// Vista SDKs no longer define IMAGE_SIZEOF_BASE_RELOCATION!?
#define IMAGE_SIZEOF_BASE_RELOCATION (sizeof(IMAGE_BASE_RELOCATION))
#endif
static BOOL
PerformBaseRelocation(unsigned char * lpNewBase, ptrdiff_t delta)
{
	unsigned char *codeBase = lpNewBase;
	PIMAGE_BASE_RELOCATION relocation;
	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)codeBase;
	PIMAGE_NT_HEADERS new_header = (PIMAGE_NT_HEADERS)&((const unsigned char *)(codeBase))[dos_header->e_lfanew];
	new_header->OptionalHeader.ImageBase = (uintptr_t)codeBase;
	//PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	PIMAGE_DATA_DIRECTORY directory = &new_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	if (directory->Size == 0) {
		return (delta == 0);
	}

	relocation = (PIMAGE_BASE_RELOCATION)(codeBase + directory->VirtualAddress);
	for (; relocation->VirtualAddress > 0;) {
		DWORD i;
		unsigned char *dest = codeBase + relocation->VirtualAddress;
		unsigned short *relInfo = (unsigned short *)((unsigned char *)relocation + IMAGE_SIZEOF_BASE_RELOCATION);
		for (i = 0; i < ((relocation->SizeOfBlock - IMAGE_SIZEOF_BASE_RELOCATION) / 2); i++, relInfo++) {
			DWORD *patchAddrHL;
#ifdef _WIN64
			ULONGLONG *patchAddr64;
#endif
			int type, offset;

			// the upper 4 bits define the type of relocation
			type = *relInfo >> 12;
			// the lower 12 bits define the offset
			offset = *relInfo & 0xfff;

			switch (type)
			{
			case IMAGE_REL_BASED_ABSOLUTE:
				// skip relocation
				break;

			case IMAGE_REL_BASED_HIGHLOW:
				// change complete 32 bit address
				patchAddrHL = (DWORD *)(dest + offset);
				*patchAddrHL += (DWORD)delta;
				break;

#ifdef _WIN64
			case IMAGE_REL_BASED_DIR64:
				patchAddr64 = (ULONGLONG *)(dest + offset);
				*patchAddr64 += (ULONGLONG)delta;
				break;
#endif

			default:
				//printf("Unknown relocation: %d\n", type);
				break;
			}
		}

		// advance to next relocation block
		relocation = (PIMAGE_BASE_RELOCATION)(((char *)relocation) + relocation->SizeOfBlock);
	}
	return TRUE;
}
#pragma comment(linker, "/INCLUDE:__tls_used")
void NTAPI my_tls_callback(PVOID h, DWORD reason, PVOID pv)
{
	if (reason == DLL_PROCESS_ATTACH){
		MessageBox(NULL, "hi,this is tls callback", "title", MB_OK);
	}
	return;
}
void WINAPI MyInit2(PVOID h, DWORD reason, PVOID pv)
{
	if (reason == DLL_PROCESS_ATTACH){
		g_pMemEXE = (unsigned char *)MemoryMyAlloc((LPVOID)(0x400000), 0x100000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE, 0);
		if (!g_pMemEXE)
		{
			MessageBoxA(0, "MemAlloc Failed !", "ff", 0);
			return ;
		}
	}
	return ;
}
#pragma data_seg(".CRT$XLB")
//PIMAGE_TLS_CALLBACK p_thread_callback = my_tls_callback;
PIMAGE_TLS_CALLBACK p_thread_callback[] = { 0/*MyInit2*/, 0 };

#pragma data_seg()
int testTls()
{
	//use tls begin
	holdExeMem[0] = 0xFF;
	MessageBoxA(0, 0, 0, 0);

	typedef int(*PF_test)(LPVOID);
	PF_test pf_test = 0;
	HINSTANCE hMod = LoadLibraryA("patchframe.dll");
	pf_test = (PF_test)::GetProcAddress(hMod, "test");
	pf_test(g_pMemEXE);

	return 0;
	//use tls end 
}
#include "VMQuery.h"
#include "MemoryModule.h"
#include "Tools.h"
HMEMORYMODULE LoadMemDll(char* fileName)
{
	void *data;
	long size;
	HMEMORYMODULE handle;
	TCHAR buffer[100];

	data = File2Buffer(&size, fileName);
	if (data == NULL)
	{
		return 0;
	}

	handle = MemoryLoadLibrary(data, size);

	if (handle == NULL)
	{
		_tprintf(_T("Can't load library from memory.\n"));
		return 0;
	}
	//AddMemMod(handle); maybe xx
	return handle;
}

extern "C" LPVOID lpMyReserved = 0;
int main_f()
{	
	lpMyReserved = &__ImageBase;
	//simulation the host exe
	//HMEMORYMODULE hMemMod = LoadMemDll("patchframe.dll");
	//HMEMORYMODULE hMemMod = LoadMemDll("G:\\dev_code_x\\patchframe\\Debug\\patchframe.dll");
	HMODULE hMod = LoadLibraryA("patchframe.dll");
	//HMODULE hMod = LoadLibraryA("G:\\dev_code_x\\patchframe\\Debug\\patchframe.dll");

	// call in the dll and not return
	Sleep(INFINITE);
	DebugBreak();
	return 0;

	unsigned char *buf;
	unsigned int bufsize;
	buf = (unsigned char*)VirtualAlloc(NULL, bufsize + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);


	//move self to another place and run
	PIMAGE_DOS_HEADER dos_header = &__ImageBase;
	PIMAGE_NT_HEADERS old_header = (PIMAGE_NT_HEADERS)&((const unsigned char *)(&__ImageBase))[dos_header->e_lfanew];
	DWORD dwProtect = 0;
	BOOL bChge = VirtualProtectEx(GetCurrentProcess(), &__ImageBase, old_header->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READWRITE, &dwProtect);
	
	
	
	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);
	size_t alignedImageSize = ALIGN_VALUE_UP(old_header->OptionalHeader.SizeOfImage, sysInfo.dwPageSize);

	// reserve memory for image of library
	// XXX: is it correct to commit the complete memory region at once?
	//      calling DllEntry raises an exception if we don't...
	unsigned char *code = (unsigned char *)VirtualAlloc(NULL,
			alignedImageSize,
			MEM_RESERVE | MEM_COMMIT,
			PAGE_READWRITE
			);
		if (code == NULL) {
			SetLastError(ERROR_OUTOFMEMORY);
			return NULL;
		};
	//do copy
		memcpy(code, dos_header, alignedImageSize);
		ptrdiff_t locationDelta = (ptrdiff_t)(code - old_header->OptionalHeader.ImageBase);

		bool isRelocated = PerformBaseRelocation(code, locationDelta);
		//fix the mem flag

		HANDLE hProcess = GetCurrentProcess();
		BOOL bExpandRegions = FALSE;
		BOOL bOk = TRUE;
		PVOID pvAddress = NULL;
		while (bOk) {

			VMQUERY vmq;
			bOk = VMQuery(hProcess, pvAddress, &vmq);

			if (bOk) {
				// Construct the line to be displayed, and add it to the list box.
				TCHAR szLine[1024];
// 				ConstructRgnInfoLine(hProcess, &vmq, szLine, _countof(szLine));
// 				ListBox_AddString(hWndLB, szLine);

				if (bExpandRegions) {
					for (DWORD dwBlock = 0; bOk && (dwBlock < vmq.dwRgnBlocks);
						dwBlock++) {

// 						ConstructBlkInfoLine(&vmq, szLine, _countof(szLine));
// 						ListBox_AddString(hWndLB, szLine);

						// Get the address of the next region to test.
						pvAddress = ((PBYTE)pvAddress + vmq.BlkSize);
						if (dwBlock < vmq.dwRgnBlocks - 1) {
							// Don't query the memory info after the last block.
							bOk = VMQuery(hProcess, pvAddress, &vmq);
						}
					}
				}

				// Get the address of the next region to test.
				pvAddress = ((PBYTE)vmq.pvRgnBaseAddress + vmq.RgnSize);
			}
		}
		CloseHandle(hProcess);


	int argsCount;
	arglist = CommandLineToArgvW(GetCommandLineW(), &argsCount);
	if (!arglist)
		return 0;

}

