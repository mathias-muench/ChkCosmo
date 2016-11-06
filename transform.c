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
