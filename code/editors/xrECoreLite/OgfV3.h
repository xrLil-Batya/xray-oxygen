#pragma once
#include "Ogf.h"

struct Ogf3Header {
	uint8_t		version;
	uint8_t		type;
	uint16_t	unused;		// really?
};

class COgfV3 : public COgf
{

public:

	OgfVersion Version() const override { return OGF3_VERSION; }
};

