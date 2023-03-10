#pragma once
#include <stdint.h>
#include <immintrin.h>

/*Approximation by:
 * https://gist.github.com/publik-void/067f7f2fef32dbe5c27d6e215f824c91
 * 
 *
 * This implementation requires:
 *  -sse4.1 -> some 99.32 % of modern machines have this according to steam hardware survey
 */



/*   _ _       
 *  /   \      
 *        \_ _/
 *  11 22 33 44
 *
 *  Ranges        
 *  [   0, 0.25]   sinApprox(x_norm)
 *  [0.25, 0.50]   sinApprox(0.5 - x_norm)
 *  [0.50, 0.75]  -sinApprox(x_norm - 0.5)
 *  [0.75, 1.00]  -sinApprox(0.25 - x_norm + 0.75))
 */
extern "C"
__m128
sinSSE4_1(__m128 xps)
{
	/*
	 * int32_t xi = x;
	 * float x_norm = x - xi;
	 * float factor = 1;
	 * if(x_norm < 0)
	 * {
	 *  x_norm += 1;
	 * }
	 */
	__m128 x_norm = _mm_sub_ps(xps,_mm_cvtepi32_ps(_mm_cvtps_epi32(xps)));
	x_norm = _mm_blendv_ps(x_norm, _mm_add_ps(x_norm, _mm_set1_ps(1.0f)), _mm_cmplt_ps(x_norm, _mm_setzero_ps()));
	__m128 quat          = _mm_set1_ps(0.25f);
	__m128 half          = _mm_set1_ps(0.5f);
	__m128 threequat     = _mm_set1_ps(0.75f);
	__m128 factor        = _mm_set1_ps(1.0f);
	__m128 const_neg_one = _mm_set1_ps(-1.0f);
	__m128 x_val = x_norm;

	/*if(x_norm >= 0.25f && x_norm < 0.5f)
	 *{
	 *   x_val = 0.5f - x_norm;
	 *}
	 */
	{
		__m128 cmp = _mm_and_ps(_mm_cmpge_ps(x_norm, quat), _mm_cmplt_ps(x_norm, half));
		x_val = _mm_blendv_ps(x_val,_mm_sub_ps(half,x_norm),cmp);
	}
	/*
	 * if(x_norm >= 0.5f && x_norm <= 0.75f)
	 * {
	 *  factor = -1.f;
	 *  x_val = x_norm - 0.5f;
	 * }
	 */
	{
		__m128 cmp = _mm_and_ps(_mm_cmpge_ps(x_norm, half), _mm_cmple_ps(x_norm, threequat));
		x_val = _mm_blendv_ps(x_val,_mm_sub_ps(x_norm, half),cmp);
		factor = _mm_blendv_ps(factor,const_neg_one, cmp);
	}
	/*
	 *if(x_norm > 0.75f && x_norm <= 1.f)
	 *{
	 * factor = -1.f;
	 * x_val = 1 - x_norm;
	 *}
	 */
	{
		__m128 cmp = _mm_cmpgt_ps(x_norm, threequat);
		x_val = _mm_blendv_ps(x_val,_mm_sub_ps(_mm_set1_ps(1.0f), x_norm),cmp);
		factor = _mm_blendv_ps(factor,const_neg_one, cmp);
	}

	__m128 x2 = _mm_mul_ps(x_val, x_val);
	__m128 a  = _mm_set1_ps(6.28250560008354834003487064338056964f);
	__m128 b  = _mm_set1_ps(-41.1664423308903732524414077000881397f);
	__m128 c  = _mm_set1_ps(74.4524187069072428211419543641796188f);
	x_val = _mm_mul_ps(_mm_mul_ps(x_val, 
		_mm_add_ps(a, _mm_mul_ps(x2, 
			_mm_add_ps(b, _mm_mul_ps(x2, c))
		))
	), factor);
	return x_val;
}

extern "C"
float
sinSlow(float x)
{
#if 1
	int32_t xi = x;
	float x_norm = x - xi;
	float factor = 1;

	if(x_norm < 0)
	{
		x_norm += 1;
	}

	float x_val = x_norm;
	
	if(x_norm >= 0.25f && x_norm < 0.5f)
	{
		x_val = 0.5f - x_norm;
	}
	if(x_norm >= 0.5f && x_norm <= 0.75f)
	{
		factor = -1.f;
		x_val = x_norm - 0.5f;
	}
	if(x_norm > 0.75f && x_norm <= 1.f)
	{
		factor = -1.f;
		x_val = 1 - x_norm;
	}
	float x1 = x_val;
	float x2 = x_val * x_val;
	x_val = x1*(6.28250560008354834003487064338056964f + x2*(-41.1664423308903732524414077000881397f + 74.4524187069072428211419543641796188f*x2));
	return factor * x_val;
#else
	__m128 xps = _mm_set1_ps(x);
	return _mm_cvtss_f32(sin4(xps));
#endif
}




/* _         _
 *  \       /
 *    \_ _/ 
 * 11 22 33 44
 *
 *  Ranges        
 *  [   0, 0.25]   cosApprox(x_norm)
 *  [0.25, 0.50]  -cosApprox(0.5 - x_norm)
 *  [0.50, 0.75]  -cosApprox(x_norm - 0.5)
 *  [0.75, 1.00]   cosApprox(0.25 - x_norm + 0.75))
 */
extern "C" 
__m128
cosSSE4_1(__m128 xps)
{
	/*
	 * int32_t xi = x;
	 * float x_norm = x - xi;
	 * float factor = 1;
	 * if(x_norm < 0)
	 * {
	 *  x_norm += 1;
	 * }
	 */
	__m128 x_norm = _mm_sub_ps(xps,_mm_cvtepi32_ps(_mm_cvtps_epi32(xps)));
	x_norm = _mm_blendv_ps(x_norm, _mm_add_ps(x_norm, _mm_set1_ps(1.0f)), _mm_cmplt_ps(x_norm, _mm_setzero_ps()));
	__m128 quat          = _mm_set1_ps(0.25f);
	__m128 half          = _mm_set1_ps(0.5f);
	__m128 threequat     = _mm_set1_ps(0.75f);
	__m128 factor        = _mm_set1_ps(1.0f);
	__m128 const_neg_one = _mm_set1_ps(-1.0f);
	__m128 x_val = x_norm;
	/*
	 * if(x_norm >= 0.25f && x_norm < 0.5f)
	 * {
	 * 	factor = -1.f;
	 * 	x_val = 0.5f - x_norm;
	 * }
	 */
	{
		__m128 cmp = _mm_and_ps(_mm_cmpge_ps(x_norm, quat), _mm_cmplt_ps(x_norm, half));
		x_val = _mm_blendv_ps(x_val,_mm_sub_ps(half, x_norm),cmp);
		factor = _mm_blendv_ps(factor,const_neg_one, cmp);
	}

	/*
	 *if(x_norm >= 0.5f && x_norm <= 0.75f)
	 *{
	 *	factor = -1.f;
	 *	x_val = x_norm - 0.5f;
	 *}
	 */
	{
		__m128 cmp = _mm_and_ps(_mm_cmpge_ps(x_norm, half), _mm_cmple_ps(x_norm, threequat));
		x_val = _mm_blendv_ps(x_val,_mm_sub_ps(x_norm, half),cmp);
		factor = _mm_blendv_ps(factor, const_neg_one, cmp);
	}
	
	/*
	 * if(x_norm > 0.75f && x_norm <= 1.f)
	 * {
	 * 	x_val = 1 - x_norm;
	 * }
	 */
	{
		__m128 cmp = _mm_cmpgt_ps(x_norm, threequat);
		x_val = _mm_blendv_ps(x_val,_mm_sub_ps(_mm_set1_ps(1.0f), x_norm),cmp);
	}
	
	__m128 x2 = _mm_mul_ps(x_val, x_val);
	__m128 a = _mm_set1_ps(0.997372645040477990699027658698347186f);
	__m128 b = _mm_set1_ps(-19.3825703453028767024917664241925739f);
	__m128 c = _mm_set1_ps(54.7937283944836616879859645265254083f);
	x_val = _mm_mul_ps(factor, 
		_mm_add_ps(a, _mm_mul_ps(x2, 
			_mm_add_ps(b, _mm_mul_ps(c, x2))
		))
	);
	return x_val;
}

extern "C"
float
cosSlow(float x)
{
	int32_t xi = x;
	float x_norm = x - xi;
	float factor = 1;

	if(x_norm < 0)
	{
		x_norm += 1;
	}

	float x_val = x_norm;
	if(x_norm >= 0.25f && x_norm < 0.5f)
	{
		factor = -1.f;
		x_val = 0.5f - x_norm;
	}
	if(x_norm >= 0.5f && x_norm <= 0.75f)
	{
		factor = -1.f;
		x_val = x_norm - 0.5f;
	}
	if(x_norm > 0.75f && x_norm <= 1.f)
	{
		x_val = 1 - x_norm;
	}
	float x2 =  x_val * x_val;
	x_val = 0.997372645040477990699027658698347186f + x2*(-19.3825703453028767024917664241925739f + 54.7937283944836616879859645265254083f*x2);
	return factor * x_val;
}



