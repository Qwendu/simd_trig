#include <stdio.h>



#include "simd_trig.hpp"

#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>


union f4
{
	float data[4];
	__m128 xmm;
};



int main()
{
	float volatile a;
	constexpr int n = 1000;
	printf("x, sin(x), sin_err_seq(x), sin_err(x), cos(x), cos_err_seq(x), cos_err(x)\n");
	for(int i = - 3 * n; i < 5 * n; i++)
	{
		float x = (float)i/n;
		f4 xw = {};
		f4 scntrl = {};
		f4 ccntrl = {};
		for(int j = 0; j < 4; j++)
		{
			xw.data[j] = x + j * 0.25;
			scntrl.data[j] = sinSlow(xw.data[j]);
			ccntrl.data[j] = cosSlow(xw.data[j]);
		}
		f4 sres = {.xmm = sinSSE4_1(xw.xmm)};
		f4 cres = {.xmm = cosSSE4_1(xw.xmm)};
		for(int j = 0; j < 4; j++)
		{
			printf("%f, %f, %f, %f, %f, %f, %f\n", xw.data[j]
				, sres.data[j], (sres.data[j] - scntrl.data[j]), (sres.data[j] - sinf(xw.data[j] * 2 * M_PI))
				, cres.data[j], (cres.data[j] - ccntrl.data[j]), (sres.data[j] - sinf(xw.data[j] * 2 * M_PI))
			);
		}
	}
}
