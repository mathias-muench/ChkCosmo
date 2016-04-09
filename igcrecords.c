#include <stdio.h>

#include "igcrecords.h"

struct hms {
	int hh;
	int mm;
	int ss;
};

/*B0905444743497N01226360EA008410096300308971*/

static double hms_to_double(const struct hms *hms) {
	return hms->hh + hms->mm / 60.0 + hms->ss / 3600.0;
}

void b_record_to_fix(const char *record, struct fix *fix) {
	struct hms bt_s, *bt = &bt_s;
	int bg, bm, lg, lm;
	sscanf(record, "B%2d%2d%2d%2d%5d%*c%3d%5d%*2c%5lf", &bt->hh, &bt->mm, &bt->ss, &bg, &bm, &lg, &lm, &fix->alt);
	fix->lat = bg + bm / 60000.0;
	fix->lon = lg + lm / 60000.0;
	fix->time = hms_to_double(bt);
}

void fix_delta(struct fix *r, const struct fix *a, const struct fix *b) {
	r->time = a->time - b->time;
	r->alt = a->alt - b->alt;
}

#ifdef CU
#include "cu.h"

int parse_time() {
	struct fix fix_s, *fix = &fix_s;

	b_record_to_fix("B0905444743497N01226360EA008410096300308971", fix);
	return fix->time == 9 + 5 / 60.0 + 44 / 3600.0;
}

int parse_alt() {
	struct fix fix_s, *fix = &fix_s;

	b_record_to_fix("B0905444743497N01226360EA008410096300308971", fix);
	return fix->alt == 841;
}

int parse_lat() {
	struct fix fix_s, *fix = &fix_s;

	b_record_to_fix("B0905444743497N01226360EA008410096300308971", fix);
	return fix->lat == 47 + 43497 / 60000.0;
}

int parse_lon() {
	struct fix fix_s, *fix = &fix_s;

	b_record_to_fix("B0905444743497N01226360EA008410096300308971", fix);
	return fix->lon == 12 + 26360 / 60000.0;
}

int main(char **v, int c)
{
	cu_test(parse_time);
	cu_test(parse_alt);
	cu_test(parse_lon);
	cu_test(parse_lat);
	return(!cu_result());
}
#endif

/* vi:ai:ts=4:sw=4:noet
*/
