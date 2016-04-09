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

#include "lodepng/lodepng.h"

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

    char line[80];
    int lh = 0;
    int llh = 0;
    while (gets(line)) {
	int bg, bm, lg, lm, bh;
	double dh;
	double du;
	if (line[0] == 'B') {
	    unsigned int *p;
	    sscanf(line, "%*7c%2d%5d%*c%3d%5d%*2c%5d", &bg, &bm, &lg,
		   &lm, &bh);
	    if (bh > 4000 && lh != 0 && llh != 0) {
		lat = bg + bm / 60000.0;
		lon = lg + lm / 60000.0;
		dh = ((bh - lh) + (lh - llh)) / 2;
		x = lat_rad(lat) * sin(lon_rad(lon)) + X_LON10;
		y = lat_rad(lat) * cos(lon_rad(lon)) + Y_LAT0;
		p = image + (y * width + x);
		if (*p == 0xFFFF00FF) {
		    du = 4.0;
		} else if (*p == 0xFFC837FF) {
		    du = 2.5;
		} else if (*p == 0xFF6F42FF) {
		    du = 1.5;
		} else if (*p == 0xFF4040FF) {
		    du = 1.0;
		} else if (*p == 0xFF5277FF) {
		    du = 0.75;
		} else if (*p == 0xFF6BC1FF) {
		    du = 0.5;
		} else if (*p == 0xFF80FFFF) {
		    du = 0.25;
		} else if (*p == 0xFFF3FFC1) {
		    du = -0.25;
		} else if (*p == 0xFFF8DC75) {
		    du = -0.5;
		} else if (*p == 0xFFFBC543) {
		    du = -0.75;
		} else if (*p == 0xFFFEA803) {
		    du = -1.0;
		} else if (*p == 0xFFFF9B2B) {
		    du = -1.5;
		} else if (*p == 0xFFFF8C59) {
		    du = -2.5;
		} else if (*p == 0xFFFF8080) {
		    du = -4.0;
		} else {
		    du = 0.0;
		}
		printf("%f %f\n", dh, du);
	    }
            llh = lh;
	    lh = bh;
	}
    }
/*B0905444743497N01226360EA008410096300308971*/

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
