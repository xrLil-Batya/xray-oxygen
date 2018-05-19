#pragma once
#include "files_list.hpp"

class ECORE_API COmf
{
protected:
			COmf();
	virtual ~COmf();

	//virtual void Load(IReader* reader) override;
	//virtual bool Save(IWriter* writer) override;	// TODO: is need?

public:

	//static COmf* Load(const char* path);
	//static bool	 Save(COmf* ogf, const char* path); // TODO: is need?
	//static bool  Save(COmf* ogf, const char* path, const char* name); // TODO: is need?
};

