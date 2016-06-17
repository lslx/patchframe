

#include <windows.h>
#include <winnt.h>
#include <stddef.h>
#include <stdint.h>
#include <tchar.h>
#include <stdio.h>
#ifdef DEBUG_OUTPUT
#endif

#if _MSC_VER
// Disable warning about data -> function pointer conversion
#pragma warning(disable:4055)
#endif

#include "Tools.h"

#ifdef DEBUG_OUTPUT
static void
OutputLastError(const char *msg)
{
    LPVOID tmp;
    char *tmpmsg;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&tmp, 0, NULL);
    tmpmsg = (char *)LocalAlloc(LPTR, strlen(msg) + strlen(tmp) + 3);
    sprintf(tmpmsg, "%s: %s", msg, tmp);
    OutputDebugString(tmpmsg);
    LocalFree(tmpmsg);
    LocalFree(tmp);
}
#endif


void* ReadLibrary(long *pSize, char* fileName) {
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
bool WriteFileOver(char* pBuf, long size, char* fileName) {
	long read;
	FILE* fp;

	fp = _tfopen(fileName, _T("wb"));
	if (fp == false)
	{
		_tprintf(_T("Can't open file \"%s\"."), fileName);
		return false;
	}

	fseek(fp, 0, SEEK_SET);
	//long old_size = ftell(fp);
	long write_size = fwrite(pBuf, sizeof(char), sizeof(char)*size, fp);
	//long old_size = ftell(fp);
	fclose(fp);
	if (write_size != static_cast<size_t>(size))
	{
		return false;
	}

	return true;
}
bool WriteFileAdd(char* pBuf, long size, char* fileName) {
	long read;
	FILE* fp;

	fp = _tfopen(fileName, _T("a+"));
	if (fp == false)
	{
		_tprintf(_T("Can't open file \"%s\"."), fileName);
		return false;
	}

	fseek(fp, 0, SEEK_END);
	//long old_size = ftell(fp);
	long write_size = fwrite(pBuf, sizeof(char),sizeof(char)*size, fp);
	//long old_size = ftell(fp);
	fclose(fp);
	if (write_size != static_cast<size_t>(size))
	{
		return false;
	}

	return true;
}
void* File2Buffer(long* pSize, const char* strPath)
{
	HANDLE hFile = ::CreateFileA(strPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD nFileSize = ::GetFileSize(hFile, NULL);
		char* lpBuffer = (char *)LocalAlloc(LPTR, nFileSize);
		DWORD nNumberOfBytesRead;
		BOOL bRet = ::ReadFile(hFile, lpBuffer, nFileSize, &nNumberOfBytesRead, NULL);// use a loop ?
		if (nFileSize != nNumberOfBytesRead){
			DebugBreak();
			return 0;
		}
		*pSize = nFileSize;
		CloseHandle(hFile);
		return lpBuffer;
	}
	return 0;
}

// void Buffer2File(const wchar_t* strPath, const wchar_t* strContent, const int nContentSize)
// {
// 	HANDLE hFile = ::CreateFile(strPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);
// 	if (hFile != INVALID_HANDLE_VALUE)
// 	{
// 		DWORD nNumberOfBytesWritten;
// 		::WriteFile(hFile, strContent, nContentSize, &nNumberOfBytesWritten, NULL);
// 
// 		// TODO ...
// 
// 		CloseHandle(hFile);
// 	}
//



#define RVATOVA(base, offset) (((INT)(base) + (INT)(offset)))
#define VATORVA(base, addr) ((INT)(addr) - (INT)(base))
#define NTHEADER(hModule)   ((PIMAGE_NT_HEADERS)RVATOVA((hModule), ((PIMAGE_DOS_HEADER)(hModule))->e_lfanew))
#define DATADIRECTORY(pNtHeader, nIndex) &(pNtHeader)->OptionalHeader.DataDirectory[(nIndex)]
#define VALIDRANGE(value, base, size) (((DWORD)(value) >= (DWORD)(base)) && ((DWORD)(value)<((DWORD)(base)+(DWORD)(size))))
#define DLLENTRY(hModule) ((DllEntryProc)RVATOVA ((DWORD)(hModule), NTHEADER(hModule)->OptionalHeader.AddressOfEntryPoint))

#define ENTRYRVA(hModule) (NTHEADER(hModule)->OptionalHeader.AddressOfEntryPoint)
#define SIZEOFIMAGE(hModule) (NTHEADER(hModule)->OptionalHeader.SizeOfImage)
#define IMAGEBASE(hModule) (NTHEADER(hModule)->OptionalHeader.ImageBase)

#ifndef IMAGE_SIZEOF_BASE_RELOCATION
#define IMAGE_SIZEOF_BASE_RELOCATION (sizeof(IMAGE_BASE_RELOCATION))
#endif

#define DLL_MODULE_ATTACH  DLL_PROCESS_DETACH + 10
#define DLL_MODULE_DETACH  DLL_MODULE_ATTACH + 1



static long Rva2FileOffset(PIMAGE_NT_HEADERS pNtHeader, long RVA, long *SizeOfRawData)
{
	if ((RVA == 0) || (pNtHeader == NULL))
	{
		return (0);
	}

	int i;
	PIMAGE_SECTION_HEADER Sections = IMAGE_FIRST_SECTION(pNtHeader);

	for (i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	{
		if (VALIDRANGE(RVA, Sections[i].VirtualAddress, Sections[i].Misc.VirtualSize))
		{
			if (Sections[i].PointerToRawData)
			{
				*SizeOfRawData = Sections[i].SizeOfRawData;
				return (Sections[i].PointerToRawData + (RVA - Sections[i].VirtualAddress));
			}
		}
	}

	*SizeOfRawData = 0;
	return 0;
}

static long rva_to_raw(PIMAGE_NT_HEADERS nt_headers, long rva)
{
	long size_of_rawdata;
	return Rva2FileOffset(nt_headers, rva, &size_of_rawdata);
}

static long va_to_raw(PIMAGE_NT_HEADERS nt_headers, long va)
{
	long image_base = nt_headers->OptionalHeader.ImageBase;
	long rva = va - image_base;
	long size_of_rawdata;
	return Rva2FileOffset(nt_headers, rva, &size_of_rawdata);
}


PVOID GetProcEATAddress(PVOID pRawData, PCHAR lpFunName)
{
	PIMAGE_DATA_DIRECTORY directory = (PIMAGE_DATA_DIRECTORY)DATADIRECTORY(NTHEADER(pRawData), IMAGE_DIRECTORY_ENTRY_EXPORT);

	if (directory->Size == 0)
	{
		return NULL;
	}

	PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)((char*)pRawData + rva_to_raw(NTHEADER(pRawData), directory->VirtualAddress));

	if (exports->NumberOfNames == 0 || exports->NumberOfFunctions == 0)
	{
		return NULL;
	}
	LPDWORD pAddressOfFunctions = (LPDWORD)((char*)pRawData + rva_to_raw(NTHEADER(pRawData), exports->AddressOfFunctions));
	//LPDWORD pAddressOfFunctions =	(LPDWORD)(rva_to_raw(NTHEADER(pRawData), exports->AddressOfFunctions));
	LPWORD  pAddressOfOrdinals = (LPWORD)((char*)pRawData + rva_to_raw(NTHEADER(pRawData), exports->AddressOfNameOrdinals));
	LPDWORD pAddressOfNames = (LPDWORD)((char*)pRawData + rva_to_raw(NTHEADER(pRawData), exports->AddressOfNames));

	int i;
	char *pName;
	for (i = 0; i < exports->NumberOfNames; i++)
	{
		pName = (char*)((char*)pRawData + rva_to_raw(NTHEADER(pRawData), pAddressOfNames[i]));
		if (_stricmp(pName, lpFunName) == 0)
		{
			//return  (LPDWORD)rva_to_raw(NTHEADER(pRawData), (long)(pAddressOfFunctions + pAddressOfOrdinals[i]));
			//return  (LPDWORD)(long)(pAddressOfFunctions + pAddressOfOrdinals[i]);
			//(FARPROC)(codeBase + (*(DWORD *)(codeBase + exports->AddressOfFunctions + (idx * 4))));
			//(char*)pRawData + (long)pAddressOfFunctions + pAddressOfOrdinals[i];
			DWORD tt = rva_to_raw(NTHEADER(pRawData), (long)(pAddressOfFunctions[(int)pAddressOfOrdinals[i]]));
			LPDWORD temp = (LPDWORD)((char*)pRawData + tt);
			return temp;
		}
	}
	return NULL;
}


char* GetRefLdrOffset(char* pRawData, char* lpFunName)
{
	PVOID pdwEATAddr = GetProcEATAddress(pRawData, lpFunName);
	return (char*)pdwEATAddr;
}
