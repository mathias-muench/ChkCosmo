/* vi:ai:ts=4:sw=4:noet
*/

#include <stdio.h>
#include <math.h>

/*
 * 4/50 = 65/51
 * 10/50 = 447/70
 * 16/50 = 829/51
 * 4/45 = 12/551
 * 10/45 = 447/574
 * 16/45 = 882/551
*/

#define X_LON10 447
#define Y_LAT45 574
#define R 4126
#define Y_LAT0 (Y_LAT45 - R)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double lon_rad(double lon)
{
	return (lon - 10) * M_PI / 180;
}

static double lat_rad(double lat)
{
	return ((R / 45.0) * ((94.5 - lat * 1.1)));
}

int fix2x(double lat, double lon)
{
	return lat_rad(lat) * sin(lon_rad(lon)) + X_LON10;
}

int fix2y(double lat, double lon)
{
	return lat_rad(lat) * cos(lon_rad(lon)) + Y_LAT0;
}

double forecast(unsigned int rgba) {
	double du;

	if (rgba == 0xFFFF00FF) {
		du = 4.0;
	} else if (rgba == 0xFFC837FF) {
		du = 2.5;
	} else if (rgba == 0xFF6F42FF) {
		du = 1.5;
	} else if (rgba == 0xFF4040FF) {
		du = 1.0;
	} else if (rgba == 0xFF5277FF) {
		du = 0.75;
	} else if (rgba == 0xFF6BC1FF) {
		du = 0.5;
	} else if (rgba == 0xFF80FFFF) {
		du = 0.25;
	} else if (rgba == 0xFFF3FFC1) {
		du = -0.25;
	} else if (rgba == 0xFFF8DC75) {
		du = -0.5;
	} else if (rgba == 0xFFFBC543) {
		du = -0.75;
	} else if (rgba == 0xFFFEA803) {
		du = -1.0;
	} else if (rgba == 0xFFFF9B2B) {
		du = -1.5;
	} else if (rgba == 0xFFFF8C59) {
		du = -2.5;
	} else if (rgba == 0xFFFF8080) {
		du = -4.0;
	} else if ((rgba & 0xFF) != 0 && (rgba & 0xFF) != 255 && rgba & 0xFF == (rgba >> 8) & 0xFF && rgba & 0xFF == (rgba >> 16) & 0xFF) {
		du = 0.0;
	} else {
		du = 99;
	}
}

#ifdef CU
#include "cu.h"
#include "assert.h"

static int fix2xy_corners()
{
/*
 * 4/50 = 65/51
 * 10/50 = 447/70
 * 16/50 = 829/51
 * 4/45 = 12/551
 * 10/45 = 447/574
 * 16/45 = 882/551
*/


}

int main(char **v, int c)
{
	assert(fix2x(50, 4) == 65);
	assert(fix2x(50, 10) == 447);
	return 0;
}
#endif
