#include "stdafx.h"
#pragma hdrstop

using namespace std;

XRCORE_API	smem_container*	g_pSharedMemoryContainer = nullptr;
XRCORE_API	bool	g_pSharedMemoryContainer_isDestroyed = false;

smem_value* smem_container::dock(u32 dwCRC, u32 dwLength, void* ptr)
{
	VERIFY(dwCRC && dwLength && ptr);

	xrCriticalSectionGuard guard(cs);
	smem_value* result = 0;

	// search a place to insert
	u8 storage[4 * sizeof(u32)];
	smem_value* value = (smem_value*)storage;
	value->dwReference = 0;
	value->dwCRC = dwCRC;
	value->dwLength = dwLength;
	cdb::iterator it = std::lower_bound(container.begin(), container.end(), value, smem_search);
	cdb::iterator saved_place = it;
	if (container.end() != it) {
		// supposedly found
		for (;; ++it)
		{
			if (it == container.end())
				break;

			if ((*it)->dwCRC != dwCRC)
				break;

			if ((*it)->dwLength != dwLength)
				break;

			if (0 == memcmp((*it)->value, ptr, dwLength))
			{
				// really found
				result = *it;
				break;
			}
		}
	}

	// if not found - create new entry
	if (!result)
	{
		result = (smem_value*)Memory.mem_alloc(4 * sizeof(u32) + dwLength);
		result->dwReference = 0;
		result->dwCRC = dwCRC;
		result->dwLength = dwLength;
		std::memcpy(result->value, ptr, dwLength);
		container.insert(saved_place, result);
	}

	return result;
}

void smem_container::clean()
{
	xrCriticalSectionGuard guard(cs);

	for (auto &it : container)
		if (!it->dwReference)
			xr_free(it);

	container.erase(remove(container.begin(), container.end(), (smem_value*)0), container.end());
	if (container.empty())	container.clear();
}

void smem_container::dump()
{
	xrCriticalSectionGuard guard(cs);
	FILE* F = fopen("$smem_dump$.txt", "w");

	for (smem_value* it : container)
		fprintf(F, "%4u : crc[%6x], %d bytes\n", it->dwReference, it->dwCRC, it->dwLength);

	fclose(F);
}

u32 smem_container::stat_economy()
{
	xrCriticalSectionGuard guard(cs);
	s64 counter = 0;
	counter -= sizeof(*this);
	counter -= sizeof(cdb::allocator_type);
	const int		node_size = 20;
	for (auto &it : container)
	{
		counter -= 16;
		counter -= node_size;
		counter += (it->dwReference - 1) * it->dwLength;
	}

	return u32(s64(counter) / s64(1024));
}

smem_container::~smem_container()
{
	clean();
}