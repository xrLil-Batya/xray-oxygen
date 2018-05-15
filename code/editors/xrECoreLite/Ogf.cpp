#include "Ogf.h"
#include "OgfV3.h"
#include "OgfV4.h"

COgf::COgf()
{
}

COgf::~COgf()
{
}

void COgf::Load(IReader* writer)
{
	// TODO Loading ogf 
}

#include <xrCore\FS_internal.h>
#pragma todo("FX to all: Maybe using CFileReader everyone?")
COgf* COgf::Load(const char* path)
{
	IReader* reader = nullptr;
	if (FS.exist(path))
	{
		reader = FS.r_open(path);
	}
	else
	{
		DWORD FileAttrib  = GetFileAttributes(path);
		if (FileAttrib != DWORD(-1))
		{
			reader = new CFileReader(path);
		}
		else
		{
			Msg("%s not exist", path);
			return nullptr;
		}
	}
	if (!reader)
	{
		FS.r_close(reader);
		Debug.fatal(DEBUG_INFO, "File not found!");
	}

	COgf* ogf;

	if (reader->find_chunk(OGF_HEADER))
	{
		unsigned version = reader->r_u8();
		switch (version)
		{
		case OGF2_VERSION: // TODO: in xray tools OGF2_VERSION implements as OGF3_VERSION
			ogf = new COgf();
			break;
		case OGF3_VERSION:
			ogf = new COgfV3();
			break;
		case OGF4_VERSION:
			ogf = new COgfV4();
			break;
		default:
			// ERROR incompatible OGF version maybe need ASSERT or Exception
			return nullptr;
		}

		if (ogf)
		{
			ogf->m_path = path;
			ogf->Load(reader);
			FS.r_close(reader);
			return ogf;
		}
	}
	FS.r_close(reader);
	return nullptr;
}

bool COgf::Save(const char* path)
{
	return false;
}

bool COgf::Save(const char* path, const char* name)
{
	return false;
}