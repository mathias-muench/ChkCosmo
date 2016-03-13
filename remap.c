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

#include "lodepng.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double lon_rad(double lon) {
	return (lon - 10) * PI / 180;
}

void doit(const char* filename)
{
  unsigned error;
  unsigned char* image;
  unsigned width, height;
  double lon;
  double lat = 45;
  int x, y;

  error = lodepng_decode24_file(&image, &width, &height, filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  /*use image here*/
  for (lon = 10; lon <= 16; lon += 0.001) {
	  unsigned char *r, *g, *b;
	  x = X_LON10 + R * sin(lon_rad(lon));
	  y = R * cos(lon_rad(lon)) - (R - Y_LAT45);
printf("%d %d\n", x, y);
	  r = image + ((y * width + x) * 3 + 0);
	  g = image + ((y * width + x) * 3 + 1);
	  b = image + ((y * width + x) * 3 + 2);
	  *r = *g = *b = 255;
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

