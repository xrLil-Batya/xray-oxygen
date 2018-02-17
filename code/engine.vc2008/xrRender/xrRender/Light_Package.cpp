#include "stdafx.h"
#include "Light_Package.h"

void light_Package::clear()
{
	v_point.clear		();
	v_spot.clear		();
	v_shadowed.clear	();
}

void light_Package::sort()
{
	auto pred_light_cmp = [](light* first, light* second)->bool
	{
		bool isPending = (second->vis.pending);

		if (first->vis.pending)
		{
			return  isPending ? (first->vis.query_order > second->vis.query_order) : false;
		}
		else 
		{
			return (!isPending) ? (first->range > second->range) : true;
		}
	};

	// resort lights (pending -> at the end), maintain stable order
	std::stable_sort	(v_point.begin(),	v_point.end(),		pred_light_cmp);
	std::stable_sort	(v_spot.begin(),	v_spot.end(),		pred_light_cmp);
	std::stable_sort	(v_shadowed.begin(),v_shadowed.end(),	pred_light_cmp);
}