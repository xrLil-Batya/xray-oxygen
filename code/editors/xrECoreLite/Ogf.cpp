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

COgf* COgf::Load(const char* path)
{
	if (FS.exist(path))
	{
		IReader* reader = FS.r_open(path);
		R_ASSERT(reader);

		COgf* ogf;

		if (reader->find_chunk(OGF_HEADER)) {
			unsigned version = reader->r_u8();
			switch (version)
			{
			case OGF2_VERSION: // TODO in xray tools OGF2_VERSION implements as OGF3_VERSION
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
				return;
			}

			if (ogf) {
				ogf->m_path = path;
				ogf->Load(reader);
				FS.r_close(reader);
				return ogf;
			}
		}
		else
		{
			// ERROR
		}
	}
	else 
	{
		Msg("%s not exist", path);
	}
}

bool COgf::Save(const char* path)
{
	return false;
}

bool COgf::Save(const char* path, const char* name)
{
	return false;
}