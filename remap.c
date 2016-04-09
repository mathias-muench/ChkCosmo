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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lodepng/lodepng.h"
#include "igcrecords.h"

static double lon_rad(double lon)
{
    return (lon - 10) * PI / 180;
}

static double lat_rad(double lat)
{
    return ((R / 45.0) * ((94.5 - lat * 1.1)));
}


static int load_images(unsigned *out[24][2], unsigned *w, unsigned *h, const char *dirname)
{
	int result = 0;
	int altitudes[] = { 3000, 5000 };

	int i, j;
	for (j = 0; j < 24; j++) {
		for (i = 0; i < 2; i++) {
			/* nb_cosde_ome_alpen_lv_003000_vt_141104_1200.png  */
			char filename[FILENAME_MAX];
			sprintf(filename, "%s/nb_cosde_ome_alpen_lv_%06d_vt_%s_%02d00.png", dirname, altitudes[i], "141104", j);

			unsigned error;
			error = lodepng_decode32_file((unsigned char **) &out[j][i], w, h, filename);
			if (error) {
				fprintf(stderr, "error %u: %s\n", error, lodepng_error_text(error));
				result = -1;
			}
		}
	}

	return result;
}

static void free_images(unsigned *images[24][2]) {
	int i, j;
	for (j = 0; j < 24; j++) {
		for (i = 0; i < 2; i++) {
			free(images[j][i]);
		}
	}
}

void doit(const char *dirname)
{
	int error;
	unsigned *images[24][2];
	unsigned width, height;

    error = load_images(images, &width, &height, dirname);
    if (error)
      exit(1);


    char line[80];
	struct fix lf = { 0 };
	double le = 0.0;
	while (fgets(line, 80, stdin)) {
		if (line[0] == 'B') {
			struct fix bf;
			b_record_to_fix(line, &bf);
			if (lf.alt != 0.0) {
				double be = fix_e_kin(&bf, &lf);
				if (le != 0.0) {
					size_t alt;
					if (bf.alt >= 4000) {
						alt = 1;
					} else if ( bf.alt >= 3000 ) {
						alt = 0;
					} else {
						alt = 2;
					}
					if (alt != 2) {
						double dh;
						double de;
						double du;
						struct fix df;
						fix_delta(&df, &bf, &lf);
						dh = df.alt / (df.time * 3600);
						de = (be - le) / 9.81;
#if 0
						dh += de;
#endif

						int x, y;
						x = lat_rad(bf.lat) * sin(lon_rad(bf.lon)) + X_LON10;
						y = lat_rad(bf.lat) * cos(lon_rad(bf.lon)) + Y_LAT0;

						unsigned int *p;
						p = images[(int)bf.time][alt] + (y * width + x);
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

						printf("%6.2f %6.2f\n", dh, du);
					}
				}
				le = be;
			}
			lf = bf;
		}
	}

	free_images(images);
}

int main(int argc, char *argv[])
{
    const char *dirname = argc > 1 ? argv[1] : "test.png";

    doit(dirname);

    return 0;
}

/* vi:ai:sw=4:ts=4:noet
*/
