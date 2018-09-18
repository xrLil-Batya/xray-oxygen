/*
  This source code is a part of IKAN.
  Copyright (c) 2000 University of Pennsylvania
  Center for Human Modeling and Simulation
  All Rights Reserved.

  IN NO EVENT SHALL THE UNIVERSITY OF PENNSYLVANIA BE LIABLE TO ANY
  PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
  DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
  SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF PENNSYLVANIA
  HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes,
  without fee, and without a written agreement is hereby granted,
  provided that the above copyright notice and the following three
  paragraphs appear in all copies. For for-profit purposes, please
  contact University of Pennsylvania
 (http://hms.upenn.edu/software/ik/ik.html) for the software license
  agreement.


  THE UNIVERSITY OF PENNSYLVANIA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
  BASIS, AND THE UNIVERSITY OF PENNSYLVANIA HAS NO OBLIGATION
  TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
  MODIFICATIONS.

 */

#include <device.h>
#include "jack-utils.h"
#include <stdarg.h>
#include "jcToolkit.h"
#include "jTypes.h"


void error(const char *format, ...)
{
    va_list args;

    fprintf(stderr, "*** FATAL ERROR ***: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(0);
}

void warning(const char *format, ...)

{
    va_list args;

    fprintf(stderr, "*** WARNING ***: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}


//
// Computes the rigid body transformation from base to final 
//

void GetRelativeTransform(Matrix &base, 
			  Matrix &final, 
			  Matrix &T)
{
    Matrix I; 

    inverthomomatrix(I, (float (*)[4]) base);
    hmatmult(T, (float (*)[4]) final, I);
}


void GetRelativeTransform(jcSite *base, 
			  jcSite *final, 
			  Matrix &T)
{
    jMatrix base_t, final_t; 

    if (base)
    {
      //GetSiteGlobal((jcSite *)base, &base_t);
      //GetSiteGlobal((jcSite *)final, &final_t);
      //GetRelativeTransform(base_t.matrix, final_t.matrix, T);

      base->getGlobal(&base_t);
      final->getGlobal(&final_t);
      GetRelativeTransform(base_t.matrix, final_t.matrix, T);
    }
    else
    {
      //GetSiteGlobal((jcSite *)final, &final_t);
      final->getGlobal(&final_t);

	cpmatrix(T, final_t.matrix); 
	
    }
}

void GetRelativeTransform(char *fig_name, 
			  char *base,
			  char *final,
			  Matrix &T)
{
    jcSite *base_s = GetSite(fig_name, base);
    jcSite *final_s = GetSite(fig_name, final);

    GetRelativeTransform(base_s, final_s, T);
}


//
// Used by GetMeterData
//
/*
static void adjust_meter(JackWindow *,
                 Meter *meter,
                 float *vals,
                 void (*action)(float *, void**),
                 void **args)
{
   
    short       x,y;
    int         buttons;

    StatusMsg("adjust scale...");
   
    buttons = (int)BUTTONS();
    x = (short)getvaluator(MOUSEX);
    while (!qtest() && buttons == BUTTONS())
    {
        y = (short)getvaluator(MOUSEY);
        GetMeterValue(meter,x,y);
        (*action)(vals, args);
        AdvanceSimulation();
        DrawWindows();
    }
}
*/

/*
 * Initialize a meter window.
 *      num_meters - number of meters
 *      names - names of meters
 *      lower - lower bounds for meter values
 *      upper - upper bounds for meter values
 *      vals  - meter values (current & returned)
 *      f     - callback function taking vals and args
 *      args  - additional arguments for f
 */

/*void GetMeterData(int num_meters,
                  char *names[],
                  float lower[],
                  float upper[],
                  float vals[],
                  void (*f)(float*, void**),
                  void *args[])
{
    JackWindow *window = CreateMeterWindow(num_meters);
    List        m;
    Meter       *meter;
    int         done;
    short       dev,val;
    short       x,y;

    m = 0;
    for (int i=0; i< num_meters; i++)
    {
        m = circlistiterator(METERWIN(window)->meters,m);
        meter = LISTDATA(Meter,m);
        meter->name = strdup(names[i]);
        meter->min = lower[i];
        meter->max = upper[i];
        meter->v = vals + i;
    }
    winset(window->wid);

    done = FALSE;
    while (!done)
    {

        DrawWindows();

        StatusMsg("Pick potentiometer...");

        // dev = jWaitForEvent(&val,W_PEABODY);
        dev = jWaitForEvent(&val,-1);
        if (dev == KEYBD)
        {
            if (val == CONTROL('C'))
            {
                done = -1;
                break;
            }
            else if (val == ESCAPE)
            {
                done = 1;
                break;
            }
        }
        if (!BUTTONS())
        {
            continue;
        }
        x = (short)getvaluator(MOUSEX);
        y = (short)getvaluator(MOUSEY);
        m = 0;
        meter = 0;
        while ((m=circlistiterator(METERWIN(window)->meters,m)) != NULL) {
            meter = LISTDATA(Meter,m);
            if (GetMeterValue(meter,x,y)) {
                break;
            }
        }

     if (meter == NULL) {
            StatusError("that's not a potentiometer!");
            FlushButtons();
            continue;
        }
        adjust_meter(window,meter,vals,f,args);
        FlushButtons();
    }
    DeleteWindow(window);
}
*/


void PrintMatrix(const Matrix &M)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            printf(" %lf ", M[i][j]);
        printf("\n");
    }
}

void PrintErrMatrix(const Matrix &M1, const Matrix &M2)
{
    Matrix M3;

    for (int i = 0; i < 4; i++)
	for (int j = 0; j < 4; j++)
	    M3[i][j] = M1[i][j] - M2[i][j]; 
    PrintMatrix(M3);
}


//
// Generate a random transformation matrix
// 
/*
void GetRandomTransform(Matrix &m, float min, float max)
{
    xyztomatrix(m, myrandom(0,2*M_PI),
		   myrandom(0,2*M_PI),
		   myrandom(0,2*M_PI));

    float v[3];

    // Make it more likely to move in a positive direction
    v[0] = myrandom(-1.0,2.0); 
    v[1] = myrandom(-1.0,2.0); 
    v[2] = myrandom(-1.0,2.0);
    unitize(v);

    float radius = myrandom(min, max);

    m[3][0] = radius * v[0];
    m[3][1] = radius * v[1];
    m[3][2] = radius * v[2];
}



int CMDRelativeTransform(VALUE *args, char **outargs)
{
    jcSite *base_site, *final_site;

    if (!(base_site = InputSite(&args, outargs, "Select base site")))
        return 0;
    if (!(final_site = InputSite(&args, outargs, "Select final site")))
        return 0;

    Transform T;

    GetRelativeTransform(base_site, final_site, T.matrix);
    PrintMatrix(T.matrix);

    return 1;
}
*/
float SiteDistance(jcSite *s1, jcSite *s2)
{
    jMatrix T;
    float t[3];

    GetRelativeTransform(s1, s2, T.matrix);
    GetTranslation(t, T.matrix); 
    return sqrt(DOT(t,t));
}

typedef float (*MatPtr)[4];

void MatMultiply(Matrix A, ...)
{
    va_list ap; 
    MatPtr m; 

    va_start(ap, A);
    if (m = va_arg(ap, MatPtr))
    {
	cpmatrix(A, m);
	while (m = va_arg(ap, MatPtr))
	    hmatmult(A, A, m);
    }
    else
	cpmatrix(A, idmat);

    va_end(ap);
}
