// LdrEx.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Tools.h"

void genData(std::string strInFile, std::string strOutFile)
{
	std::string strInContent;	 
	std::string strOutContent;
	int i = 0;
	if (ReadBinFile(strInFile, strInContent)){
		for (std::string::iterator it = strInContent.begin(); it != strInContent.end(); it++, i++)
		{
			//strOutContent += "0x";
			// 			char temp[8];
			// 			itoa(*it, temp, 16);
			std::string hexStr = std::to_string(*it);
			strOutContent += hexStr;
			strOutContent += ",";
			if (i > 50)
			{
				i = 0;
				strOutContent += "\n";
			}
		}
	}
	WriteBinFile(strOutFile, strOutContent);
}

int _tmain(int argc, _TCHAR* argv[])
{
	{
		std::string strInFile = "G:\\dev_code_x\\patchframe\\LdrEx\\netpass_unpack.exe";
		std::string strOutFile = "G:\\dev_code_x\\patchframe\\LdrEx\\netpass_unpack.txt";
		genData(strInFile, strOutFile);
	}
	{
		std::string strInFile = "G:\\dev_code_x\\patchframe\\Release\\patchframe.dll";
		std::string strOutFile = "G:\\dev_code_x\\patchframe\\Release\\patchframe.txt";
		genData(strInFile, strOutFile);
	}
	{
		std::string strInFile = "G:\\dev_code_x\\patchframe\\Debug\\patchframe.dll";
		std::string strOutFile = "G:\\dev_code_x\\patchframe\\Debug\\patchframe.txt";
		genData(strInFile, strOutFile);
	}

	return 0;
}

