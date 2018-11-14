/////////////////////////////////////////////////////////////////////////////////////////////////////
// BRDF Implementations
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define FGD_BRDFS_COUNT	6	// Only 6 BRDF types are supported at the moment

// Specular BRDFs
#define FGD_BRDF_INDEX_GGX				0
#define FGD_BRDF_INDEX_COOK_TORRANCE	1	// Not available => GGX is stored in that slot
#define FGD_BRDF_INDEX_WARD				2	// Not available => GGX is stored in that slot

// Diffuse BRDFs
#define FGD_BRDF_INDEX_OREN_NAYAR		3
#define FGD_BRDF_INDEX_CHARLIE			4	// Not available => Oren-Nayar is stored in that slot
#define FGD_BRDF_INDEX_DISNEY			5	// Not available => Oren-Nayar is stored in that slot

float	pow2( float x ) { return x * x; }
float2	pow2( float2 x ) { return x * x; }
float3	pow2( float3 x ) { return x * x; }
float4	pow2( float4 x ) { return x * x; }
float	pow3( float x ) { return x * x * x; }
float2	pow3( float2 x ) { return x * x * x; }
float3	pow3( float3 x ) { return x * x * x; }
float4	pow3( float4 x ) { return x * x * x; }



/////////////////////////////////////////////////////////////////////////////////////////////////////
// FRESNEL

// Assuming n1=1 (air) we get:
//	F0 = ((n2 - n1) / (n2 + n1))²
//	=> n2 = (1 + sqrt(F0)) / (1 - sqrt(F0))
//
float	Fresnel_IORFromF0( float _F0 ) {
	float	SqrtF0 = sqrt( _F0 );
	return (1.0 + SqrtF0) / (1.0001 - SqrtF0);
}
float3	Fresnel_IORFromF0( float3 _F0 ) {
	float3	SqrtF0 = sqrt( _F0 );
	return (1.0 + SqrtF0) / (1.0001 - SqrtF0);
}

// Assuming n1=1 (air) we get:
//	IOR = (1 + sqrt(F0)) / (1 - sqrt(F0))
//	=> F0 = ((n2 - 1) / (n2 + 1))²
//
float	Fresnel_F0FromIOR( float _IOR ) {
	float	ratio = (_IOR - 1.0) / (_IOR + 1.0);
	return ratio * ratio;
}
float3	Fresnel_F0FromIOR( float3 _IOR ) {
	float3	ratio = (_IOR - 1.0) / (_IOR + 1.0);
	return ratio * ratio;
}

// Schlick's approximation to Fresnel reflection (http://en.wikipedia.org/wiki/Schlick's_approximation)
float	FresnelDielectricSchlick( float _F0, float _cosTheta, float _fresnelStrength=1.0 ) {
	float	t = 1.0 - saturate( _cosTheta );
	float	t2 = t * t;
	float	t4 = t2 * t2;
	return lerp( _F0, 1.0, _fresnelStrength * t4 * t );
}

float3	FresnelDielectricSchlick( float3 _F0, float _cosTheta, float _fresnelStrength=1.0 ) {
	float	t = 1.0 - saturate( _cosTheta );
	float	t2 = t * t;
	float	t4 = t2 * t2;
	return lerp( _F0, 1.0, _fresnelStrength * t4 * t );
}

// Full accurate Fresnel computation (from Walter's paper §5.1 => http://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf)
// For dielectrics only but who cares!?
float	FresnelDielectric( float _IOR, float _cosTheta, float _fresnelStrength=1.0 ) {
	float	c = lerp( 1.0, _cosTheta, _fresnelStrength );
	float	g_squared = max( 0.0, _IOR*_IOR - 1.0 + c*c );

	float	g = sqrt( g_squared );

	float	a = (g - c) / (g + c);
			a *= a;
	float	b = (c * (g+c) - 1.0) / (c * (g-c) + 1.0);
			b = 1.0 + b*b;

	return 0.5 * a * b;
}

float3	FresnelDielectric( float3 _IOR, float _cosTheta, float _fresnelStrength=1.0 ) {
	float	c = lerp( 1.0, _cosTheta, _fresnelStrength );
	float3	g_squared = max( 0.0, _IOR*_IOR - 1.0 + c*c );

	float3	g = sqrt( g_squared );

	float3	a = (g - c) / (g + c);
			a *= a;
	float3	b = (c * (g+c) - 1.0) / (c * (g-c) + 1.0);
			b = 1.0 + b*b;

	return 0.5 * a * b;
}

// From Ole Gulbrandsen "Artist Friendly Metallic Fresnel" (http://jcgt.org/published/0003/04/03/paper.pdf)
//	_F0, metal color at normal incidence (i.e. theta=0°)
//	_F90, metal color at grazing incidence (i.e. theta=90°)
//	_cosTheta, cos(theta) of the light/view angle (should be the angle between the view/light vector and half vector in a standard micro-facet model)
//
float3	FresnelMetal( float3 _F0, float3 _F90, float _cosTheta ) {

	float3	r = clamp( _F0, 0, 0.99 );
	float3	g = _F90;
	float	c = _cosTheta;
	float	c2 = pow2(c);

	// Compute n and k
	float3	sqrtR = sqrt( r );
	float3	n_min = (1-r) / (1+r);
	float3	n_max = (1+sqrtR) / (1-sqrtR);
	float3	n = lerp( n_min, n_max, g );
	float3	n2 = pow2(n);

	float3	nr = pow2(n+1) * r - pow2(n-1);
	float3	k2 = nr / (1 - r);

	// Compute perpendicular polarized Fresnel
	float3	numPe = n2 + k2 - 2*n*c + c2;
	float3	denPe = n2 + k2 + 2*n*c + c2;
	float3	Pe = numPe / denPe;

	// Compute parallel polarized Fresnel
	float3	numPa = (n2 + k2)*c2 - 2*n*c + 1;
	float3	denPa = (n2 + k2)*c2 + 2*n*c + 1;
	float3	Pa = numPa / denPa;

	return 0.5 * (Pe + Pa);
}

// Returns the "average Fresnel" term for dielectrics, as given by Kulla & Conty, slide 18 (http://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_slides.pdf)
//	Favg = Integral[Omega+]{ F(µi) µi dwi }
//
// _IOR, from 1 to 400
float3	FresnelAverage( float3 _IOR ) {
	return (_IOR - 1.0) / (4.08567 + 1.00071 * _IOR);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// 
float	SampleIrradiance( float _cosTheta, float _alpha, uint _BRDFIndex ) {
	return s_accumulator.SampleLevel( smp_base, float3( _cosTheta, _alpha, _BRDFIndex ), 0.0 );
}

float	SampleAlbedo( float _alpha, uint _BRDFIndex ) {
	return s_diffuse.SampleLevel( smp_base, float2( _alpha, (0.5 + _BRDFIndex) / FGD_BRDFS_COUNT ), 0.0 );
}
// ====== GGX ======
float	GGX_NDF( float _NdotH, float _alpha2 ) {
	float	den = PI * pow2( pow2( _NdotH ) * (_alpha2 - 1) + 1 );
	return _alpha2 * rcp( den );
}
	// Height-correlated shadowing/masking: G2 = 1 / (1 + Lambda(light) + Lambda(view))
	// Warning: 1 / (4 * NdotL * NdotV) is already accounted for!
float	GGX_Smith( float _NdotL, float _NdotV, float _alpha2 ) {
		float	denL = _NdotV * sqrt( pow2( _NdotL ) * (1-_alpha2) + _alpha2 );
		float	denV = _NdotL * sqrt( pow2( _NdotV ) * (1-_alpha2) + _alpha2 );
		return rcp( 2.0 * (denL + denV) );
}

float3	BRDF_GGX( float3 _tsNormal, float3 _tsView, float3 _tsLight, float _alpha, float3 _F0 ) {
	float	NdotL = dot( _tsNormal, _tsLight );
	float	NdotV = dot( _tsNormal, _tsView );
	if ( NdotL < 0.0 || NdotV < 0.0 )
		return 0.0;

	float3	IOR = Fresnel_IORFromF0( _F0 );

	float	a2 = pow2( _alpha );
	float3	H = normalize( _tsView + _tsLight );
	float	NdotH = saturate( dot( H, _tsNormal ) );
	float	HdotL = saturate( dot( H, _tsLight ) );

	float	D = GGX_NDF( NdotH, a2 );
	float	G = GGX_Smith( NdotL, NdotV, a2 );
	float3	F = FresnelDielectric( IOR, HdotL );

	return max( 0.0, F * G * D );
}

// ====== Simple Oren-Nayar implementation ======
//  _normal, unit surface normal
//  _light, unit vector pointing toward the light
//  _view, unit vector pointing toward the view
//  _roughness, Oren-Nayar roughness parameter in [0,PI/2]
//
float3	BRDF_OrenNayar( in float3 _normal, in float3 _view, in float3 _light, float _roughness, float3 _rho ) {
	float3  n = _normal;
	float3  l = _light;
	float3  v = _view;

	float   LdotN = dot( l, n );
	float   VdotN = dot( v, n );

	float   gamma = dot( normalize( v - n * VdotN ), normalize( l - n * LdotN ) );
//	float   gamma = dot( v - n * VdotN, l - n * LdotN )
//				  / (sqrt( saturate( 1.0 - VdotN*VdotN ) ) * sqrt( saturate( 1.0 - LdotN*LdotN ) ));	// This yields NaN when LdotN is exactly 1. Can be fixed using sqrt( saturate( 1.000001 - LdotN*LdotN ) ) instead, or a max...

	float rough_sq = _roughness * _roughness;
	float A = 1.0 - 0.5 * (rough_sq / (rough_sq + 0.57));   // You can replace 0.33 by 0.57 to simulate the missing inter-reflection term, as specified in footnote of page 22 of the 1992 paper
	float B = 0.45 * (rough_sq / (rough_sq + 0.09));

	// Original formulation
	//  float angle_vn = acos( VdotN );
	//  float angle_ln = acos( LdotN );
	//  float alpha = max( angle_vn, angle_ln );
	//  float beta  = min( angle_vn, angle_ln );
	//  float C = sin(alpha) * tan(beta);

	// Optimized formulation (without tangents, arccos or sines)
	float2  cos_alpha_beta = VdotN < LdotN ? float2( VdotN, LdotN ) : float2( LdotN, VdotN );   // Here we reverse the min/max since cos() is a monotonically decreasing function
	float2  sin_alpha_beta = sqrt( saturate( 1.0 - cos_alpha_beta*cos_alpha_beta ) );           // Saturate to avoid NaN if ever cos_alpha > 1 (it happens with floating-point precision)
	float   C = sin_alpha_beta.x * sin_alpha_beta.y / (1e-6 + cos_alpha_beta.y);

	return _rho * (1/PI) * (A + B * max( 0.0, gamma ) * C);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// ====== Multiple-scattering BRDF computed from energy compensation ======
// http://patapom.com/blog/BRDF/MSBRDF/
/////////////////////////////////////////////////////////////////////////////////////////////////////
//

// Estimates the view-dependent part of the MSBRDF
float	MSBRDF_View( float _mu_o, float _roughness, uint _BRDFIndex ) {
	float	E_o = 1.0 - SampleIrradiance( _mu_o, _roughness, _BRDFIndex );	// 1 - E_o
	float	E_avg = SampleAlbedo( _roughness, _BRDFIndex );					// E_avg
	return E_o / max( 0.001, PI - E_avg );
}

// Estimates the full MSBRDF (view- and light-dependent)
float	MSBRDF( float _roughness, float3 _tsNormal, float3 _tsView, float3 _tsLight, uint _BRDFIndex ) {

	float	mu_o = saturate( dot( _tsView, _tsNormal ) );
	float	mu_i = saturate( dot( _tsLight, _tsNormal ) );
	float	a = _roughness;

	float	E_o = 1.0 - SampleIrradiance( mu_o, a, _BRDFIndex );	// 1 - E_o
	float	E_i = 1.0 - SampleIrradiance( mu_i, a, _BRDFIndex );	// 1 - E_i
	float	E_avg = SampleAlbedo( a, _BRDFIndex );					// E_avg

	return E_o * E_i / max( 0.001, PI - E_avg );
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Computes the full Single-Scattering + Multiple-Scattering BRDF model
////////////////////////////////////////////////////////////////////////////////////////////////
//
float3	ComputeBRDF_GGX( float3 _tsNormal, float3 _tsView, float3 _tsLight, float _roughness, float3 _F0, const bool _enableMS, const bool _enableSaturation ) {
		float3	BRDF = BRDF_GGX( _tsNormal, _tsView, _tsLight, _roughness, _F0 );
		// From http://patapom.com/blog/BRDF/MSBRDFEnergyCompensation/#varying-the-fresnel-reflectance-f_0f_0
		float3		MSFactor = _enableSaturation ? _F0 * (0.04 + _F0 * (0.66 + _F0 * 0.3)) : _F0;

		BRDF += MSFactor * MSBRDF( _roughness, _tsNormal, _tsView, _tsLight, FGD_BRDF_INDEX_GGX );

	return BRDF;
}

float3	ComputeBRDF_OrenNayar( float3 _tsNormal, float3 _tsView, float3 _tsLight, float _roughness, float3 _rho, const bool _enableMS, const bool _enableSaturation ) {
		float3	BRDF = BRDF_OrenNayar( _tsNormal, _tsView, _tsLight, _roughness, _rho );
		// From http://patapom.com/blog/BRDF/MSBRDFEnergyCompensation/#varying-diffuse-reflectance-rhorho
		const float	tau = 0.28430405702379613;
		const float	A1 = (1.0 - tau) / pow2( tau );
		float3		rho = tau * _rho;
		float3		MSFactor = _enableSaturation ? A1 * pow2( rho ) / (1.0 - rho) : rho;

		BRDF += MSFactor * MSBRDF( _roughness, _tsNormal, _tsView, _tsLight, FGD_BRDF_INDEX_OREN_NAYAR );

	return BRDF;
}

// Computes the full dielectric BRDF model as described in http://patapom.com/blog/BRDF/MSBRDFEnergyCompensation/#complete-approximate-model
//
float3	ComputeBRDF_Full( float3 _tsNormal, float3 _tsView, float3 _tsLight, float _roughnessSpecular, float3 _F0, float _roughnessDiffuse, float3 _rho, const bool _enableMS, const bool _enableSaturation ) {
	// Compute specular BRDF
		float3	BRDF_spec = BRDF_GGX( _tsNormal, _tsView, _tsLight, _roughnessSpecular, _F0 );

	float3	MSFactor_spec = _enableSaturation ? _F0 * (0.04 + _F0 * (0.66 + _F0 * 0.3)) : _F0;	// From http://patapom.com/blog/BRDF/MSBRDFEnergyCompensation/#varying-the-fresnel-reflectance-f_0f_0
		BRDF_spec += MSFactor_spec * MSBRDF( _roughnessSpecular, _tsNormal, _tsView, _tsLight, FGD_BRDF_INDEX_GGX );

	// Compute diffuse contribution
		float3	BRDF_diff = BRDF_OrenNayar( _tsNormal, _tsView, _tsLight, _roughnessDiffuse, _rho );
		const float	tau = 0.28430405702379613;
		const float	A1 = (1.0 - tau) / pow2( tau );
		float3		rho = tau * _rho;
		float3		MSFactor_diff = _enableSaturation ? A1 * pow2( rho ) / (1.0 - rho) : rho;	// From http://patapom.com/blog/BRDF/MSBRDFEnergyCompensation/#varying-diffuse-reflectance-rhorho

		BRDF_diff += MSFactor_diff * MSBRDF( _roughnessDiffuse, _tsNormal, _tsView, _tsLight, FGD_BRDF_INDEX_OREN_NAYAR );

	// Attenuate diffuse contribution
	float	mu_o = saturate( dot( _tsView, _tsNormal ) );
	float	a = _roughnessSpecular;
	float	E_o = SampleIrradiance( mu_o, a, FGD_BRDF_INDEX_GGX );	// Already sampled by MSBRDF earlier, optimize!

	float3	IOR = Fresnel_IORFromF0( _F0 );
	float3	Favg = FresnelAverage( IOR );
	float3	kappa = 1 - (Favg * E_o + MSFactor_spec * (1.0 - E_o));

	return BRDF_spec + kappa * BRDF_diff;
}
