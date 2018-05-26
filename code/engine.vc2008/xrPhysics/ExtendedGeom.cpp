#include	"stdafx.h"
#include	"extendedgeom.h"
#include	"dcylinder/dcylinder.h"

XRPHYSICS_API bool	IsCyliderContact(const dContact& c)
{
	int geomClass = -1;
	if (dGeomGetBody(c.geom.g1))
	{
		geomClass = dGeomGetClass(retrieveGeom(c.geom.g1));
	}
	else
	{
		geomClass = dGeomGetClass(retrieveGeom(c.geom.g2));
	}

	return (geomClass == dCylinderClassUser);
}

dxGeomUserData* PHRetrieveGeomUserData(dGeomID geom)
{
	return retrieveGeomUserData(geom);
}

void	get_user_data(dxGeomUserData* &gd1, dxGeomUserData* &gd2, bool bo1, const dContactGeom &geom)
{
	if (bo1)
	{
		gd1 = retrieveGeomUserData(geom.g1);
		gd2 = retrieveGeomUserData(geom.g2);
	}
	else
	{
		gd2 = retrieveGeomUserData(geom.g1);
		gd1 = retrieveGeomUserData(geom.g2);
	}
}