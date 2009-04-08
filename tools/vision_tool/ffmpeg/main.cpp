#include <stdlib.h>
#include "SDL.h"
extern "C" {
#include <libavformat/avformat.h>
}
#include "movie.h"

int main(int argc, char *argv[]) {
	SDL_Surface *screen;

	SDL_Init(SDL_INIT_VIDEO| SDL_INIT_TIMER);

	screen = SDL_SetVideoMode(640, 480, 24, SDL_SWSURFACE);

	av_register_all();

	Movie *movie = new Movie(argv[1]);
	SDL_Event event;
	bool done = false;
	while (!done && SDL_WaitEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			done = true;
			break;
		case SDL_MOUSEBUTTONDOWN:
			movie->readFrame();
			SDL_LockSurface(screen);
			memcpy(screen->pixels, movie->getPFrameRGB()->data[0],
					movie->getNumBytes());
			SDL_UnlockSurface(screen);
			SDL_UpdateRect(screen, 0, 0, movie->getWidth(), movie->getHeight());
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LEFT) {
				movie->seekBy(-5);
			}
			if (event.key.keysym.sym == SDLK_RIGHT) {
				movie->seekBy(5);
			}
			movie->readFrame();
			SDL_LockSurface(screen);
			memcpy(screen->pixels, movie->getPFrameRGB()->data[0],
					movie->getNumBytes());
			SDL_UnlockSurface(screen);
			SDL_UpdateRect(screen, 0, 0, movie->getWidth(), movie->getHeight());
			break;
		}
	}
	delete movie;

	SDL_Quit();
}
