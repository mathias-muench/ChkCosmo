#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "lodepng/lodepng.h"
#include "kvec.h"
#include "igcrecords.h"

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

#define MEAN_INTERVAL 20

static double zip(double q, double i, double j, double b)
{
	double zz = 1;
	double z = zz;
	double k = i;
	while (k <= j) {
		zz = zz * q * k / (k - b);
		z = z + zz;
		k = k + 2;
	}
	return z;
}

static double buzz(double t, int n)
{
	t = fabs(t);
	double rt = t / sqrt(n);
	double fk = atan(rt);
	if (n == 1) {
		return 1 - fk / (M_PI / 2);
	}
	double ek = sin(fk);
	double dk = cos(fk);
	if ((n % 2) == 1) {
		return 1 - (fk + ek * dk * zip(dk * dk, 2, n - 3, -1)) / (M_PI / 2);
	} else {
		return 1 - ek * zip(dk * dk, 1, n - 3, -1);
	}
}

/* see http://vassarstats.net/textbook/ch4apx.html and http://vassarstats.net/textbook/tcall.js */
static double pvalue(int n, double r)
{
	int df = n - 2;
	double t_denom = sqrt((1 - (r * r)) / (n - 2));
	double t = r / t_denom;
	printf("%f\n", t);
	printf("%d\n", df);
	double p = buzz(t, df);

	return p;
}

static double lon_rad(double lon)
{
	return (lon - 10) * M_PI / 180;
}

static double lat_rad(double lat)
{
	return ((R / 45.0) * ((94.5 - lat * 1.1)));
}


static int load_images(unsigned *out[24][2], unsigned *w, unsigned *h, const char *dirname, const char *date)
{
	int result = 0;
	int altitudes[] = { 3000, 5000 };
	char dd[7];

	dd[0] = date[2];
	dd[1] = date[3];
	dd[2] = date[5];
	dd[3] = date[6];
	dd[4] = date[8];
	dd[5] = date[9];
	dd[6] = '\0';

	int i, j;
	for (j = 0; j < 24; j++) {
		for (i = 0; i < 2; i++) {
			/* nb_cosde_ome_alpen_lv_003000_vt_141104_1200.png  */
			char filename[FILENAME_MAX];
			sprintf(filename, "%s/nb_cosde_ome_alpen_lv_%06d_vt_%s_%02d00.png", dirname, altitudes[i], dd, j);

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

static void free_images(unsigned *images[24][2])
{
	int i, j;
	for (j = 0; j < 24; j++) {
		for (i = 0; i < 2; i++) {
			free(images[j][i]);
		}
	}
}

double mean(kvec_t(struct fix) *b_fixes, size_t pos, size_t interval) {
	int i;
	double dh = 0;
	for (i = pos; i > pos - interval; i--) {
		struct fix lf = kv_A(*b_fixes, i - 1);
		struct fix bf = kv_A(*b_fixes, i);
		struct fix df;
		fix_delta(&df, &bf, &lf);
		dh += df.alt / (df.time * 3600);
	}
	return dh / interval;
}
	
void doit(const char *dirname, const char *date)
{
	int error;
	unsigned *images[24][2];
	unsigned width, height;

	error = load_images(images, &width, &height, dirname, date);
	if (error)
		exit(1);


	char line[80];
	double sxy = 0;
	double sx = 0;
	double sy = 0;
	double sxx = 0;
	double syy = 0;
	unsigned nr = 0;

	kvec_t(struct fix) b_fixes;
	kv_init(b_fixes);
	while (fgets(line, 80, stdin)) {
		if (line[0] == 'B') {
			struct fix bf;
			b_record_to_fix(line, &bf);
			kv_push(struct fix, b_fixes, bf);
		}
	}
	
	assert(kv_size(b_fixes) > MEAN_INTERVAL);

    int i;
	for (i = 1; i < kv_size(b_fixes); i++) {
		struct fix bf = kv_A(b_fixes, i);
		struct fix lf = kv_A(b_fixes, i - 1);
		size_t alt;
		if (bf.alt >= 4000) {
			alt = 1;
		} else if (bf.alt >= 3000) {
			alt = 0;
		} else {
			alt = 2;
		}
		if (alt != 2) {
			double dh;
			double de;
			double du;

			dh = mean(&b_fixes, i, MEAN_INTERVAL);

			int x, y;
			x = lat_rad(bf.lat) * sin(lon_rad(bf.lon)) + X_LON10;
			y = lat_rad(bf.lat) * cos(lon_rad(bf.lon)) + Y_LAT0;

			unsigned int *p;
			p = images[(int) bf.time][alt] + (y * width + x);
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
			sxy += dh * du;
			sx += dh;
			sy += du;
			sxx += dh * dh;
			syy += du * du;
			nr++;
		}
	}
	double r = (nr * sxy - sx * sy) / sqrt((nr * sxx - sx * sx) * (nr * syy - sy * sy));
	printf("# %f %d %f\n", r, nr, pvalue(nr, r));

	free_images(images);
}

int main(int argc, char *argv[])
{
	assert(argc == 3);

	doit(argv[1], argv[2]);

	return 0;
}

/* vi:ai:sw=4:ts=4:noet
*/
