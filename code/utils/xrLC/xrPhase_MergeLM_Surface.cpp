#include "stdafx.h"
#include "build.h"
#include "xrPhase_MergeLM_Rect.h"
#include "../xrlc_light/xrdeflector.h"

#include <emmintrin.h>

static	BYTE	surface[c_LMAP_size*c_LMAP_size];
const	u32		alpha_ref = 254 - BORDER;

// Initialization
void _InitSurface()
{
	FillMemory(surface, c_LMAP_size*c_LMAP_size, 0);
}

// Rendering of rect
void _rect_register(L_rect &R, lm_layer* D, BOOL bRotate)
{
	u8*		lm = &*(D->marker.begin());
	u32		s_x = D->width + 2 * BORDER;
	u32		s_y = D->height + 2 * BORDER;

	if (!bRotate) {
		// Normal (and fastest way)
		for (u32 y = 0; y<s_y; y++)
		{
			BYTE*	P = surface + (y + R.a.y)*c_LMAP_size + R.a.x;	// destination scan-line
			u8*		S = lm + y*s_x;
			for (u32 x = 0; x<s_x; x++, P++, S++)
				if (*S >= alpha_ref)			*P = 255;
		}
	}
	else {
		// Rotated :(
		for (u32 y = 0; y<s_x; y++)
		{
			BYTE*	P = surface + (y + R.a.y)*c_LMAP_size + R.a.x;	// destination scan-line
			for (u32 x = 0; x<s_y; x++, P++)
				if (lm[x*s_x + y] >= alpha_ref)	*P = 255;
		}
	}
}

// Test of per-pixel intersection (surface test)
bool Place_Perpixel(L_rect& R, lm_layer* D, BOOL bRotate)
{
	u8*	lm = &*(D->marker.begin());
	int	source_x = D->width + 2 * BORDER;
	int	source_y = D->height + 2 * BORDER;
	int x = 0;

	if (!bRotate) 
	{
		// Normal (and fastest way)
		for (int y = 0; y < source_y; y++)
		{
			BYTE* P = surface + (y + R.a.y) * c_LMAP_size + R.a.x;	// destination scan-line
			u8* S = lm + y * source_x;

			// remainder part
			for (; x < source_x; x++, P++, S++)
				if ((*P) && (*S >= alpha_ref)) {
					return false;
				}
		}
	}
	else
	{
		// Rotated :(
		for (int y = 0; y < source_x; y++) {
			BYTE* P = surface + (y + R.a.y) * c_LMAP_size + R.a.x;	// destination scan-line
			for (x = 0; x < source_y; x++, P++)
				if ((*P) && (lm[x * source_x + y] >= alpha_ref)) {
					return false;
				}
		}
	}

	return true;
}

// Check for intersection
BOOL _rect_place(L_rect &r, lm_layer* D)
{
	L_rect R;
	int _X = 0;
	BYTE* temp_surf;

	// Normal
	{
		int x_max = c_LMAP_size - r.b.x;
		int y_max = c_LMAP_size - r.b.y;
		for (int _Y = 0; _Y < y_max; _Y++) 
		{
			temp_surf = surface + _Y * c_LMAP_size;
			
			// remainder part
			for (; _X < x_max; _X++) 
			{
				if (temp_surf[_X]) continue;
				R.init(_X, _Y, _X + r.b.x, _Y + r.b.y);
				if (Place_Perpixel(R, D, FALSE)) {
					_rect_register(R, D, FALSE);
					r.set(R);
					return TRUE;
				}
			}
		}
	}

	// Rotated
	{
		int x_max = c_LMAP_size - r.b.y;
		int y_max = c_LMAP_size - r.b.x;
		for (int _Y = 0; _Y < y_max; _Y++) 
		{
			temp_surf = surface + _Y * c_LMAP_size;
			_X = 0;
			
			// remainder part
			for (; _X < x_max; _X++) {
				if (temp_surf[_X]) continue;
				R.init(_X, _Y, _X + r.b.y, _Y + r.b.x);
				if (Place_Perpixel(R, D, TRUE)) {
					_rect_register(R, D, TRUE);
					r.set(R);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
