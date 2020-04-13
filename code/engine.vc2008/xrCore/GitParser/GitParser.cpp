////////////////////////////////////////////////////
// Author: ForserX								  //
// Task  : Parsing current branch and commit hash //
////////////////////////////////////////////////////
// Specital for X-Ray Oxygen Project | 15.07.2018 //
////////////////////////////////////////////////////
// FX: Deque for spliting. | 13.04.2020			  //
////////////////////////////////////////////////////
#include <string>
#include <string_view>
#include <fstream>
#include <istream>
#include <sstream>
#include <deque> 
#include <utility>

#if __has_include("hack.appveyor")
#define ITS_CI_BUILD
// https://www.appveyor.com/docs/environment-variables/
#define NewStr(str) #str
#define xstr(ToStr) NewStr(ToStr)
#endif 

std::deque<std::string> dqSplit(std::string_view Str, char splitCh) noexcept
{
	std::deque<std::string> Result;

	size_t SubStrBeginCursor = 0;
	size_t Len = 0;
	for (size_t StrCursor = 0; StrCursor < Str.size(); ++StrCursor)
	{
		if (Str[StrCursor] == splitCh)
		{
			//Don't create empty string
			if ((StrCursor - 1 - SubStrBeginCursor) > 0)
			{
				Len = StrCursor - 1 - SubStrBeginCursor;
				Result.emplace_back(std::move(Str.substr(SubStrBeginCursor, Len)));
				SubStrBeginCursor = StrCursor + 1;
			}
		}
	}

	Result.emplace_back(std::move(Str.substr(SubStrBeginCursor, Str.length() - Len)));
	return Result;
}

int main()
{
#ifndef ITS_CI_BUILD
	std::string PathFile = "../../../.git/";

	// Get repo data 
	std::string BranchName = "";
	std::ifstream Reader(PathFile + "HEAD");
	Reader >> BranchName;
	BranchName = dqSplit(BranchName, '/').back();

	// Get current branch 
	PathFile += "refs/heads/" + BranchName;
	Reader.close();
	Reader.open(PathFile);

	// Get current hash commit code 
	std::string hash = "";
	Reader >> hash;
	Reader.close();
#endif

	// Make oxy_version.h
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
