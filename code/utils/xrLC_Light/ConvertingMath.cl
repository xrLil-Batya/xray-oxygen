/////////////////////////////////////
// Author: ForserX
// Desc  : CUDA Converting to OpenCL analogy

// Round-down : __float2int_rd [CUDA]
inline int convert_int_rtz (float number) 
{
   return ((int)(number));
}