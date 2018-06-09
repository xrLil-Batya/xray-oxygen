#include "stdafx.h"
#include "xml_str_id_loader.h"
#include "object_broker.h"

void _destroy_item_data_vector_cont(T_VECTOR* vec)
{
	T_VECTOR::iterator it		= vec->begin();
	T_VECTOR::iterator it_e		= vec->end();

	xr_vector<CUIXml*>			_tmp;	
	for(;it!=it_e;++it)
	{
		xr_vector<CUIXml*>::iterator it_f = std::find(_tmp.begin(), _tmp.end(), (*it)._xml);
		if(it_f==_tmp.end())
			_tmp.push_back	((*it)._xml);
	}
	delete_data	(_tmp);
}
