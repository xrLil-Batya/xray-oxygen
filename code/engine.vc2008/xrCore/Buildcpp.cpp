#include "stdafx.h"

// computing build id
extern LPCSTR	build_date;
extern u32		build_id;

static LPSTR month_id[12] = {
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

static u32 days_in_month[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static u32 start_day = 31;	// 31
static u32 start_month = 1;	// January
static u32 start_year = 1999;	// 1999

// binary hash, mainly for copy-protection
void compute_build_id()
{
	build_date = __DATE__;

	u32 days;
	u32 months = 0;
	u32 years;
	string16 month;
	string256 buffer;
	xr_strcpy(buffer, __DATE__);

	sscanf(buffer, "%s %d %d", month, &days, &years);

	for (u32 i = 0; i<12; i++)
	{
		if (_stricmp(month_id[i], month))
			continue;

		months = i;
		break;
	}

	build_id = (years - start_year) * 365 + days - start_day;

	for (u32 i = 0; i<months; ++i)
		build_id += days_in_month[i];

	for (u32 i = 0; i<start_month - 1; ++i)
		build_id -= days_in_month[i];
}