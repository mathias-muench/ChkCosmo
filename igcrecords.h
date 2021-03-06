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
double fix_distance(const struct fix *a, const struct fix *b);
double fix_e_kin(const struct fix *a, const struct fix *b);
int fix_hh(const struct fix *this);
int fix_mm(const struct fix *this);
int fix_ss(const struct fix *this);

#endif
