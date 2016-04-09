#include <stdio.h>
#include <stdlib.h>

#include "lodepng/lodepng.h"

void doit(const char *filename)
{
    unsigned error;
    unsigned int *image;
    unsigned width, height;
    int x, y;

    error =
	lodepng_decode32_file((unsigned char **) &image, &width, &height,
			      filename);
    if (error)
	printf("error %u: %s\n", error, lodepng_error_text(error));

    /*use image here */
    for (y = 0; y < height; y++) {
	for (x = 0; x < width; x++) {
	    unsigned *p = image + (y * width + x);
	    if (*p == 0xFFFF00FF) {
		*p = 0xfff0f0f0;
	    } else if (*p == 0xFFC837FF) {
		*p = 0xffe0e0e0;
	    } else if (*p == 0xFF6F42FF) {
		*p = 0xffd0d0d0;
	    } else if (*p == 0xFF4040FF) {
		*p = 0xffc0c0c0;
	    } else if (*p == 0xFF5277FF) {
		*p = 0xffb0b0b0;
	    } else if (*p == 0xFF6BC1FF) {
		*p = 0xffa0a0a0;
	    } else if (*p == 0xFF80FFFF) {
		*p = 0xff909090;
	    } else if (*p == 0xFFF3FFC1) {
		*p = 0xff606060;
	    } else if (*p == 0xFFF8DC75) {
		*p = 0xff505050;
	    } else if (*p == 0xFFFBC543) {
		*p = 0xff404040;
	    } else if (*p == 0xFFFEA803) {
		*p = 0xff303030;
	    } else if (*p == 0xFFFF9B2B) {
		*p = 0xff202020;
	    } else if (*p == 0xFFFF8C59) {
		*p = 0xff101010;
	    } else if (*p == 0xFFFF8080) {
		*p = 0xff000000;
	    } else {
		*p = 0xffff0000;
	    }
	}
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
