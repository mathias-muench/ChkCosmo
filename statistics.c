#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "statistics.h"

static double zip(double q, double i, double j, double b)
{
	double zz = 1;
	double z = zz;
	double k = i;
	while (k <= j) {
		zz = zz * q * k / (k - b);
		z = z + zz;
		k = k + 2;
	}
	return z;
}

static double buzz(double t, int n)
{
	t = fabs(t);
	double rt = t / sqrt(n);
	double fk = atan(rt);
	if (n == 1) {
		return 1 - fk / (M_PI / 2);
	}
	double ek = sin(fk);
	double dk = cos(fk);
	if ((n % 2) == 1) {
		return 1 - (fk + ek * dk * zip(dk * dk, 2, n - 3, -1)) / (M_PI / 2);
	} else {
		return 1 - ek * zip(dk * dk, 1, n - 3, -1);
	}
}

/* see http://vassarstats.net/textbook/ch4apx.html and http://vassarstats.net/textbook/tcall.js */
double pvalue(int n, double r)
{
	int df = n - 2;
	double t_denom = sqrt((1 - (r * r)) / (n - 2));
	double t = r / t_denom;
	double p = buzz(t, df);

	return p;
}


