// ----------------------------------------------------------------
// The contents of this file are distributed under the CC0 license.
// See http://creativecommons.org/publicdomain/zero/1.0/
// ----------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "rand.h"
#include "error.h"

using std::vector;

	
Rand::Rand(uint64 seed)
{
	setSeed(seed);
}

Rand::~Rand()
{
}

void Rand::setSeed(uint64 seed)
{
	m_b = 0xCA535ACA9535ACB2ull + seed;
	m_a = 0x6CCF6660A66C35E7ull + (seed << 24);
}

uint64 Rand::next(uint64 range)
{
	// Use rejection to find a random value in a range that is a multiple of "range"
	uint64 n = (0xffffffffffffffffull % range) + 1;
	uint64 x;
	do
	{
		x = next();
	} while((x + n) < n);

	// Use modulus to return the final value
	return x % range;
}

double Rand::uniform()
{
	// use 52 random bits for the mantissa
	return (double)(next() & 0xfffffffffffffull) / 4503599627370496.0;
}

double Rand::normal()
{
	double x, y, mag;
	do
	{
		x = uniform() * 2 - 1;
		y = uniform() * 2 - 1;
		mag = x * x + y * y;
	} while(mag >= 1.0 || mag == 0);
	return y * sqrt(-2.0 * log(mag) / mag); // the Box-Muller transform	
}

int Rand::categorical(vector<double>& probabilities)
{
	double d = uniform();
	int i = 0;
	for(vector<double>::iterator it = probabilities.begin(); it != probabilities.end(); it++)
	{
		d -= *it;
		if(d < 0)
			return i;
		i++;
	}
	throw Ex("the probabilities are not normalized");
	return (int)probabilities.size() - 1;
}

double Rand::exponential()
{
	return -log(uniform());
}

double Rand::cauchy()
{
	return normal() / normal();
}

double GMath_logGamma(double x)
{
#ifdef WIN32
	double x0, x2, xp, gl, gl0;
	int n = 0;
	int k;
	static double a[] =
	{
		8.333333333333333e-02, // 1/12
		-2.777777777777778e-03, // -1/360
		7.936507936507937e-04, // 1/1260
		-5.952380952380952e-04, // -1/1680
		8.417508417508418e-04, // 1/1188
		-1.917526917526918e-03, // -691/360360
		6.410256410256410e-03, // 1/156
		-2.955065359477124e-02,
		1.796443723688307e-01,
		-1.39243221690590
	};

	x0 = x;
	if (x <= 0.0)
		return 1e308;
	else if ((x == 1.0) || (x == 2.0))
		return 0.0;
	else if (x <= 7.0)
	{
		n = (int)(7 - x);
		x0 = x + n;
	}
	x2 = 1.0 / (x0 * x0);
	xp = 2.0 * M_PI;
	gl0 = a[9];
	for (k = 8; k >= 0; k--)
		gl0 = gl0 * x2 + a[k];
	gl = gl0 / x0 + 0.5 * log(xp) + (x0 - 0.5) * log(x0) - x0;
	if (x <= 7.0)
	{
		for (k = 1; k <= n; k++)
		{
			gl -= log(x0 - 1.0);
			x0 -= 1.0;
		}
	}
	return gl;
#else
	return lgamma(x);
#endif
}

int Rand::poisson(double mu)
{
	if(mu <= 0)
		throw Ex("invalid parameter");
	double p = 1.0;
	int n = 0;
	if(mu < 30)
	{
		mu = exp(-mu);
		do {
			p *= uniform();
			n++;
		} while(p >= mu);
		return n - 1;
	}
	else
	{
		double u1, u2, x, y;
		double c = 0.767-3.36 / mu;
		double b = M_PI / sqrt(3.0 * mu);
		double a = b * mu;
		if(c <= 0)
			throw Ex("Error generating Poisson deviate");
		double k = log(c) - mu - log(b);
		double ck1 = 0.0;
		double ck2;
		do {
			ck2=0.;
			do {
				u1 = uniform();
				x = (a - log(0.1e-18 + (1.0 - u1) / u1)) / b;
				if(x > -0.5)
					ck2=1.0;
			} while (ck2<0.5);
			n = (int)(x + 0.5);
			u2 = uniform();
			y = 1 + exp(a - b * x);
			ck1 = a - b * x + log(.1e-18 + u2/(y * y));
#ifdef WIN32
			ck2 = k + n * log(.1e-18 + mu) - GMath_logGamma(n + 1.0);
#else
			ck2 = k + n * log(.1e-18 + mu) - lgamma(n + 1.0);
#endif
			if(ck1 <= ck2)
				ck1 = 1.0;
		} while (ck1 < 0.5);
		return n;
	}
}

double Rand::gamma(double alpha)
{
	double x;
	if(alpha <= 0)
		throw Ex("invalid parameter");
	if(alpha == 1)
		return exponential();
	else if(alpha < 1)
	{
		double aa = (alpha + M_E) / M_E;
		double r1, r2;
		do {
			r1 = uniform();
			r2 = uniform();
			if(r1 > 1.0 / aa)
			{
				x = -log(aa * (1.0 - r1) / alpha);
				if(r2 < pow(x, (alpha - 1.0)))
					return x;
			}
			else
			{
				x = pow((aa * r1), (1.0 / alpha));
				if(r2 < exp(-x))
					return x;
			}
		} while(r2 < 2);
	}
	else
	{
		double c1 = alpha-1;
		double c2 = (alpha - 1.0 / (6.0 * alpha)) / c1;
		double c3 = 2.0 / c1;
		double c4 = c3 + 2.0;
		double c5 = 1.0 / sqrt(alpha);
		double r1, r2;
		do {
			do {
				r1 = uniform();
				r2 = uniform();
				if(alpha > 2.5)
					r1 = r2 + c5 * (1.0 - 1.86 * r1);
			} while(r1 <= 0 || r1 >= 1);
			double w = c2 * r2 / r1;
			if((c3 * r1) + w + (1.0 / w) <= c4)
				return c1 * w;
			if((c3 * log(r1)) - log(w) + w < 1)
				return c1 * w;
		} while(r2 < 2);
	}
	throw Ex("Error making random gamma");
	return 0;
}

double Rand::chiSquare(double t)
{
	return gamma(t / 2.0) * 2.0;
}

int Rand::binomial(int n, double p)
{
	int c = 0;
	for(int i = 0; i < n; i++)
	{
		if(uniform() < p)
			c++;
	}
	return c;
}

double Rand::softImpulse(double s)
{
	double y = uniform();
	return 1.0 / (1.0 + pow(1.0 / y - 1.0, 1.0 / s));
}

double Rand::weibull(double gamma)
{
	if(gamma <= 0)
		throw Ex("invalid parameter");
	return pow(exponential(), (1.0 / gamma));
}

void Rand::dirichlet(double* pOutVec, const double* pParams, int dims)
{
	double* pOut = pOutVec;
	const double* pIn = pParams;
	for(int i = 0; i < dims; i++)
		*(pOut++) = gamma(*(pIn++));
	double sum = 0.0;
	pOut = pOutVec;
	for(int i = 0; i < dims; i++)
		sum += *(pOutVec++);
	sum = 1.0 / sum;
	pOut = pOutVec;
	for(int i = 0; i < dims; i++)
	{
		*pOutVec *= sum;
		pOutVec++;
	}
}

double Rand::student(double t)
{
	if(t <= 0)
		throw Ex("invalid parameter");
	return normal() / sqrt(chiSquare(t) / t);
}

int Rand::geometric(double p)
{
	if(p < 0 || p > 1)
		throw Ex("invalid parameter");
	return (int)floor(-exponential() / log(1.0 - p));
}

double Rand::f(double t, double u)
{
	if(t <= 0 || u <= 0)
		throw Ex("invalid parameters");
	return chiSquare(t) * u / (t * chiSquare(u));
}

double Rand::logistic()
{
	double y = uniform();
	return log(y) - log(1.0 - y);
}

double Rand::logNormal(double mean, double dev)
{
	return exp(normal() * dev + mean);
}

double Rand::beta(double alpha, double beta)
{
	if(alpha <= 0 || beta <= 0)
		throw Ex("invalid parameters");
	double r = gamma(alpha);
	return r / (r + gamma(beta));
}

double GVec_squaredMagnitude(const double* pVector, int nSize)
{
	double dMag = 0;
	while(nSize > 0)
	{
		dMag += ((*pVector) * (*pVector));
		pVector++;
		nSize--;
	}
	return dMag;
}

void GVec_multiply(double* pVector, double dScalar, int nDims)
{
	for(int i = 0; i < nDims; i++)
	{
		*pVector *= dScalar;
		pVector++;
	}
}

void GVec_safeNormalize(double* pVector, int nSize, Rand* pRand)
{
	double dMag = GVec_squaredMagnitude(pVector, nSize);
	if(dMag <= 0)
		pRand->spherical(pVector, nSize);
	else
		GVec_multiply(pVector, 1.0  / sqrt(dMag), nSize);
}

void Rand::spherical(double* pOutVec, int dims)
{
	double* pEl = pOutVec;
	for(int i = 0; i < dims; i++)
		*(pEl++) = normal();
	GVec_safeNormalize(pOutVec, dims, this);
}

void Rand::spherical_volume(double* pOutVec, int dims)
{
	spherical(pOutVec, dims);
	GVec_multiply(pOutVec, pow(uniform(), 1.0 / dims), dims);
}

void Rand::cubical(double* pOutVec, int dims)
{
	double* pEl = pOutVec;
	for(int i = 0; i < dims; i++)
		*(pEl++) = uniform();
}

