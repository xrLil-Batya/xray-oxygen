#pragma once
#include <ctime>
#include <string>

class XRCORE_API Time
{
private:
	time_t t;
	tm* aTm;
	using string = std::string;

public:
	Time();

	// Convert time to string
	string GetSeconds();
	string GetMinutes();
	string GetHours();

	// Convert date to string
	string GetDay();
	string GetMonth();
	string GetYear();

	int GetSecond();
	int GetMin();
	int GetHour();
};