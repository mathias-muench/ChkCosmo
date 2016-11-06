%_utest: %.c
	$(CC) -DCU $(CFLAGS) $(LDFLAGS) -o $@ $^
	./$@

CFLAGS = -ansi -g -O0 -Wdeprecated-declarations
LDFLAGS = -lm

remap: remap.o transform.o igcrecords.o lodepng/lodepng.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

color_updown: color_updown.o lodepng/lodepng.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

utest: $(addsuffix _utest, $(basename transform.c)) $(addsuffix _utest, $(basename igcrecords.c))
