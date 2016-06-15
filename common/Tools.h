
#ifndef __MY_TOOLS_HEADER
#define __MY_TOOLS_HEADER

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif
	void* ReadLibrary(long* pSize, char* fileName);
	bool WriteFileOver(char* pBuf, long size, char* fileName);
	bool WriteFileAdd(char* pBuf, long size, char* fileName);
	void* File2Buffer(long* pSize, const char* strPath);

#ifdef __cplusplus
}
#endif

#endif  // __MY_TOOLS_HEADER
