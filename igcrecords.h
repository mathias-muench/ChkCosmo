#ifndef IGCRECORDS_H
#define IGCRECORDS_H

struct fix {
	double time;
	double lat;
	double lon;
	double alt;
};

void b_record_to_fix(const char *record, struct fix *fix);
void fix_delta(struct fix *r, const struct fix *a, const struct fix *b);

#endif
