

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