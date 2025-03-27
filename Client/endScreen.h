#pragma once
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <memory>
#include "audio.h"
#include "game.h"
#include "mainMenu.h"

struct SDLDeleterES {
	void operator()(SDL_Texture* texture) const {
		SDL_DestroyTexture(texture);
	}
	void operator()(TTF_Font* font) const {
		TTF_CloseFont(font);
	}
};

class EndScreen {
public:
	EndScreen(SDL_Renderer* rendererSdl);
	~EndScreen();

	void init();
	void handleEvents();
	void render();
	void clean();
	static bool isActiveEND;

private:
	std::unique_ptr<SDL_Texture, SDLDeleterES> loadTexture(const char* filePath);

	bool initialized = false;
	bool buttonHovered = false;

	SDL_Rect exitButtonRect;

	std::unique_ptr<SDL_Texture, SDLDeleterES> backgroundTexture;
	std::unique_ptr<SDL_Texture, SDLDeleterES> exitButtonTexture;
	std::unique_ptr<SDL_Texture, SDLDeleterES> exitButtonHoverTexture;

	Audio audioES;
	SDL_Renderer* rendererSdl;
};