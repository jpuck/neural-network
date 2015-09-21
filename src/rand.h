// ----------------------------------------------------------------
// The contents of this file are distributed under the CC0 license.
// See http://creativecommons.org/publicdomain/zero/1.0/
// ----------------------------------------------------------------

#ifndef RAND_H
#define RAND_H

#include <vector>


typedef unsigned long long int uint64;


// This is a 64-bit pseudo-random number generator. This class is superior
// to the standard C rand() method because it is faster, it is consistent
// across platforms, it makes bigger random numbers, and it can draw from
// several useful distributions. The only methods you will use in CS478 are
// setSeed, next, uniform, and normal. I just left the other methods here 
// for completeness.
class Rand
{
protected:
	uint64 m_a;
	uint64 m_b;

public:
	Rand(uint64 seed);
	~Rand();

	// Sets the seed
	void setSeed(uint64 seed);

	// Returns an unsigned pseudo-random 64-bit value
	uint64 next()
	{
		m_a = 0x141F2B69ull * (m_a & 0x3ffffffffull) + (m_a >> 32);
		m_b = 0xC2785A6Bull * (m_b & 0x3ffffffffull) + (m_b >> 32);
		return m_a ^ m_b;
	}

	// Returns a pseudo-random uint from a discrete uniform
	// distribution in the range 0 to range-1 (inclusive).
	// (This method guarantees the result will be drawn from
	// a uniform distribution, whereas doing "next() % range"
	// does not guarantee a truly uniform distribution.)
	uint64 next(uint64 range);

	// Returns a random value from a beta distribution
	double beta(double alpha, double beta);

	// Returns a random value from a binomial distribution
	int binomial(int n, double p);

	// Returns a random value from a categorical distribution
	// with the specified vector of category probabilities.
	int categorical(std::vector<double>& probabilities);

	// Returns a random value from a standard Cauchy distribution
	double cauchy();

	// Returns a random value from a chi-squared distribution
	double chiSquare(double t);

	// Returns a random vector from a dirichlet distribution with the
	// specified parameters and dimensionality. (Both pOutVec and
	// pParams should be an array of dims values.)
	void dirichlet(double* pOutVec, const double* pParams, int dims);

	// Returns a random value from a standard exponential distribution.
	// (To convert it to a random value from an arbitrary exponential
	// distribution, just divide the value this returns by the
	// rate (usually lambda), or if you use the scale parameterization,
	// just multiply the value this returns by the scale (usually beta)).
	double exponential();

	// Returns a random value from an f-distribution
	double f(double t, double u);

	// Returns a random value from a gamma distribution with beta=theta=1.
	// To convert to a value from an arbitrary gamma distribution,
	// just divide the value this returns by beta (or use alpha=k, and
	// multiply the value this returns by theta).
	double gamma(double alpha);

	// Returns a random value from a geometric distribution with support
	// for {0, 1, 2, ...}.
	int geometric(double p);

	// Returns a random value from a standard logistic distribution. To
	// convert to a random value from an arbitrary logistic distribution,
	// just multiply the value this returns by the scale (or divide by
	// the steepness), and then add the mean.
	double logistic();

	// Returns a random value from a log-Normal distribution
	double logNormal(double mean, double dev);

	// Returns a random value from a standard normal distribution. (To
	// convert it to a random value from an arbitrary normal distribution,
	// just multiply the value this returns by the deviation (usually
	// lowercase-sigma), then add the mean (usually mu).)
	double normal();

	// Returns a random value from a Poisson distribution
	int poisson(double mu);

	// Returns a random value from a soft-impulse distribution with support
	// from 0 to 1. (The cdf of the soft-impulse distribution is the soft-step
	// function: (1/(pow(1/x-1,s)+1)). The mean is always at 0.5, where
	// the probability-density is s.
	double softImpulse(double s);

	// Returns a random point on the surface of a dims-dimensional unit sphere
	void spherical(double* pOutVec, int dims);

	// Returns a random point within the volume of a dims-dimensional unit sphere
	void spherical_volume(double* pOutVec, int dims);

	// Returns a random point uniformly distributed within a unit cube
	void cubical(double* pOutVec, int dims);

	// Returns a random value from Student's t-distribution
	double student(double t);

	// Returns a pseudo-random double from 0 (inclusive)
	// to 1 (exclusive). This uses 52 random bits for the
	// mantissa, and discards the extra 12 random bits.
	double uniform();

	// Returns a random value from a Weibull distribution with lambda=1.
	double weibull(double gamma);
};



#endif // RAND_H
