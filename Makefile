%_utest: %.c
	$(CC) -DCU $(CFLAGS) $(LDFLAGS) -o $@ $^
	./$@

CFLAGS = -std=c99 -g -O0 -Wdeprecated-declarations
LDFLAGS = -lm

remap: remap.o transform.o igcrecords.o lodepng.o statistics.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

lodepng.o: lodepng/lodepng.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

utest: $(addsuffix _utest, $(basename transform.c)) $(addsuffix _utest, $(basename igcrecords.c))
