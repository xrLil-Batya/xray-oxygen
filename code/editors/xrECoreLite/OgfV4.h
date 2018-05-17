#pragma once
#include "Ogf.h"

struct Ogf4Header {
	uint8_t		format_version;
	uint8_t		type;
	uint16_t	shader_id;
	//ogf_bbox	bb;
	//ogf_bsphere	bs;
};

class COgfV4 : public COgf
{

public:

	OgfVersion version() const override { return OGF4_VERSION; }
};

