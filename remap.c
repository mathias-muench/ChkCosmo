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
#define PI 3.1415
#define Y_LAT0 (Y_LAT45 - R)

#include "lodepng.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static double lon_rad(double lon)
{
    return (lon - 10) * PI / 180;
}

static double lat_rad(double lat)
{
    return ((R / 45.0) * ((94.5 - lat * 1.1)));
}

void doit(const char *filename)
{
    unsigned error;
    unsigned int *image;
    unsigned width, height;
    double lon;
    double lat;
    int x, y;

    error =
	lodepng_decode32_file((unsigned char **) &image, &width, &height,
			      filename);
    if (error)
	printf("error %u: %s\n", error, lodepng_error_text(error));

    for (lat = 45; lat <= 50; lat += 5) {
	for (lon = 10; lon <= 16; lon += 0.001) {
	    unsigned int *p;
	    x = lat_rad(lat) * sin(lon_rad(lon)) + X_LON10;
	    y = lat_rad(lat) * cos(lon_rad(lon)) + Y_LAT0;
	    p = image + (y * width + x);
	    *p = 0xffffffff;
	}
    }

    for (lon = 4; lon <= 16; lon += 6) {
	for (lat = 45; lat <= 50; lat += 0.001) {
	    unsigned int *p;
	    x = lat_rad(lat) * sin(lon_rad(lon)) + X_LON10;
	    y = lat_rad(lat) * cos(lon_rad(lon)) + Y_LAT0;
	    p = image + (y * width + x);
	    *p = 0xffffffff;
	}

	char line[80];
	int skip = 0;
	int lh = 0;
	while (gets(line)) {
	    int bg, bm, lg, lm, bh, dh;
	    if (line[0] == 'B' && skip++ % 1 == 0) {
		unsigned int *p;
		sscanf(line, "%*7c%2d%5d%*c%3d%5d%*2c%5d", &bg, &bm, &lg,
		       &lm, &bh);
		if (lh != 0) {
		    dh = bh - lh;
		    lat = bg + bm / 60000.0;
		    lon = lg + lm / 60000.0;
		    dh = bh - lh;
		    printf("%f %f %d\n", lat, lon, dh);
		    x = lat_rad(lat) * sin(lon_rad(lon)) + X_LON10;
		    y = lat_rad(lat) * cos(lon_rad(lon)) + Y_LAT0;
		    p = image + (y * width + x);
		    *p = 0xff000000;
		}
		lh = bh;
	    }
	}
/*B0905444743497N01226360EA008410096300308971*/

    }

    error =
	lodepng_encode32_file("xx.png", (unsigned char *) image, width,
			      height);
    if (error)
	printf("error %u: %s\n", error, lodepng_error_text(error));

    free(image);
}

int main(int argc, char *argv[])
{
    const char *filename = argc > 1 ? argv[1] : "test.png";

    doit(filename);

    return 0;
}
