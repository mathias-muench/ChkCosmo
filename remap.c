#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "lodepng/lodepng.h"
#include "klib/kvec.h"
#include "igcrecords.h"
#include "transform.h"
#include "statistics.h"

#define MEAN_INTERVAL 120

typedef kvec_t(struct fix) fix_kvec;
typedef kvec_t(double) double_kvec;

struct interval {
	int startPos;
	int endPos;
};
typedef kvec_t(struct interval) interval_kvec;

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

size_t time_interval(fix_kvec *b_fixes, size_t pos, unsigned dt) {
	int i;
	struct fix df = { 0 };
	for (i = pos; df.time < dt; i--) {
		struct fix ff = kv_A(*b_fixes, pos);
		struct fix bf = kv_A(*b_fixes, i);
		fix_delta(&df, &ff, &bf);
	}
	return pos - i;
}

double mean(fix_kvec *b_fixes, size_t pos, size_t interval) {
	int i;
	double dh = 0;
	for (i = pos; i > pos - interval; i--) {
		struct fix lf = kv_A(*b_fixes, i - 1);
		struct fix bf = kv_A(*b_fixes, i);
		struct fix df;
		fix_delta(&df, &bf, &lf);
		dh += df.alt / df.time;
	}
	return dh / interval;
}

double mean_fc(double_kvec *forecasts, size_t pos, size_t interval) {
	int i;
	double du = 0;
	for (i = pos; i > pos - interval; i--) {
		du += kv_A(*forecasts, i);
	}
	return du / interval;
}

interval_kvec mark_climb(interval_kvec climb_zones, fix_kvec fixes) {
	int i;
	int startPos = 0;
	int endPos = 0;
	for (i = 1; i < kv_size(fixes); i++) {
		size_t interval = time_interval(&fixes, i, MEAN_INTERVAL);
		double dh = mean(&fixes, i, interval);
		if (dh > 0.5 && startPos == 0 && endPos == 0) {
			startPos = i - interval;
		}
		if (dh < 0.0 && startPos != 0 && endPos == 0) {
			endPos = i - interval;
		}

		if (startPos && endPos) {
			if (startPos != endPos) {
				struct interval zone;
				zone.startPos = startPos;
				zone.endPos = endPos;
				kv_push(struct interval, climb_zones, zone);
			}
			startPos = endPos = 0;
		}
	}
	return climb_zones;
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

	fix_kvec b_fixes;
	kv_init(b_fixes);
	double_kvec forecasts;
	kv_init(forecasts);
	while (fgets(line, 80, stdin)) {
		if (line[0] == 'B') {
			struct fix bf;
			b_record_to_fix(line, &bf);
			int x = fix2x(bf.lat, bf.lon);
			int y = fix2y(bf.lat, bf.lon);
			size_t alt;
			if (bf.alt >= 4000) {
				alt = 1;
			} else {
				alt = 0;
			}
			unsigned int *p = images[fix_hh(&bf)][alt] + (y * width + x);
			double du = forecast(*p);
			if (du != 99) {
				kv_push(double, forecasts, du);
				kv_push(struct fix, b_fixes, bf);
			}
		}
	}

    interval_kvec climb_zones; 
	kv_init(climb_zones);
    climb_zones = mark_climb(climb_zones, b_fixes);
	for (int i = 0; i < kv_size(climb_zones); i++) {
		struct fix bs = kv_A(b_fixes, kv_A(climb_zones, i).startPos);
		struct fix be = kv_A(b_fixes, kv_A(climb_zones, i).endPos);
			if (be.alt >= 3000)
		printf("# %02d%02d%02d %02d%02d%02d\n", fix_hh(&bs),fix_mm(&bs),  fix_ss(&bs), fix_hh(&be),fix_mm(&be),  fix_ss(&be));
	}

	assert(kv_size(b_fixes) > MEAN_INTERVAL);

	int i;
	int startPos = 0;
	int endPos = 0;
	for (i = 1; i < kv_size(b_fixes); i++) {
		size_t interval = time_interval(&b_fixes, i, MEAN_INTERVAL);
		double dh = mean(&b_fixes, i, interval);
		if (dh > 0.5 && startPos == 0 && endPos == 0) {
			startPos = i - interval;
		}
		if (dh < 0.0 && startPos != 0 && endPos == 0) {
			endPos = i - interval;
		}

		if (startPos && endPos) {
			struct fix bs = kv_A(b_fixes, startPos);
			struct fix be = kv_A(b_fixes, endPos);
			if (startPos != endPos && be.alt >= 3000) {
				double dh = mean(&b_fixes, endPos, endPos - startPos);
				double du = mean_fc(&forecasts, endPos, endPos - startPos);

				printf("%6.2f\t%6.2f\t# %02d%02d%02d %02d%02d%02d\n", dh, du, fix_hh(&bs),fix_mm(&bs),  fix_ss(&bs), fix_hh(&be),fix_mm(&be),  fix_ss(&be));


				sxy += dh * du;
				sx += dh;
				sy += du;
				sxx += dh * dh;
				syy += du * du;
				nr++;
			}
			startPos = endPos = 0;
		}
	}
	double r = (nr * sxy - sx * sy) / sqrt((nr * sxx - sx * sx) * (nr * syy - sy * sy));
	printf("# %f %d %f\n", r, nr, pvalue(nr, r));

	kv_destroy(b_fixes);
	kv_destroy(forecasts);
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
