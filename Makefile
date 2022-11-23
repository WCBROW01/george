CFLAGS = -Wall -Wextra -O2 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_mixer

george: main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
