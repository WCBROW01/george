#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#define WINDOW_W 800
#define WINDOW_H 600

#define RECT_W 96.0f
#define RECT_H 64.0f
#define VELOCITY 128.0f

// Unpack color from Uint32 (no alpha)
#define UNPACK_UINT32_COLOR(color) (color) >> 16 & 0xFF, (color) >> 8 & 0xFF, (color) & 0xFF, 255

static const char *HELP_TEXT =
	"Usage: %s -i [IMAGE_FILE] -a [WAV_FILE]\n"
	"\t-h, --help \tShow this help text.\n"
	"\t-i, --image\tLoad an image to overlay on top of the rectangle.\n"
	"\t-a, --audio\tLoad a wav file to play when the rectangle bounces.\n";

int main(int, char *argv[]) {
	char *image_path = NULL;
	char *audio_path = NULL;
	for (char **arg = argv; *arg; ++arg) {
		if (!strcmp(*arg, "-h") || !strcmp(*arg, "--help")) {
			printf(HELP_TEXT, argv[0]);
			return 0;
		} else if (!strcmp(*arg, "-i") || !strcmp(*arg, "--image")) {
			image_path = *++arg;
		} else if (!strcmp(*arg, "-a") || !strcmp(*arg, "--audio")) {
			audio_path = *++arg;
		}
	}


	srand(time(NULL));

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(
		"george", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_W, WINDOW_H, 0
	);

	if (!window) {
		fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
		return 1;
	}

	if (Mix_OpenAudio(32000, MIX_DEFAULT_FORMAT, 2, 4096)) {
		fprintf(stderr, "Mix_OpenAudio: %s\n", SDL_GetError());
		return 1;
	}

	Mix_Chunk *sound = NULL;
	if (audio_path) sound = Mix_LoadWAV(audio_path);

	SDL_Texture *texture = NULL;
	if (image_path) {
		SDL_Surface *surface = IMG_Load(image_path);
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
	}

	float ratio = 1.0f;
	int w, h;
	if (texture) {
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);
		ratio = (float) w / h;
	}

	SDL_FRect r = {
		.x = rand() % WINDOW_W,
		.y = rand() % WINDOW_H,
		.w = image_path ? RECT_H * ratio : RECT_W,
		.h = RECT_H
	};

	// bounds check the random position
	if (r.x > WINDOW_W - r.w) r.x = WINDOW_W - r.w;
	if (r.y > WINDOW_H - r.h) r.y = WINDOW_H - r.h;

	float dx = VELOCITY;
	float dy = VELOCITY;
	Uint32 color = rand();

	bool quit = false;
	Uint64 last_ticks = SDL_GetTicks64();
	while (!quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT: {
					quit = true;
				} break;
			}
		}

		Uint64 current_ticks = SDL_GetTicks64();
		Uint64 theta = current_ticks - last_ticks;
		last_ticks = current_ticks;

		r.x += dx * theta / 1000.0f;
		r.y += dy * theta / 1000.0f;

		if (r.x <= 0 || r.x + r.w >= WINDOW_W) {
			color = rand();
			dx = -dx;
			if (sound) Mix_PlayChannel(-1, sound, 0);
		}
		if (r.y <= 0 || r.y + r.h >= WINDOW_H) {
			color = rand();
			dy = -dy;
			if (sound) Mix_PlayChannel(-1, sound, 0);
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, UNPACK_UINT32_COLOR(color));
		SDL_RenderFillRectF(renderer, &r);
		if (texture) SDL_RenderCopyF(renderer, texture, NULL, &r);
		SDL_RenderPresent(renderer);
	}

	if (sound) Mix_FreeChunk(sound);
	Mix_CloseAudio();
	if (texture) SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
