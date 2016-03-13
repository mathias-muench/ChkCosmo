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

static double lon_rad(double lon) {
	return (lon - 10) * PI / 180;
}

static double lat_rad(double lat) {
	return ((R / 45.0) * ((94.5 - lat * 1.1)));
}

void doit(const char* filename)
{
  unsigned error;
  unsigned char* image;
  unsigned width, height;
  double lon;
  double lat;
  int x, y;

  error = lodepng_decode24_file(&image, &width, &height, filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  for (lat = 45; lat <= 50; lat += 5) {
  for (lon = 10; lon <= 16; lon += 0.001) {
	  unsigned char *r, *g, *b;
	  x = lat_rad(lat) * sin(lon_rad(lon)) + X_LON10;
	  y = lat_rad(lat) * cos(lon_rad(lon)) + Y_LAT0;
	  r = image + ((y * width + x) * 3 + 0);
	  g = image + ((y * width + x) * 3 + 1);
	  b = image + ((y * width + x) * 3 + 2);
	  *r = *g = *b = 255;
  }
  }

  for (lon = 4; lon <= 16; lon += 6) {
  for (lat = 45; lat <= 50; lat += 0.001) {
	  unsigned char *r, *g, *b;
	  x = lat_rad(lat) * sin(lon_rad(lon)) + X_LON10;
	  y = lat_rad(lat) * cos(lon_rad(lon)) + Y_LAT0;
	  r = image + ((y * width + x) * 3 + 0);
	  g = image + ((y * width + x) * 3 + 1);
	  b = image + ((y * width + x) * 3 + 2);
	  *r = *g = *b = 255;
  }

  char line[80];
  while (gets(line)) {
    int bg, bm, lg, lm;
    if (line[0] == 'B') {
	  unsigned char *r, *g, *b;
      sscanf(line, "%*7c%2d%5d%*c%3d%5d", &bg, &bm, &lg, &lm);
	lat = bg + bm / 60000.0;
        lon = lg + lm / 60000.0;
printf("%f %f\n", lat, lon);
	  x = lat_rad(lat) * sin(lon_rad(lon)) + X_LON10;
	  y = lat_rad(lat) * cos(lon_rad(lon)) + Y_LAT0;
	  r = image + ((y * width + x) * 3 + 0);
	  g = image + ((y * width + x) * 3 + 1);
	  b = image + ((y * width + x) * 3 + 2);
	  *r = *g = *b = 0;
    }
}
/*B0905444743497N01226360EA008410096300308971*/

}

  error = lodepng_encode24_file("xx.png", image, width, height);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  free(image);
}

int main(int argc, char *argv[])
{
  const char* filename = argc > 1 ? argv[1] : "test.png";

  doit(filename);

  return 0;
}
