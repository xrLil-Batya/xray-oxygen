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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <assert.h>
#include "grid.h"



static void xyz_to_spherical(float x, float y, float z,
		      float &r, float &psi, float &theta)
{
    r = sqrt(x*x+y*y+z*z);
    theta = atan2(z,x);
    if (theta < 0)
	theta += 2*M_PI;
    psi = acos(y/r);
}

static void spherical_to_xyz(float r, float psi, float theta,
		      float &x, float &y, float &z)
{
    float temp = r*sin(psi);

    y = r*cos(psi);
    x = temp*cos(theta);
    z = temp*sin(theta);
}


//
// Given psi,theta return the indices of the closest grid point
// also compute the distance between the point and the grid point
//
float ClavicleGrid::index(float psi, 
			 float theta,
			 int &psi_i, 
			 int &theta_i)
{
    float dist_psi, dist_theta;

    psi_i = (psi + d_psi / 2.0) / d_psi;
    if (psi_i >= psi_n)
    {
	psi_i = psi_n - 1;
	dist_psi = M_PI - psi;
    }
    else
	dist_psi = fabs(psi - psi_i * d_psi);

    theta_i = (theta+d_theta/2.0)/d_theta;
    if (theta_i >=  theta_n)
    {
	theta_i = 0;
	dist_theta = 2*M_PI - theta;
    }
    else
	dist_theta = fabs(theta - theta_i * d_theta);

    return dist_psi + dist_theta;
}



void ClavicleGrid::initialize()
{
    memset(init,0,sizeof(init));
    memset(vals, 0,sizeof(vals));
    d_psi   = M_PI / (psi_n - 1);
    d_theta = 2*M_PI / (theta_n - 1);
}


void ClavicleGrid::Add(const float p[3], float v[2])
{
    float r, psi, theta, d;
    int theta_i, psi_i; 
    xyz_to_spherical(p[0],p[1],p[2],r,psi,theta);
    d = index(psi, theta, psi_i, theta_i);

    if (!init[psi_i][theta_i] || (d < dist[psi_i][theta_i]))
    {
	dist[psi_i][theta_i] = d;
	init[psi_i][theta_i] = 1;
	vals[psi_i][theta_i][0] = v[0];
	vals[psi_i][theta_i][1] = v[1];
    }
}

#if 0
void ClavicleGrid::AddDebug(int psi_i, int theta_i, float v[2])
{
    float d;
    d = 0;

    if (!init[psi_i][theta_i] || (d < dist[psi_i][theta_i]))
    {
	dist[psi_i][theta_i] = d;
	init[psi_i][theta_i] = 1;
	vals[psi_i][theta_i][0] = v[0];
	vals[psi_i][theta_i][1] = v[1];
    }
}
#endif

void ClavicleGrid::Debug()
{
    for (int i = 0; i < psi_n; i++)
	for (int j = 0; j < theta_n; j++)
	    printf("[%d,%d] %f,%f\n", i,j,vals[i][j][0],vals[i][j][1]);
}


int modulo_increment(int i, int N)
{
    return (i + 1) % N;
}

int modulo_decrement(int i, int N)
{
    return (i - 1) < 0 ? ((N + (i-1)) % N) : i-1;
}

int modulo_distance(int i, int j, int N)
{
    int m = abs(i-j);
    return (m > N/2) ? (N - m) : m;
}

int ClavicleGrid::visit(int i, int j, float v[2])
{
    if (init[i][j])
    {
	v[0] += vals[i][j][0];
	v[1] += vals[i][j][1];
	return 1;
    }

    return 0;
}



void ClavicleGrid::Fill()
{
    //
    // First interpolate in the psi direction
    // 
    for (int i = 0; i < psi_n; i++)
    {
	int left = 0;
	int l = 0;
	int state = 0;

	for (int j = 0; j < theta_n; j++)
	{
	    switch(state)
	    {
	    case 0:
		if (init[i][j])
		{
		    left = j;
		    for (int k = l; k < j; k++) 
		    {
			vals[i][k][0] = vals[i][left][0];
			vals[i][k][1] = vals[i][left][1];
			init[i][k] = 1;
		    }
		    state = 1;
		}
		break;

	    case 1:
		if (init[i][j])
		    left = j;
		else
		{
		    l = j;
		    state = 2;
		}
		break;
	    case 2:
		if (init[i][j])
		{
		    assert(left == (l-1));
		    for (int k = l; k < j; k++)
		    {
			float a = (k - left);
			float b = (j - k);
			float c = a + b;
			a /= c;
			b /= c;

			vals[i][k][0] = 
			    b*vals[i][left][0] + 
			    a*vals[i][j][0]; 

			vals[i][k][1] = 
			    b*vals[i][left][1] + 
			    a*vals[i][j][1]; 

			init[i][k] = 1;
		    }
		    state = 1;
		}
	    }
	}

	if (state == 2)
	    for (int k = l; k < theta_n; k++)
	    {
		vals[i][k][0] = vals[i][left][0];
		vals[i][k][1] = vals[i][left][1];
	    }
    }


    // Now interpolate in theta direction
    for (int j = 0; j < theta_n; j++)
	for (int i = 0; i < psi_n; i++)
	    if (!init[i][j])
	    {
		// Find left most initialized entry
		int left  = modulo_decrement(i, psi_n);
		int right = modulo_increment(i, psi_n);
		
		while (left != i)
		    if (init[left][j])
			break;
		    else
			left = modulo_decrement(left, psi_n);

		while (right != i)
		    if (init[right][j])
			break;
		    else
			right = modulo_increment(right, psi_n);

		if (right != i && left != i)
		{
		    float a = modulo_distance(i, left, psi_n);
		    float b = modulo_distance(i, right, psi_n); 
		    float c = a + b;
		    a /= c;
		    b /= c;

		    vals[i][j][0] =
			b*vals[left][j][0] + 
			a*vals[right][j][0]; 

		    vals[i][j][1] = 
			b*vals[left][j][1] + 
			a*vals[right][j][1]; 

		    init[i][j] = 1;
		}
	    }


    // Initialize all unintialized theta isolines
    for (j = 0; j < theta_n; j++)
    {
	if (!init[0][j])
	{
	    int left  = modulo_decrement(j, theta_n);
	    int right = modulo_increment(j, theta_n);
	    int use = -1;

	    while (use == -1)
	    {
		if (init[0][left])
		    use = left;
		else
		    left = modulo_decrement(left, theta_n);

		if (init[0][right])
		    use = right;
		else
		    right = modulo_increment(right, theta_n); 


		assert(left != i);
		assert(right != i);
	    }

	    for (int k = 0; k < psi_n; k++)
	    {
		vals[k][j][0] = vals[k][use][0];
		vals[k][j][1] = vals[k][use][1];
		init[k][j] = 1;
	    }
	}
    }
    

    memset(init, 1, sizeof(init));
}

//
// Find the four points on the grid closest to v and interpolate
// these four points to find a suitable set of vals
//
// 
void ClavicleGrid::Interpolate(const float v[3], float val[2])
{
    float r, psi, theta;
    xyz_to_spherical(v[0], v[1], v[2], r, psi, theta);

    int psi_1, psi_2, theta_1, theta_2;
    float u1, u2, v1, v2, w;

    psi_1 = psi / d_psi;
    psi_2 = psi_1 + 1;

    if (psi_2 > psi_n - 1)
    {
	psi_2 = psi_n - 1;
	u1 = 0;
    }
    else
	u1 = (psi - d_psi * psi_1) / d_psi;

    if (u1 < 0 || u1 > 1.0)
	printf("bug 0\n");

    u2 = 1.0 - u1;

    theta_1 = theta/d_theta;
    theta_2 = theta_1 + 1;

    // handle wrap around for theta
    if (theta_2 > theta_n - 1)
	theta_2 = 0;


    v1 = (theta - d_theta * theta_1) / d_theta;
    v2 = 1.0 - v1;

    if (v1 < 0)
	printf("bug1 %lf\n", v1);

    if (v1 > 1.0)
	printf("bug2 %lf\n", v2);


#if 0
    printf("interpolating (%d,%d)=(%lf,%lf) (%d,%d)=(%lf,%lf)\n",
	   psi_1, theta_1, 
	   vals[psi_1][theta_1][0],
	   vals[psi_1][theta_1][1],
	   psi_2, theta_2,
	   vals[psi_2][theta_2][0],
	   vals[psi_2][theta_2][1],
	   v1, u1, v2, u2);
#endif

    // Add contribution of each point

    w = u2*v2;
    val[0] = w * vals[psi_1][theta_1][0];
    val[1] = w * vals[psi_1][theta_1][1];

    w = u1*v2;
    val[0] += w * vals[psi_2][theta_1][0];
    val[1] += w * vals[psi_2][theta_1][1];
		
    w = u2*v1;
    val[0] += w * vals[psi_1][theta_2][0];
    val[1] += w * vals[psi_1][theta_2][1];

    w = u1*v1;
    val[0] += w * vals[psi_2][theta_2][0];
    val[1] += w * vals[psi_2][theta_2][1];

}


void ClavicleGrid::Write(const char *fname)
{
    FILE *fp = fopen(fname, "w");

    if (!fp)
	fprintf(stderr, "Cannot write %s\n", fname);
    else
    {
	for (int i = 0; i < psi_n; i++)
	    for (int j = 0; j < theta_n; j++)
		fprintf(fp, "%f %f\n", vals[i][j][0], vals[i][j][1]);  
	fclose(fp);
    }
}

void ClavicleGrid::Read(const char *fname)
{
    FILE *fp = fopen(fname, "r");

    if (!fp)
	fprintf(stderr, "Cannot read %s\n", fname);
    else
    {
	for (int i = 0; i < psi_n; i++)
	    for (int j = 0; j < theta_n; j++)
		if (fscanf(fp, "%f %f\n", &vals[i][j][0], &vals[i][j][1]) != 2)
		{
		    fprintf(stderr, "Cannot read data item %d %d from fname %s\n",i, j, fname);
		    fclose(fp);
		    return;
		}
	memset(dist, 0, sizeof(dist));
	memset(init, 1, sizeof(init));
	fclose(fp);
    }
}

void ClavicleGrid::Entry(int i, int j, float v[2])
{
    if (i < 0 || i >=  psi_n || j < 0 || j >= theta_n)
    {
	fprintf(stderr, "bug: bad index %d,%d in entry\n", i, j);
    }

    v[0] = vals[i][j][0];
    v[1] = vals[i][j][1];
}

/*
int main()
{
    ClavicleGrid G;
    float v[2]; 

#if 0
    v[0] = 0;
    v[1] = 0;
    for (int i = 0; i < 4; i++)
	for (int j = 0; j < 5; j++)
	{
	    G.AddDebug(i,j,v);
	    v[0] += 1;
	    v[1] += 1;
	}

    float dpsi = M_PI/3;
    float dtheta = M_PI/2;
    float psi   = 2.5*dpsi;
    float theta = 3.5*dtheta;

    G.InterpolateDebug(psi, theta, v);
    printf("[%f,%f]\n", v[0], v[1]);


    G.Write("grid.dat");

    ClavicleGrid G2("grid.dat");
    G2.Write("grid2.dat");
#else
    float dpsi = M_PI/3;
    float dtheta = M_PI/2;

    v[0] = 1;
    v[1] = 1;

    float p[3]; 
    float eps = .1;

    spherical_to_xyz(10.0,dpsi + eps, dtheta + eps, p[0],p[1],p[2]);
    G.Add(p, v);
    spherical_to_xyz(10.0,dpsi , dtheta , p[0],p[1],p[2]);
    G.Add(p, v);
    spherical_to_xyz(10.0,dpsi + eps, dtheta + eps, p[0],p[1],p[2]);
    G.Add(p, v);

#endif
}


*/
