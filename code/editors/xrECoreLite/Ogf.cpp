#include "Ogf.h"
#include "OgfV4.h"
#include <xrCore\FS_internal.h>

COgf::COgf()
{
}

COgf::~COgf()
{
}

void COgf::load(IReader* reader)
{
	// TODO: Loading ogf 
	load_header(reader);
}

void COgf::load_header(IReader* reader)
{

}

void COgf::load_texture(IReader* reader)
{

}

#pragma todo("FX to all: Maybe using CFileReader everyone?")
COgf* COgf::load(const char* path)
{
	DWORD fileAttribute = GetFileAttributes(path);
	if (fileAttribute != DWORD(-1))
	{
		IReader* reader = new CFileReader(path);
		if (reader && reader->find_chunk(OGF_HEADER)) {
			COgf* ogf = nullptr;

			u8 version = reader->r_u8();
			switch (version)
			{
			case OGF3_VERSION:
				ogf = new COgf();
				break;
			case OGF4_VERSION:
				ogf = new COgfV4();
				break;
			default:
				Msg("Unsupportable OGF version %d", version);
				break;
			}

			if (ogf)
			{
				ogf->m_path = path;
				ogf->load(reader);
				FS.r_close(reader);
				return ogf;
			}
		}
	}
	return nullptr;
}