#include "stdafx.h"

// computing build id
extern LPCSTR	build_date;
extern u32		build_id;

static constexpr char* month_id[12] = {
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

static constexpr u32 days_in_month[12] = 
{
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static constexpr u32 start_day = 26;
static constexpr u32 start_month = 12;
static constexpr u32 start_year = 2016;

// binary hash, mainly for copy-protection
void compute_build_id()
{
	build_date = __DATE__;

	u32 Days;
	u32 MonthId = 0;
	u32 Years;
	string16 Month = { 0 };

	sscanf(__DATE__, "%s %d %d", Month, &Days, &Years);

	for (u32 i = 0; i < 12; i++)
	{
		if (_stricmp(month_id[i], Month))
			continue;

		MonthId = i;
		break;
	}

	build_id = (Years - start_year) * 365 + Days - start_day;

	for (u32 i = 0; i < MonthId; ++i)
		build_id += days_in_month[i];

	for (u32 i = 0; i < start_month - 1; ++i)
		build_id -= days_in_month[i];
}