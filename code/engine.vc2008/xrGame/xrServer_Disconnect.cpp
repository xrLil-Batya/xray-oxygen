#include "stdafx.h"
#include "xrServer.h"
#include "file_transfer.h"
#include "screenshot_server.h"

void xrServer::Disconnect()
{
	if (m_file_transfers)
	{
		xr_delete(m_file_transfers);
	}

	IPureServer::Disconnect	();
	SLS_Clear				();
	xr_delete				(game);
}
