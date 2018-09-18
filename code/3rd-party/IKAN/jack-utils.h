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

#ifndef _JACKUTILSH
#define _JACKUTILSH


#include <stdio.h>
#include <stdlib.h>

#include "jcToolkit.h"
#include "jTypes.h"
#include "jcFigure.h"
#include "jcJoint.h" 
#include "myvec.h"

//
// These routines don't require Jack
//

// random generator from low to high
inline void mysrandom(long seed) { srand48(seed); }

// random double from low to high
inline double myrandom(double low, double high)
{
    return (high - low) * drand48() + low;
}

inline void myisrand(long seed) { srand(seed); }

// random integer from low to high
inline int myirand(int low, int high)
{
    return low + (rand() % (high - low + 1));
}

// rounds a float to an int
inline int ftoi(float x)
{ 
    return x < 0 ? ((int)(x - .5)) : ((int) (x + .5)); 
}


// report error and warning messages
void error(const char *format, ...);
void warning(const char *format, ...);


/*
#include "jack.h" 
#include "human.h"
#include "jcmd.h" 
#include "constraint.h"
*/



//
// Finds a joint given the figure name and the name of the joint
//

/* for Jack Toolkit */
inline jcJoint *GetJoint(char *hname, char *name)
{
    jcFigure *figure = jcscene->findFigure(hname);
    jcJoint  *j = figure->findJoint(name);
    if (!j)
    {
	fprintf(stderr, "Cannot find joint %s\n", name);
	exit(0);
    }
    return j;
}
	
/* only for Jack
inline jcJoint *GetJoint(const char *hname, const char *name)
{
    char buf[100];
    if (hname)
	sprintf(buf, "%s.%s", hname, name);
    else
	strcpy(buf, name);
    char *ptr = buf;

    jcJoint *j = findJoint(&ptr);

    if (!j)
    {
	fprintf(stderr, "Cannot find joint %s\n", buf);
	exit(0);
    }

    return j;
}
*/

//
// Finds a site given the figure name and the name of the joint
//
/* for Jack Toolkit */
inline jcSite *GetSite(char *hname, char *name)
{
    jcFigure *figure = jcscene->findFigure(hname);
    
    char *sitename = new char[strlen(hname)+strlen(name)+4];
    sprintf(sitename,"%s.%s", hname,name);
    jcSite  *s = figure->findSite(sitename);
    if (!s)
    {
	fprintf(stderr, "Cannot find site %s\n", sitename);
	exit(0);
    }
    return s;
}

/* for JACK
inline jcSite *GetSite(const char *hname, const char *name)
{
    char buf[100];
    if (hname)
	sprintf(buf, "%s.%s", hname, name);
    else
	strcpy(buf, name);

    char *ptr = buf;

    jcSite *s = FindSiteByName(0, &ptr);
    if (!s)
    {
	fprintf(stderr, "Cannot find site %s\n", buf);
	exit(0);
    }

    return s;
}
*/

/* for JackTool Kit */
inline jcFigure *GetFigure(char *figname)
{
    jcFigure *f = jcscene->findFigure(figname);
    if (!f)
    {
	fprintf(stderr, "Cannot find figure %s\n", figname);
	exit(0);
    }
    return f;
}

/* for JACK
inline jcFigure *GetFigure(const char *figname)
{
    char buf[100];
    strcpy(buf, figname);
    char *ptr = buf;
    jcFigure *f = FindFigureByName(0, &ptr); 
    return f;
} 
*/

//
// Miscellaneous matrix and transformation routines
//

void PrintMatrix(Matrix &M);

// Prints the error between two matrices
void PrintErrMatrix(Matrix &M1, Matrix &M2);

// Generates a random a transformation matrix. min and max 
// are the minimum and maximum translation distance

void GetRandomTransform(Matrix &m, float min, float max);


// Get rotation and translation componets of a transformation matrix

// Only sets rotation of M to R. Does not change translation part of R
inline void GetRotation(Matrix &R, Matrix &M)
{
    for (int i = 0; i < 3; i++)
	for (int j = 0; j < 3; j++)
	    R[i][j] = M[i][j];
}

// Only sets translation of M to T. Does not change rotation part of T
inline void GetTranslation(Matrix &T, Matrix &M)
{
    T[3][0] = M[3][0];
    T[3][1] = M[3][1];
    T[3][2] = M[3][2];
}

inline void GetTransposeRotation(Matrix &M, Matrix &N)
{
    for (int i = 0; i < 3; i++)
	for (int j = 0; j < 3; j++)
	    M[j][i] = N[i][j];
}

inline void GetTranslation(float pos[], Matrix &M)
{
    pos[0] = M[3][0];
    pos[1] = M[3][1];
    pos[2] = M[3][2];
}

inline void GetTranslation(float pos[], jcSite *s)
{
    jVector vec;
    s->getGlobalPosition(vec);
    pos[0] = vec.v[0];
    pos[1] = vec.v[1];
    pos[2] = vec.v[2];
/* for JACK
    Transform T;
    GetSiteGlobal((jcSite *)s, &T);
    GetTranslation(pos, T.matrix);
*/
}


inline void SetTranslation(Matrix &M, const float pos[])
{
    M[3][0] = pos[0];
    M[3][1] = pos[1];
    M[3][2] = pos[2];
}

inline void SetZeroTranslation(Matrix &M)
{
    M[3][0] = M[3][1] = M[3][2] = 0.0;
}

// return distance between two sites
float SiteDistance(jcSite *s1, jcSite *s2);

//
// Compute the rigid body transformation from site base to site final 
//
void GetRelativeTransform(jcSite *base, 
			  jcSite *final, 
			  Matrix &T);

void GetRelativeTransform(char *figure_name,
			  char *base,
			  char *final,
			  Matrix &T);

void GetRelativeTransform(Matrix &base,
			  Matrix &final,
			  Matrix &T);

void GetRelativeTransform(jcSite *base, 
			  jcSite *final, 
			  Matrix &T);
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

void GetMeterData(int num_meters,
                  char *names[],
                  float lower[],
                  float upper[],
                  float vals[],
                  void (*f)(float*, void**),
                  void *args[]);



//
// Interactive routine to report the transformation between two sites
//

/* only for jack
int CMDRelativeTransform(VALUE *args, char **outargs);
*/
int CMDRelativeTransform(int argc, char *argv[]);


// A = B*C*... (Terminated by 0)
void MatMultiply(Matrix A, ...);


#endif
