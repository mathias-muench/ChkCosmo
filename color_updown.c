/*
LodePNG Examples

Copyright (c) 2005-2012 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#include "lodepng.h"

#include <stdio.h>
#include <stdlib.h>

/*
3 ways to decode a PNG from a file to RGBA pixel data (and 2 in-memory ways).
*/

/*
Example 1
Decode from disk to raw pixels with a single function call
*/
void decodeOneStep(const char* filename)
{
  unsigned error;
  unsigned char* image;
  unsigned width, height;
  int x, y;

  error = lodepng_decode24_file(&image, &width, &height, filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  /*use image here*/
  for (y = 0; y < height; y++) {
	  for (x = 0; x < width * 3; x += 3) {
		  unsigned char *r, *g, *b;
#if 0
		  printf("%02x %02x %02x\n", image[y * width + x * 3], image[y * width + x * 3 + 1], image[y * width + x * 3 + 2]);
#endif
		  r = image + (y * width * 3 + x);
		  g = image + (y * width * 3 + x + 1);
		  b = image + (y * width * 3 + x + 2);
		  if (!(*r == 193 && *g == 255 && *b == 243)) {
			*r = *g = *b = 0;
		  }
	}
  }

  error = lodepng_encode24_file("xx.png", image, width, height);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  free(image);
}

int main(int argc, char *argv[])
{
  const char* filename = argc > 1 ? argv[1] : "test.png";

  decodeOneStep(filename);

  return 0;
}

