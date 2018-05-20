#include "stdafx.h"
#include "level.h"
#include "xrserver.h"

void	CLevel::SLS_Default				()					// Default/Editor Load
{
	if (Server)
        Server->SLS_Default();
}
