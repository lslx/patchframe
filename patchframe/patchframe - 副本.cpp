// patchframe.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MemoryModule.h"
#define WIN32_LEAN_AND_MEAN
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <assert.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <malloc.h>
#include "detours/detours.h"
void* ReadLibrary(long* pSize, char* fileName) {
	long read;
	void* result;
	FILE* fp;

	fp = _tfopen(fileName, _T("rb"));
	if (fp == NULL)
	{
		_tprintf(_T("Can't open DLL file \"%s\"."), fileName);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	*pSize = ftell(fp);
	if (*pSize < 0)
	{
		fclose(fp);
		return NULL;
	}

	result = (unsigned char *)malloc(*pSize);
	if (result == NULL)
	{
		return NULL;
	}

	fseek(fp, 0, SEEK_SET);
	read = fread(result, 1, *pSize, fp);
	fclose(fp);
	if (read != static_cast<size_t>(*pSize))
	{
		free(result);
		return NULL;
	}

	return result;
}

void RunMemExe(char* fileName)
{
	void *data;
	long size;
	HMEMORYMODULE handle;
	TCHAR buffer[100];

	data = ReadLibrary(&size, fileName);
	if (data == NULL)
	{
		return;
	}

	handle = MemoryLoadLibrary(data, size);
	if (handle == NULL)
	{
		_tprintf(_T("Can't load library from memory.\n"));
		goto exit;
	}
	MemoryCallEntryPoint(handle);

	MessageBoxA(0, "ff", "ff", 0);
	// 	MemoryFreeLibrary(handle);
	// 
exit:
	return;
	// 	free(data);
}
//#define EXE_FILE TEXT("C:\\Users\\Administrator\\Desktop\\netpass\\netpass.exe")
#define EXE_FILE TEXT("C:\\Users\\Administrator\\Desktop\\netpass\\netpass_unpack.exe")

// why not hook the iat ? because if you call 3rd dll and the 3rd dll maybe call we care fuctions
//--
typedef HMODULE (WINAPI *PF_GetModuleHandleA)(LPCSTR lpModuleName);
PF_GetModuleHandleA pPF_GetModuleHandleA = 0;
HANDLE WINAPI hk_GetModuleHandleA(LPCSTR lpModuleName)
{
	return pPF_GetModuleHandleA(lpModuleName);
}
//--
typedef HMODULE (WINAPI *PF_GetModuleHandleW)(LPCWSTR lpModuleName);
PF_GetModuleHandleW pPF_GetModuleHandleW = 0;
HANDLE WINAPI hk_GetModuleHandleW(LPCWSTR lpModuleName)
{
	return pPF_GetModuleHandleW(lpModuleName);
}
//--
typedef BOOL(WINAPI *PF_GetModuleHandleExA)(DWORD dwFlags, LPCSTR lpModuleName, HMODULE * phModule);
PF_GetModuleHandleExA pPF_GetModuleHandleExA = 0; 
BOOL WINAPI hk_GetModuleHandleExA(DWORD dwFlags, LPCSTR lpModuleName, HMODULE * phModule)
{
	return pPF_GetModuleHandleExA(dwFlags, lpModuleName, phModule);
}
//--
typedef BOOL(WINAPI *PF_GetModuleHandleExW)(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE * phModule);
PF_GetModuleHandleExW pPF_GetModuleHandleExW = 0; 
BOOL WINAPI hk_GetModuleHandleExW(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE * phModule)
{
	return pPF_GetModuleHandleExW(dwFlags, lpModuleName, phModule);
}
//--
typedef DWORD(WINAPI *PF_GetModuleFileNameA)(HMODULE hModule, LPSTR lpFilename,DWORD nSize);
PF_GetModuleFileNameA pPF_GetModuleFileNameA = 0;
DWORD WINAPI hk_GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
	return pPF_GetModuleFileNameA(hModule, lpFilename, nSize);
}
//--
typedef DWORD(WINAPI *PF_GetModuleFileNameW)(HMODULE hModule, LPWSTR lpFilename,DWORD nSize);
PF_GetModuleFileNameW pPF_GetModuleFileNameW = 0;
DWORD WINAPI hk_GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
	return pPF_GetModuleFileNameW(hModule, lpFilename, nSize);
}
//--
typedef DWORD(WINAPI *PF_GetModuleFileNameExA)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize);
PF_GetModuleFileNameExA pPF_GetModuleFileNameExA = 0;
DWORD WINAPI hk_GetModuleFileNameExA(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
	return pPF_GetModuleFileNameExA(hProcess, hModule, lpFilename, nSize);
}
//--
typedef DWORD(WINAPI *PF_GetModuleFileNameExW)(HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
PF_GetModuleFileNameExW pPF_GetModuleFileNameExW = 0;
DWORD WINAPI hk_GetModuleFileNameExW(HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
	return pPF_GetModuleFileNameExW(hProcess, hModule, lpFilename, nSize);
}
typedef FARPROC (WINAPI *PF_GetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
PF_GetProcAddress pPF_GetProcAddress = 0;
FARPROC WINAPI hk_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	return pPF_GetProcAddress(hModule, lpProcName);
}
//--- 
typedef HACCEL(WINAPI *PF_LoadAcceleratorsA)(HINSTANCE hInstance, LPCSTR lpTableName);
PF_LoadAcceleratorsA pPF_LoadAcceleratorsA = 0;
HACCEL WINAPI hk_LoadAcceleratorsA(HINSTANCE hInstance, LPCSTR lpTableName)
{
	return pPF_LoadAcceleratorsA(hInstance, lpTableName);
}
typedef HACCEL(WINAPI *PF_LoadAcceleratorsW)(HINSTANCE hInstance, LPCWSTR lpTableName);
PF_LoadAcceleratorsW pPF_LoadAcceleratorsW = 0;
HACCEL WINAPI hk_LoadAcceleratorsW(HINSTANCE hInstance, LPCWSTR lpTableName)
{
	return pPF_LoadAcceleratorsW(hInstance, lpTableName);
}
typedef HCURSOR (WINAPI *PF_LoadCursorA)(HINSTANCE hInstance,  LPCSTR lpCursorName);
PF_LoadCursorA pPF_LoadCursorA = 0;
HCURSOR WINAPI hk_LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName)
{
	return pPF_LoadCursorA(hInstance, lpCursorName);
}
typedef HCURSOR(WINAPI *PF_LoadCursorW)(HINSTANCE hInstance, LPCWSTR lpCursorName);
PF_LoadCursorW pPF_LoadCursorW = 0;
HCURSOR WINAPI hk_LoadCursorW(HINSTANCE hInstance, LPCWSTR lpCursorName)
{
	return pPF_LoadCursorW(hInstance, lpCursorName);
}
typedef HICON(WINAPI *PF_LoadIconA)(HINSTANCE hInstance, LPCSTR lpIconName);
PF_LoadIconA pPF_LoadIconA = 0;
HICON WINAPI hk_LoadIconA(HINSTANCE hInstance, LPCSTR lpIconName)
{
	return pPF_LoadIconA(hInstance, lpIconName);
}
typedef HICON(WINAPI *PF_LoadIconW)(HINSTANCE hInstance, LPCWSTR lpIconName);
PF_LoadIconW pPF_LoadIconW = 0;
HICON WINAPI hk_LoadIconW(HINSTANCE hInstance, LPCWSTR lpIconName)
{
	return pPF_LoadIconW(hInstance, lpIconName);
}
typedef HANDLE (WINAPI *PF_LoadImageA)(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad);
PF_LoadImageA pPF_LoadImageA = 0;
HANDLE WINAPI hk_LoadImageA(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad)
{
	return pPF_LoadImageA(hInst, name, type, cx, cy, fuLoad);
}
typedef HANDLE(WINAPI *PF_LoadImageW)(HINSTANCE hInst, LPCWSTR name, UINT type, int cx, int cy, UINT fuLoad);
PF_LoadImageW pPF_LoadImageW = 0;
HANDLE WINAPI hk_LoadImageW(HINSTANCE hInst, LPCWSTR name, UINT type, int cx, int cy, UINT fuLoad)
{
	return pPF_LoadImageW(hInst, name, type, cx, cy, fuLoad);
}
typedef HMENU (WINAPI *PF_LoadMenuA)(HINSTANCE hInstance, LPCSTR lpMenuName);
PF_LoadMenuA pPF_LoadMenuA = 0;
HMENU WINAPI hk_LoadMenuA(HINSTANCE hInstance, LPCSTR lpMenuName)
{
	return pPF_LoadMenuA(hInstance, lpMenuName);
}
typedef HMENU(WINAPI *PF_LoadMenuW)(HINSTANCE hInstance, LPCWSTR lpMenuName);
PF_LoadMenuW pPF_LoadMenuW = 0;
HMENU WINAPI hk_LoadMenuW(HINSTANCE hInstance, LPCWSTR lpMenuName)
{
	return pPF_LoadMenuW(hInstance, lpMenuName);
}
typedef HGLOBAL (WINAPI *PF_LoadResource)(HMODULE hModule, HRSRC hResInfo);
PF_LoadResource pPF_LoadResource = 0;
HGLOBAL WINAPI hk_LoadResource(HMODULE hModule, HRSRC hResInfo)
{
	return pPF_LoadResource(hModule, hResInfo);
}
typedef int (WINAPI *PF_LoadStringA)(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax);
PF_LoadStringA pPF_LoadStringA = 0;
int WINAPI hk_LoadStringA(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
	return pPF_LoadStringA(hInstance, uID, lpBuffer, cchBufferMax);
}
typedef int (WINAPI *PF_LoadStringW)(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax);
PF_LoadStringW pPF_LoadStringW = 0;
int WINAPI hk_LoadStringW(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax)
{
	return pPF_LoadStringW(hInstance, uID, lpBuffer, cchBufferMax);
}
typedef HMODULE (WINAPI *PF_LoadLibraryA)(LPCSTR lpLibFileName);
PF_LoadLibraryA pPF_LoadLibraryA = 0;
HMODULE WINAPI hk_LoadLibraryA(LPCSTR lpLibFileName)
{
	return pPF_LoadLibraryA(lpLibFileName);
}
typedef HMODULE (WINAPI *PF_LoadLibraryW)(LPCWSTR lpLibFileName);
PF_LoadLibraryW pPF_LoadLibraryW = 0;
HMODULE WINAPI hk_LoadLibraryW(LPCWSTR lpLibFileName)
{
	return pPF_LoadLibraryW(lpLibFileName);
}
typedef HMODULE (WINAPI *PF_LoadLibraryExA)(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
PF_LoadLibraryExA pPF_LoadLibraryExA = 0;
HMODULE WINAPI hk_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	return pPF_LoadLibraryExA(lpLibFileName, hFile, dwFlags);
}
typedef HMODULE(WINAPI *PF_LoadLibraryExW)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
PF_LoadLibraryExW pPF_LoadLibraryExW = 0;
HMODULE WINAPI hk_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	return pPF_LoadLibraryExW(lpLibFileName, hFile, dwFlags);
}


void hook()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)(pPF_GetModuleHandleA = GetModuleHandleA), hk_GetModuleHandleA);
	DetourAttach(&(PVOID&)(pPF_GetModuleHandleW = GetModuleHandleW), hk_GetModuleHandleW);
	DetourAttach(&(PVOID&)(pPF_GetModuleHandleExA = GetModuleHandleExA), hk_GetModuleHandleExA);
	DetourAttach(&(PVOID&)(pPF_GetModuleHandleExW = GetModuleHandleExW), hk_GetModuleHandleExW);

	DetourAttach(&(PVOID&)(pPF_GetModuleFileNameA = GetModuleFileNameA), hk_GetModuleFileNameA);
	DetourAttach(&(PVOID&)(pPF_GetModuleFileNameW = GetModuleFileNameW), hk_GetModuleFileNameW);
// 	DetourAttach(&(PVOID&)(pPF_GetModuleFileNameExA = GetModuleFileNameExA), hk_GetModuleFileNameExA);
// 	DetourAttach(&(PVOID&)(pPF_GetModuleFileNameExW = GetModuleFileNameExW), hk_GetModuleFileNameExW);

	DetourTransactionCommit();
}
void unhook()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)pPF_GetModuleHandleA, hk_GetModuleHandleA);
	DetourDetach(&(PVOID&)pPF_GetModuleHandleW, hk_GetModuleHandleW);
	DetourDetach(&(PVOID&)pPF_GetModuleHandleExA, hk_GetModuleHandleExA);
	DetourDetach(&(PVOID&)pPF_GetModuleHandleExW, hk_GetModuleHandleExW);

	DetourDetach(&(PVOID&)pPF_GetModuleFileNameA, hk_GetModuleFileNameA);
	DetourDetach(&(PVOID&)pPF_GetModuleFileNameW, hk_GetModuleFileNameW);
// 	DetourDetach(&(PVOID&)pPF_GetModuleFileNameExA, hk_GetModuleFileNameExA);
// 	DetourDetach(&(PVOID&)pPF_GetModuleFileNameExW, hk_GetModuleFileNameExW);
	DetourTransactionCommit();
}

//GetModuleHandleA  --> GetModuleHandleW --> GetModuleHandleExW ,(GetModuleHandleExA   no test)
//some times its this call list , but not all the time, so, you must hook them all (4 functions)
//

// GetModuleFileNameExA  not call GetModuleFileNameExW
// GetModuleFileNameA  not call  GetModuleFileNameW
// so, you must hook  that all 4 functions

#include <psapi.h>
extern "C" __declspec(dllexport) int test(){

	HMODULE hMod = 0;
	GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"kernel32.dll",&hMod);
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, "kernel32.dll", &hMod);
	hMod = GetModuleHandleW(L"kernel32.dll");
	hMod = GetModuleHandleA("kernel32.dll");

	HMODULE xx = GetModuleHandleA("psapi.dll");
	{
		WCHAR BufferW[MAX_PATH];
		GetModuleFileNameExW(GetCurrentProcess(), hMod, BufferW, sizeof(BufferW)* sizeof(WCHAR));
		CHAR Buffer[MAX_PATH];
		GetModuleFileNameExA(GetCurrentProcess(), hMod, Buffer, sizeof(BufferW)* sizeof(CHAR));

		GetModuleFileNameW(hMod, BufferW, sizeof(BufferW)* sizeof(WCHAR));
		GetModuleFileNameA(hMod, Buffer, sizeof(BufferW)* sizeof(CHAR));

	}



	hook();
	GetClipboardData(0);
	unhook();

	RunMemExe(EXE_FILE);

	return 0;
}