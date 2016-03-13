CFLAGS = -ansi -g -Wdeprecated-declarations
LDFLAGS = -lm

color_updown: color_updown.o lodepng.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

remap: remap.o lodepng.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm *.o
