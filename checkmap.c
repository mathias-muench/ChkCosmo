#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lodepng/lodepng.h"
#include "transform.h"

static void checktransform(unsigned int *image, unsigned width, unsigned height) {
	for (double lon = 6.0; lon < 16.0; lon += 1.0/60) {
		int x = fix2x(50.0, lon);
		int y = fix2y(50.0, lon);
		unsigned int *p = image + (y * width + x);
		*p = 0xFFFFFFFF;
		x = fix2x(45.0, lon);
		y = fix2y(45.0, lon);
		p = image + (y * width + x);
		*p = 0xFFFFFFFF;
	}
	for (double lat = 45.0; lat < 50.0; lat += 1.0/60) {
		int x = fix2x(lat, 6.0);
		int y = fix2y(lat, 6.0);
		unsigned int *p = image + (y * width + x);
		*p = 0xFFFFFFFF;
		x = fix2x(lat, 16.0);
		y = fix2y(lat, 16.0);
		p = image + (y * width + x);
		*p = 0xFFFFFFFF;
	}
}

static void checkforecast(unsigned int *image, unsigned width, unsigned height) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned int *p = image + (y * width + x);
			if (forecast(*p) != 99) {
				*p = 0xFFFFFFFF;
			}
		}
	}
}

static void doit(const char* infilename, const char* outfilename, int check)
{
  unsigned error;
  unsigned char* image;
  unsigned width, height;

  error = lodepng_decode32_file(&image, &width, &height, infilename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  switch(check) {
  case 1:
    checktransform((unsigned int *)image, width, height);
    break;
  case 2:
    checkforecast((unsigned int *)image, width, height);
    break;
  }

  error = lodepng_encode32_file(outfilename, image, width, height);

  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  free(image);
}

int main(int argc, char *argv[])
{
	int check = 0;
	int opt;
	while ((opt = getopt(argc, argv, "tf")) != -1) {
		switch (opt) {
		case 't':
			check = 1;
			break;
		case 'f':
			check = 2;
			break;
		default:			/* '?' */
			fprintf(stderr, "Invalid option\n");
			exit(1);
		}
	}
  const char* infilename = argc > optind ? argv[optind] : "test.png";
  optind++;
  const char* outfilename = argc > optind ? argv[optind] : "checkmap.png";
  optind++;

  doit(infilename, outfilename, check);

  return 0;
}
