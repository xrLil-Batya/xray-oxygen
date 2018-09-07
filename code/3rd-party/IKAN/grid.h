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

#ifndef _GRIDH
#define _GRIDH

#ifdef _WINDOWS
#define M_PI            3.14159265358979323846
#endif

//
// Stores a table of clavicle angles as functions of the
// spherical coordinates of the elbow relative to the
// clavicle joint
// 

// 
// The table is stored in a table with psi ranging from 0 .. 180 degrees 
// and theta ranging from 0 .. 360 degrees. The table is discretized 
// into intervals given by values psi_n and theta_n 

//
// The user adds values to the table with the Add procedure. The
// elbow pos is mapped to the closest point on the grid. If the
// grid point is uninitialized then the entry is added. Otherwise
// if the value is closer to the grid point than the previous
// entry it is added.
// 

//
// Not all the grid points will be initialized because of holes
// in the workspace. Calling Fill() will fill the uninitialized
// grid points by interpolation using the four points: the two
// closest in each dimension. 
//

class ClavicleGrid
{
private:
    enum { psi_n = 20, theta_n = 40 };

    float vals[psi_n][theta_n][2];
    float dist[psi_n][theta_n];
    char  init[psi_n][theta_n];

    float index(float psi, float theta, int &psi_i, int &theta_i);
    //    void index_to_pt(float x, float y, float p[3]); 

    float d_psi, d_theta;

    int visit(int i, int j, float v[2]);
    int fill_radius(int i, int j, int r);
    void initialize(); 

public:
    ClavicleGrid() { initialize(); }
    ClavicleGrid(const char *fname) { initialize(); Read(fname); }
    
    void Write(const char *fname);
    void Read(const char *fname);

    // Adds an entry to the table
    //	p is the position of the elbow which is converted to spherical coords 
    //  theta is the value of the clavicle joints

    void Add(const float p[3], float v[2]);

    //
    // Fills all the empty entries in the table
    //
    void Fill();

    // Interpolates a set of theta angles given the elbow pos 
    void Interpolate(const float v[3], float vals[2]);

    //
    // Temporary debugging routines
    //
    void Debug();
    void Entry(int i, int j, float v[2]);
    int NumPsi()   { return psi_n; }
    int NumTheta() { return theta_n; } 


#if 0
    void AddDebug(int psi, int theta, float v[2]);
    void InterpolateDebug(float psi, float theta, float v[2]);
#endif

};



#endif
