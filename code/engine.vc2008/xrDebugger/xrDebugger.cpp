// xrDebuger.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "xrLazyVector.h"

int main()
{
	Debug._initialize();
	Core._initialize("X-Ray Oxygen Test Runner", nullptr, TRUE, "fsgame.ltx");
	xrLogger::EnableFastDebugLog();

	Msg("Hallo!");
	xrLazyVector<int> SomeShit;
	SomeShit.reserve(15);

	SomeShit.push_back(1);
	SomeShit.push_back(2);
	SomeShit.push_back(8);
	SomeShit.push_back(4);
	SomeShit.push_back(5);
	SomeShit.push_back(6);
	SomeShit.push_back(7);
	SomeShit.push_back(2);

	SomeShit.remove(5);

	xr_vector<int> OtherShit;
	OtherShit.push_back(1);
	OtherShit.push_back(2);
	OtherShit.push_back(8);
	OtherShit.push_back(4);
	OtherShit.push_back(6);
	OtherShit.push_back(7);
	OtherShit.push_back(2);


	std::sort(OtherShit.begin(), OtherShit.end());
	std::sort(SomeShit.begin(), SomeShit.end());


	xrLazyVector<int> InsertTest;
	InsertTest.insert(InsertTest.end(), SomeShit.begin(), SomeShit.end());

	for (auto reverseIter = SomeShit.rbegin(); reverseIter != SomeShit.rend(); reverseIter++)
	{
		Msg("* %d", *reverseIter);
	}

	return 0;
}