CFLAGS = -ansi -g -O0 -Wdeprecated-declarations
LDFLAGS = -lm

remap: remap.o lodepng/lodepng.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

color_updown: color_updown.o lodepng/lodepng.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm *.o
