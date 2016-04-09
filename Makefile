CFLAGS = -ansi -g -Wdeprecated-declarations
LDFLAGS = -lm

remap: remap.o lodepng.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

color_updown: color_updown.o lodepng.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm *.o
