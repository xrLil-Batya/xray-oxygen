////////////////////////////////////////////////////
// Author: ForserX
// Task  : Parsing current branch and commit hash
////////////////////////////////////////////////////
// Specital for X-Ray Oxygen Project | 15.07.2018 //
////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <fstream>
#include <istream>
#include <sstream>

#if __has_include("hack.appveyor")
#define ITS_CI_BUILD
// https://www.appveyor.com/docs/environment-variables/
#define NewStr(str) #str
#define xstr(ToStr) NewStr(ToStr)
#endif 

std::vector<std::string> Split(std::string Str, size_t StrSize, char splitCh) noexcept
{
	std::vector<std::string> Result;
	std::string temp_str = Str;

	size_t SubStrBeginCursor = 0;
	size_t Len = 0;
	for (size_t StrCursor = 0; StrCursor < StrSize; ++StrCursor)
	{
		if (Str[StrCursor] == splitCh)
		{
			//Don't create empty string
			if ((StrCursor - 1 - SubStrBeginCursor) > 0)
			{
				Len = StrCursor - 1 - SubStrBeginCursor;
				temp_str = Str.substr(SubStrBeginCursor, Len);
				Result.push_back(temp_str);
				SubStrBeginCursor = StrCursor + 1;
			}
		}
	}

	Result.push_back(Str.substr(SubStrBeginCursor, Str.length() - Len));
	return Result;
}

int main()
{
#ifndef ITS_CI_BUILD
	std::ifstream *Reader = nullptr;
	std::string PathFile = "../../../.git/";

	// Get repo data 
	std::vector<std::string> Directories;
	std::string BranchName;
	Reader = new std::ifstream(PathFile + "HEAD");
	std::getline(*Reader, BranchName);
	Directories = Split(BranchName, BranchName.size(), '/');
	BranchName = Directories[Directories.size() - 1];

	// РџРѕР»СѓС‡Р°РµРј С„Р°Р№Р»
	PathFile += "refs/heads/" + BranchName;
	Reader->close();
	Reader = new std::ifstream(PathFile);

	// РџРѕР»СѓС‡Р°РµРј С…РµС€ РєРѕРјРјРёС‚Р°
	std::string hash;
	std::getline(*Reader, hash);
	Reader->close();
	delete Reader;
#endif
	// Запишем полученные данные в oxy_version
	std::stringstream HeaderString;
	HeaderString << "#pragma once" << std::endl;
	
#ifdef ITS_CI_BUILD
	HeaderString << "#define _AUTHOR " << "\"" << xstr(APPVEYOR_REPO_COMMIT_AUTHOR) << "\"" << std::endl;
	HeaderString << "#define _BRANCH " << "\"" << xstr(APPVEYOR_REPO_BRANCH) << "\"" << std::endl;
	HeaderString << "#define _HASH " << "\"" << xstr(APPVEYOR_REPO_COMMIT) << "\"" << std::endl;
#else
	HeaderString << "#define _BRANCH " << "\"" << BranchName << "\"" << std::endl;
	HeaderString << "#define _HASH " << "\"" << hash << "\"" << std::endl;
#endif
	std::ofstream Writter("oxy_version.h");
	Writter.write(HeaderString.str().c_str(), HeaderString.str().size());
	Writter.close();

    return 0;
}
