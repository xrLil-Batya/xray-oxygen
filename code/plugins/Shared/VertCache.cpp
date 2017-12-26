#include "stdafx.h"
class VertexCache
{

public:
	VertexCache(int size);
	VertexCache();
	~VertexCache();

private:
	xr_vector<int>	entries;
};

VertexCache::VertexCache() : VertexCache(16)
{
}


VertexCache::VertexCache(int size)
{
	entries.assign(size, -1);
}

VertexCache::~VertexCache()
{
	entries.clear();
}

