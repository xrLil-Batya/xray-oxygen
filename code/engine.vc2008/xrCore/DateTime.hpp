#pragma once
#include <ctime>
#include <string>

class Time
{
private:
	time_t t;
	tm* aTm;

public:
	Time();

	// Convert time to string
	std::string GetSeconds();
	std::string GetMinutes();
	std::string GetHours();

	// Convert date to string
	std::string GetDay();
	std::string GetMonth();
	std::string GetYear();
};
