// xrXRC.h: interface for the xrXRC class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "xrCDB.h"

#ifdef	DEBUG
extern	XRCDB_API	CStatTimer	*cdb_clRAY;				// total: ray-testing
extern	XRCDB_API	CStatTimer	*cdb_clBOX;				// total: box query
extern	XRCDB_API	CStatTimer	*cdb_clFRUSTUM;			// total: frustum query
#endif

class XRCDB_API xrXRC  
{
	CDB::COLLIDER	CL;
public:
	IC void			ray_options		(u32 f)		
	{ 
		CL.ray_options(f); 
	}
	IC void			ray_query		(const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f)
	{
#ifdef DEBUG
		cdb_clRAY->Begin();
#endif
		CL.ray_query(m_def,r_start,r_dir,r_range);
#ifdef DEBUG
		cdb_clRAY->End	();
#endif
	}
	
	IC void			box_options		(u32 f)	
	{	
		CL.box_options(f);
	}
	IC void			box_query		(const CDB::MODEL *m_def, const Fvector& b_center, const Fvector& b_dim)
	{
#ifdef DEBUG
		cdb_clBOX->Begin();
#endif
		CL.box_query(m_def,b_center,b_dim);
#ifdef DEBUG
		cdb_clBOX->End	();
#endif
	}
	
	IC void			frustum_options	(u32 f)
	{
		CL.frustum_options(f);
	}
	IC void			frustum_query	(const CDB::MODEL *m_def, const CFrustum& F)
	{
#ifdef DEBUG
		cdb_clFRUSTUM->Begin();
#endif
		CL.frustum_query(m_def,F);
#ifdef DEBUG
		cdb_clFRUSTUM->End	();
#endif
	}
	
	IC xr_vector<CDB::RESULT>::iterator r_realBegin()	{ return CL.r_realBegin();    };
	IC xr_vector<CDB::RESULT>::iterator r_realEnd()		{ return CL.r_realEnd();	  };
	IC CDB::RESULT& r_getElement(size_t id)				{ return CL.r_getElement(id); };

	IC void			r_free			()	{	CL.r_free();				}
	IC size_t		r_count			()	{	return CL.r_count();		};
	IC bool			r_empty			()	{	return CL.r_empty();		};
	IC void			r_clear			()	{	CL.r_clear();				};
	IC void			r_clear_compact	()	{	CL.r_clear_compact();		};
	
	xrXRC()		= default;
	~xrXRC()	= default;
};
XRCDB_API extern xrXRC XRC;