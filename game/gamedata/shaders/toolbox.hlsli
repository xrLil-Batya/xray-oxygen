//Shader Toolbox Version 0.1b
//Xray-Oxygen 1.7
//Author: MatthewKush
//Credits: GodComplex, WickedEngine, MaxestFramework, Nvidia, AMD, Intel, MJP, and many others
//TODO: Organize this properly.

#define half float
#define vec float
#define vec2 float2
#define vec3 float3
#define mix lerp
#define mat2 float2x2
#define mat3 float3x3
#define mat4 float4x4 
#define fract frac

#define TEXTURE_SAMPLING


#ifdef 0

#define PI_SHORT 3.14159265359
#define PI_LONG 3.1415926535897932384626433832795
#define TWO_PI_SHORT 6.28318530718
#define GOLDEN_ANGLE 2.4
#define MILLIMETERS_PER_METER 1000
#define METERS_PER_MILLIMETER rcp(MILLIMETERS_PER_METER)
#define CENTIMETERS_PER_METER 100
#define METERS_PER_CENTIMETER rcp(CENTIMETERS_PER_METER)
#define FLT_INF  asfloat(0x7F800000)
#define FLT_EPS  5.960464478e-8  // 2^-24, machine epsilon: 1 + EPS = 1 (half of the ULP for 1.0f)
#define FLT_MIN  1.175494351e-38 // Minimum normalized positive floating-point number
#define FLT_MAX  3.402823466e+38 // Maximum representable floating-point number
#define HALF_MIN 6.103515625e-5  // 2^-14, the same value for 10, 11 and 16-bit: https://www.khronos.org/opengl/wiki/Small_Float_Formats
#define HALF_MAX 65504.0
#define UINT_MAX 0xFFFFFFFFu

#endif


#ifdef TEXTURE_SAMPLING

#ifdef SM_4_0 | SM_4_1 | SM_5_0

float2 2DTextureSample(Texture2D myTex, Sampler smp_example, float2 uv)
{
    return myTex.Sample(smp_example, uv);
}

float4 getTexel(float2 uv, float2 myTexResolution)
{
    p = p*myTexResolution + 0.5;

    vec2 i = floor(p);
    vec2 f = p - i;
    f = f*f*f*(f*(f*6.0-15.0)+10.0);
    p = i + f;

    p = (p - 0.5)/myTexResolution;
    return texture2D( myTex, p );
}
#endif //SM_4_0 | SM_4_1 | SM_5_0

#endif


#ifdef FILTERING
//Box filtered checkerboard
float checkers( in vec2 p, in vec2 dpdx, in vec2 dpdy )
{
    vec2 w = max(abs(dpdx), abs(dpdy));
    vec2 i = 2.0*(abs(fract((p-0.5*w)*0.5)-0.5)-
                  abs(fract((p+0.5*w)*0.5)-0.5))/w;
    return 0.5 - 0.5*i.x*i.y;                  
}

//Box filtered grid
float grid( in vec2 p, in vec2 dpdx, in vec2 dpdy )
{
    const float N = 10.0; // grid ratio
    vec2 w = max(abs(dpdx), abs(dpdy));
    vec2 a = p + 0.5*w;                        
    vec2 b = p - 0.5*w;           
    vec2 i = (floor(a)+min(fract(a)*N,1.0)-
              floor(b)-min(fract(b)*N,1.0))/(N*w);
    return (1.0-i.x)*(1.0-i.y);
}

// improved bilinear interpolated texture fetch
vec4 textureGood( sampler2D sam, vec2 uv )
{
    vec2 res = textureSize( sam );

    vec2 st = uv*res - 0.5;

    vec2 iuv = floor( st );
    vec2 fuv = fract( st );

    vec4 a = texture( sam, (iuv+vec2(0.5,0.5))/res );
    vec4 b = texture( sam, (iuv+vec2(1.5,0.5))/res );
    vec4 c = texture( sam, (iuv+vec2(0.5,1.5))/res );
    vec4 d = texture( sam, (iuv+vec2(1.5,1.5))/res );

    return mix( mix( a, b, fuv.x),
                mix( c, d, fuv.x), fuv.y );
}
#endif

#ifdef REMAPPING
/*******************************************************************
    Spherical map sampling
*******************************************************************/
float2 dirToSphericalCrd(float3 direction)
{
    float3 p = normalize(direction);
    float2 uv;
    uv.x = (1 + atan2(-p.z, p.x) / M_PI) * 0.5;
    uv.y = 1 - (-acos(p.y) / M_PI);
    return uv;
}

float3 sphericalCrdToDir(float2 uv)
{
    float phi = M_PI * uv.y;
    float theta = M_PI2 * uv.x - (M_PI / 2.0f);

    float3 dir;
    dir.x = sin(phi) * sin(theta);
    dir.y = cos(phi);
    dir.z = sin(phi) * cos(theta);

    return normalize(dir);
}

mat2 scale(vec2 _scale){
    return mat2(_scale.x,0.0,
                0.0,_scale.y);
}

#endif

#ifdef COLOR_UTILS

float calc_luminance(float3 rgb)
{
	//return dot(rgb, float3(0.212671, 0.715160, 0.072169) );		// Defined by sRGB gamut
    return dot(rgb, float3(0.299f, 0.587f, 0.114f));
}
// Returns relative luminance. To not be confused with "luma" which
// is defined by same coefficients multiplied by gamma compressed
// sRGB values.
float GetRelativeLuminance(float3 rgb)
{
    // ITU-R BT.709 Coefficients
    // http://www.itu.int/rec/R-REC-BT.709
    return dot(rgb, float3(0.2126, 0.7152, 0.0722));
}

float3 srgb_to_rgb(float3 srgb)
{
    float3 rgb = (srgb <= 0.04045) * srgb / 12.92;
    rgb += (srgb > 0.04045) * pow((abs(srgb) + 0.055) / 1.055, 2.4);
    return rgb;
}

float4 srgba_to_rgba(float4 srgb)
{
    float4 rgb = (srgb <= 0.04045) * srgb / 12.92;
    rgb += (srgb > 0.04045) * pow((abs(srgb) + 0.055) / 1.055, 2.4);
    return rgb;
}

float3 rgb_to_srgb(float3 rgb)
{
    return pow(rgb, 1 / 2.2f);
}

float linearizeColor(float component)
{
    float lC = (component <= 0.04045) * component / 12.92;
    lC += (component > 0.04045) * pow((abs(component) + 0.055) / 1.055, 2.4);
    return lC;
}

float3 RGBToHSL(float3 rgb)
{
    float3 hcv = RGBToHCV(rgb);
    float L = hcv.z - hcv.y * 0.5;
    float S = hcv.y / (1 - abs(L * 2 - 1) + COLOR_EPSILON);
    return float3(hcv.x, S, L);
}

float3 HSLToRGB(float3 hsl)
{
    float3 rgb = HUEToRGB(hsl.x);
    float C = (1 - abs(2 * hsl.z - 1)) * hsl.y;
    return (rgb - 0.5) * C + hsl.z;
}

float MiddleGrey(float avgLuminance) 
{
    // "Fast Filtering and Tone Mapping using Importance sampling",
    // Balázs Tóth and László Szirmay-Kalos
	return 1.03f - 2/(2 + log10(avgLuminance + 1));
}

float CalculateExposure(float avgLuminance, float exposure)
{
    float avg_lum = max(avgLuminance, 0.0001f);
    float linear_exposure = MiddleGrey(avgLuminance) / avg_lum;
    linear_exposure = log2(max(linear_exposure, 0.0001f));
    linear_exposure += exposure;
	return linear_exposure;
}

// Reference: https://delightlylinux.wordpress.com/2014/02/18/sweetfx-vibrance/
float3 VibranceFilter(float3 rgb, float vibrance)
{
    float luminance = GetRelativeLuminance(rgb);

    float minc = min(min(rgb.r, rgb.g), rgb.b);
    float maxc = max(max(rgb.r, rgb.g), rgb.b);

    float saturation = maxc - minc;

    float s = 1.0 + (vibrance * (1.0 - (sign(vibrance) * saturation)));
    return lerp(luminance, rgb, s);
}


//  Function from Iñigo Quiles
//  https://www.shadertoy.com/view/MsS3Wc
vec3 hsb2rgb( in vec3 c ){
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
                             6.0)-3.0)-1.0,
                     0.0,
                     1.0 );
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix( vec3(1.0), rgb, c.y);
}

// Reference: http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
// See ColorExtensions.TemperatureToRGB() for Kelvins to RGB conversion algorithm
float3 TemperatureFilter(float3 color, float3 TemperatureColor, float TemperatureStrength)
{
    // Restore original luminance
    float3 originalHSL = RGBToHSL(color);
    float3 filteredHSL = RGBToHSL(TemperatureColor);
    filteredHSL.xy = lerp(originalHSL.xy, filteredHSL.xy, TemperatureStrength);
    return HSLToRGB(float3(filteredHSL.x, filteredHSL.y, originalHSL.z));
}

#endif

#ifdef SHAPES

float box(in vec2 _st, in vec2 _size){
    _size = vec2(0.5) - _size*0.5;
    vec2 uv = smoothstep(_size,
                        _size+vec2(0.001),
                        _st);
    uv *= smoothstep(_size,
                    _size+vec2(0.001),
                    vec2(1.0)-_st);
    return uv.x*uv.y;
}

float cross(in vec2 _st, float _size){
    return  box(_st, vec2(_size,_size/4.)) +
            box(_st, vec2(_size/4.,_size));
}

#endif

#ifdef THE_MATRIX

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

#endif

#ifdef BRDF_FUNCTIONS

float getMetallic(float diffuse, float spec)
{
    // This is based on the way that UE4 and Substance Painter 2 converts base+metallness+specular level to diffuse/spec colors
    // We don't have the specular level information, so the assumption is that it is equal to 0.5 (based on the UE4 documentation)
    float a = 0.04;
    float b = spec + diffuse - 0.08;
    float c = 0.04 - spec;
    float root = sqrt(b*b - 0.16*c);
    float m = (root - b) * 12.5;
    return m;
}

float3 ComputeF0(float3 baseColor, float metallic = 0, float dielectricF0 = 0.04)
{
    return lerp(dielectricF0.xxx, baseColor, metallic);
}

float radicalInverse(uint i)
{
    i = (i & 0x55555555) << 1 | (i & 0xAAAAAAAA) >> 1;
    i = (i & 0x33333333) << 2 | (i & 0xCCCCCCCC) >> 2;
    i = (i & 0x0F0F0F0F) << 4 | (i & 0xF0F0F0F0) >> 4;
    i = (i & 0x00FF00FF) << 8 | (i & 0xFF00FF00) >> 8;
    i = (i << 16) | (i >> 16);
    return float(i) * 2.3283064365386963e-10f;
}

float2 getHammersley(uint i, uint N)
{
    return float2(float(i) / float(N), radicalInverse(i));
}

void ConvertRoughnessToAnisotropy(float roughnessT, float roughnessB, out float anisotropy)
{
    anisotropy = ((roughnessT - roughnessB) / max(roughnessT + roughnessB, 0.0001));
}

#endif

#ifdef STEREO_PROJECTION

float4 stereo(float4 v) { // stereographic projection: 4d sphere -> 3d space
		v.xyz /= 1 - v.w;
		v.w = 1;
		return v;
	}
float4 stereo1(float4 v) { // stereographic projection inverse function: 3d space -> 4d sphere
		v.xyz /= v.w;
		float s = dot(v.xyz, v.xyz);
		v.xyz *= 2;
		v.w = s - 1;
		v /= s + 1;
		return v;
	}
#endif

#ifdef TONEMAP_PASS

float3 AcesToneMap(float3 color)
{
    float A = 2.51f;
    float B = 0.03f;
    float C = 2.43f;
    float D = 0.59f;
    float E = 0.14f;

    color = saturate((color*(A*color+B))/(color*(C*color+D)+E));
    return color;
}

#endif

// Helper functions:
float slopeFromT (float t, float A, float B, float C){
  float dtdx = 1.0/(3.0*A*t*t + 2.0*B*t + C); 
  return dtdx;
}
float xFromT (float t, float A, float B, float C, float D){
  float x = A*(t*t*t) + B*(t*t) + C*t + D;
  return x;
}
float yFromT (float t, float E, float F, float G, float H){
  float y = E*(t*t*t) + F*(t*t) + G*t + H;
  return y;
}
float B0 (float t){
  return (1.0-t)*(1.0-t)*(1.0-t);
}
float B1 (float t){
  return  3.0*t*(1.0-t)*(1.0-t);
}
float B2 (float t){
  return 3.0*t*t* (1.0-t);
}
float B3 (float t){
  return t*t*t;
}
float  findx (float t, float x0, float x1, float x2, float x3){
  return x0*B0(t) + x1*B1(t) + x2*B2(t) + x3*B3(t);
}
float  findy (float t, float y0, float y1, float y2, float y3){
  return y0*B0(t) + y1*B1(t) + y2*B2(t) + y3*B3(t);
}

float cubicBezier(float x, vec2 a, vec2 b){
  float y0a = 0.0; // initial y
  float x0a = 0.0; // initial x 
  float y1a = a.y;    // 1st influence y   
  float x1a = a.x;    // 1st influence x 
  float y2a = b.y;    // 2nd influence y
  float x2a = b.x;    // 2nd influence x
  float y3a = 1.0; // final y 
  float x3a = 1.0; // final x 

  float A =   x3a - 3.0*x2a + 3.0*x1a - x0a;
  float B = 3.0*x2a - 6.0*x1a + 3.0*x0a;
  float C = 3.0*x1a - 3.0*x0a;   
  float D =   x0a;

  float E =   y3a - 3.0*y2a + 3.0*y1a - y0a;    
  float F = 3.0*y2a - 6.0*y1a + 3.0*y0a;             
  float G = 3.0*y1a - 3.0*y0a;             
  float H =   y0a;

  // Solve for t given x (using Newton-Raphelson), then solve for y given t.
  // Assume for the first guess that t = x.
  float currentt = x;
  for (int i=0; i < 5; i++){
    float currentx = xFromT (currentt, A,B,C,D); 
    float currentslope = slopeFromT (currentt, A,B,C);
    currentt -= (currentx - x)*(currentslope);
  	currentt = clamp(currentt,0.0,1.0); 
  } 

  float y = yFromT (currentt,  E,F,G,H);
  return y;
}

float cubicBezierNearlyThroughTwoPoints(float x, vec2 a, vec2 b){

  float y = 0.0;
  float epsilon = 0.00001;
  float min_param_a = 0.0 + epsilon;
  float max_param_a = 1.0 - epsilon;
  float min_param_b = 0.0 + epsilon;
  float max_param_b = 1.0 - epsilon;
  a.x = max(min_param_a, min(max_param_a, a.x));
  a.y = max(min_param_b, min(max_param_b, a.y));

  float x0 = 0.0;  
  float y0 = 0.0;
  float x4 = a.x;  
  float y4 = a.y;
  float x5 = b.x;  
  float y5 = b.y;
  float x3 = 1.0;  
  float y3 = 1.0;
  float x1,y1,x2,y2; // to be solved.

  // arbitrary but reasonable 
  // t-values for interior control points
  float t1 = 0.3;
  float t2 = 0.7;

  float B0t1 = B0(t1);
  float B1t1 = B1(t1);
  float B2t1 = B2(t1);
  float B3t1 = B3(t1);
  float B0t2 = B0(t2);
  float B1t2 = B1(t2);
  float B2t2 = B2(t2);
  float B3t2 = B3(t2);

  float ccx = x4 - x0*B0t1 - x3*B3t1;
  float ccy = y4 - y0*B0t1 - y3*B3t1;
  float ffx = x5 - x0*B0t2 - x3*B3t2;
  float ffy = y5 - y0*B0t2 - y3*B3t2;

  x2 = (ccx - (ffx*B1t1)/B1t2) / (B2t1 - (B1t1*B2t2)/B1t2);
  y2 = (ccy - (ffy*B1t1)/B1t2) / (B2t1 - (B1t1*B2t2)/B1t2);
  x1 = (ccx - x2*B2t1) / B1t1;
  y1 = (ccy - y2*B2t1) / B1t1;

  x1 = max(0.0+epsilon, min(1.0-epsilon, x1));
  x2 = max(0.0+epsilon, min(1.0-epsilon, x2));

  y = cubicBezier (x, vec2(x1,y1), vec2(x2,y2));
  y = max(0.0, min(1.0, y));
  return y;
}

vec2 wave(vec2 st, float freq) {
	st.y += cos(st.x*freq);
	return st;
}

vec2 zigzag(vec2 st, float freq) {
	st.y += mix(abs(floor(sin(st.x*3.1415))),abs(floor(sin((st.x+1.)*3.1415))),fract(st.x*freq));
	return st;
}

float line(vec2 st, float width) {
    return step(width,1.0-smoothstep(.0,1.,abs(sin(st.y*PI))));
}

float plot(vec2 _st, float _pct){
  return  smoothstep( _pct-0.01, _pct, _st.y) -
          smoothstep( _pct, _pct+0.01, _st.y);
}


float circle(vec2 st, float radius){
  vec2 pos = vec2(0.5)-st;
  radius *= 0.75;
  return 1.-smoothstep(radius-(radius*0.01),radius+(radius*0.01),dot(pos,pos)*3.14);
}

float plot(vec2 st, float pct){
  return  smoothstep( pct-0.01, pct, st.y) -
          smoothstep( pct, pct+0.01, st.y);
}

float linearstep(float begin, float end, float t) {
    return clamp((t - begin) / (end - begin), 0.0, 1.0);
}

float easeInCubic(float t) {
    return t * t * t;
}

float easeOutQuad(float t) {
    return -1.0 * t * (t - 2.0);
}

float smoothedge(float v, float f) {
    return smoothstep(0.0, f / u_resolution.x, v);
}

float rect(vec2 p, vec2 size) {
  vec2 d = abs(p) - size;
  return min(max(d.x, d.y), 0.0) + length(max(d,0.0));
}

float rectPlot(vec2 p, vec2 size) {
  return 1.0 - smoothedge(rect(p, size), 1.0);
}

float collider(vec2 p, vec2 b, vec2 e, float t) {

    float t0 = linearstep(0.0, 0.5, t);
    float p0 = easeInCubic(t0);
    float t1 = linearstep(0.5, 1.0, t);
    float p1 = easeOutQuad(t1);

    return rectPlot(p - mix(b, e, p0 - p1), vec2(0.05));
}

float colliders(vec2 st, float t) {
    float t0 = fract(t);
    float t1 = fract(t + 0.5);
    float v = collider(st, vec2(0.05, 0.5), vec2(0.45, 0.5), t0);
    v = max(v, collider(st, vec2(0.95, 0.5), vec2(0.55, 0.5), t0));
    v = max(v, collider(st, vec2(0.5, 0.05), vec2(0.5, 0.45), t1));
    v = max(v, collider(st, vec2(0.5, 0.95), vec2(0.5, 0.55), t1));
    return v;
}

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float snoise(vec2 v) {
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,  // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
        + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

#define OCTAVES 6
float turbulence(in vec2 st) {
    float value = 0.0;
    float amplitude = 1.0;
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * abs(snoise(st));
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

float cellular(vec2 p) {
    vec2 i_st = floor(p);
    vec2 f_st = fract(p);
    float m_dist = 10.;
    for (int j=-1; j<=1; j++ ) {
        for (int i=-1; i<=1; i++ ) {
            vec2 neighbor = vec2(float(i),float(j));
            vec2 point = random2(i_st + neighbor);
            point = 0.5 + 0.5*sin(6.2831*point);
            vec2 diff = neighbor + point - f_st;
            float dist = length(diff);
            if( dist < m_dist ) {
                m_dist = dist;
            }
        }
    }
    return m_dist;
}

#ifdef AA_DLAA

// Directionally Localized antiAliasing
// Original ver: facepuncherfromrussia (2011)
// Update   ver: ForserX (2018) 

// New: New Luminance code

#ifndef PIXEL_SIZE
#define PIXEL_SIZE float2(1.0/1280, 1.0/720)
#endif

#ifdef DX9Ver
#define LD(o, dx, dy) o = tex2D(s_image, tc + float2(dx, dy) * PIXEL_SIZE.xy);
#else
#define LD(o, dx, dy) o = s_image.Sample(smp_rtlinear, tc + float2(dx, dy) * PIXEL_SIZE.xy);
#endif

static const bool bPreserveHf = 0;

float4 PreProcessPS(float2 tc)
{
    float4 center, left, right, top, bottom;
 
    LD(center, 0,  0)
    LD(left,  -1,  0)
    LD(right,  1,  0)
    LD(top,    0, -1)
    LD(bottom, 0,  1)
 
    float4 edges = 4.0f * abs((left + right + top + bottom) - 4.0f * center);
    float  edges_lum = Luminance(edges.xyz);
 
    return float4(center.xyz, edges_lum);
}


float4 DLAAPixelShader(float2 tc)
{
    const float lambda = 3.0f;
    const float epsilon = 0.1f;

    float4 center, left_01, right_01, top_01, bottom_01;
 
    LD(center,      0,   0)
    LD(left_01,  -1.5,   0)
    LD(right_01,  1.5,   0)
    LD(top_01,      0,-1.5)
    LD(bottom_01,   0, 1.5)
 
	float4 w_h = 2.0f * (left_01 + right_01);
	float4 w_v = 2.0f * (top_01 + bottom_01);
	
#ifdef TFU2_HIGH_PASS
		float4 edge_h = abs(w_h - 4.0f * center) / 4.0f;
		float4 edge_v = abs(w_v - 4.0f * center) / 4.0f;
#else
		float4 left, right, top, bottom;
	 
		LD(left,  -1,  0)
		LD(right,  1,  0)
		LD(top,    0, -1)
		LD(bottom, 0,  1)
	 
		float4 edge_h = abs(left + right - 2.0f * center) / 2.0f;
		float4 edge_v = abs(top + bottom - 2.0f * center) / 2.0f;
#endif
	 
	float4 blurred_h = (w_h + 2.0f * center) / 6.0f;
	float4 blurred_v = (w_v + 2.0f * center) / 6.0f;
	 
	float edge_h_lum = Luminance(edge_h.xyz);
	float edge_v_lum = Luminance(edge_v.xyz);
	
	float blurred_h_lum = Luminance(blurred_h.xyz);
	float blurred_v_lum = Luminance(blurred_v.xyz);
	 
	float edge_mask_h = saturate((lambda * edge_h_lum - epsilon) / blurred_v_lum);
	float edge_mask_v = saturate((lambda * edge_v_lum - epsilon) / blurred_h_lum);
	 
	float4 clr = center;
	clr = lerp(clr, blurred_h, edge_mask_v);
	clr = lerp(clr, blurred_v, edge_mask_h * 0.5f); // TFU2 uses 1.0f instead of 0.5f
	 
	float4 h0, h1, h2, h3, h4, h5, h6, h7;
	float4 v0, v1, v2, v3, v4, v5, v6, v7;

	LD(h0, 1.5, 0) LD(h1, 3.5, 0) LD(h2, 5.5, 0) LD(h3, 7.5, 0) LD(h4, -1.5,0) LD(h5, -3.5,0) LD(h6, -5.5,0) LD(h7, -7.5,0)
	LD(v0, 0, 1.5) LD(v1, 0, 3.5) LD(v2, 0, 5.5) LD(v3, 0, 7.5) LD(v4, 0,-1.5) LD(v5, 0,-3.5) LD(v6, 0,-5.5) LD(v7, 0,-7.5)
	 
	float long_edge_mask_h = (h0.a + h1.a + h2.a + h3.a + h4.a + h5.a + h6.a + h7.a) / 8.0f;
	float long_edge_mask_v = (v0.a + v1.a + v2.a + v3.a + v4.a + v5.a + v6.a + v7.a) / 8.0f;
	 
	long_edge_mask_h = saturate(long_edge_mask_h * 2.0f - 1.0f);
	long_edge_mask_v = saturate(long_edge_mask_v * 2.0f - 1.0f);

	if (abs(long_edge_mask_h - long_edge_mask_v) > 0.2f)	// resistant to noise (TFU2 SPUs)
	{
		float4 left, right, top, bottom;
	 
		LD(left,  -1,  0)
		LD(right,  1,  0)
		LD(top,    0, -1)
		LD(bottom, 0,  1)
	 
		float4 long_blurred_h = (h0 + h1 + h2 + h3 + h4 + h5 + h6 + h7) / 8.0f;
		float4 long_blurred_v = (v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7) / 8.0f;
	 
		float lb_h_lum   = Luminance(long_blurred_h.xyz);
		float lb_v_lum   = Luminance(long_blurred_v.xyz);
	 
		float center_lum = Luminance(center.xyz);
		float left_lum   = Luminance(left.xyz);
		float right_lum  = Luminance(right.xyz);
		float top_lum    = Luminance(top.xyz);
		float bottom_lum = Luminance(bottom.xyz);
	 
		float4 clr_v = center;
		float4 clr_h = center;
	 
		float hx = saturate((lb_h_lum - top_lum   ) / (center_lum - top_lum   ));
		float vx = saturate((lb_v_lum - left_lum  ) / (center_lum - left_lum  ));
		
		float hy = saturate(1 + (lb_h_lum - center_lum) / (center_lum - bottom_lum));
		float vy = saturate(1 + (lb_v_lum - center_lum) / (center_lum - right_lum ));
	 
		// Vectorized searching
		float4 vhxy = float4(vx, vy, hx, hy);
		vhxy = vhxy == float4(0, 0, 0, 0) ? float4(1, 1, 1, 1) : vhxy;
	 
		clr_v = lerp(left  , clr_v, vhxy.x);
		clr_v = lerp(right , clr_v, vhxy.y);
		clr_h = lerp(top   , clr_h, vhxy.z);
		clr_h = lerp(bottom, clr_h, vhxy.w);

		clr = lerp(clr, clr_v, long_edge_mask_v);
		clr = lerp(clr, clr_h, long_edge_mask_h);
	}
	
	if (bPreserveHf)
	{
		float4 r0, r1;
		float4 r2, r3;
	 
		LD(r0, -1.5, -1.5)
		LD(r1,  1.5, -1.5)
		LD(r2, -1.5,  1.5)
		LD(r3,  1.5,  1.5)
	 
		float4 r = (4.0f * (r0 + r1 + r2 + r3) + center + top_01 + bottom_01 + left_01 + right_01) / 25.0f;
		clr = lerp(clr, center, saturate(r.a * 3.0f - 1.5f));
	}
	return clr;
}

#endif




