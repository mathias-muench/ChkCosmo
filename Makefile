CFLAGS = -ansi -g

color_updown: color_updown.o lodepng.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm *.o
