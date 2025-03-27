#ifndef MAINMENU_H
#define MAINMENU_H

#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include "network_client.h"
#include "statsMenu.h"
#include "audio.h"
import cout;

struct SDLDeleter {
	void operator()(SDL_Texture* texture) const {
		if (texture) {
			SDL_DestroyTexture(texture);
		}
	}
};

class MainMenu {
public:
	static bool isActive;
	int pos;
	bool initialized;

	MainMenu(const std::string& user, SDL_Renderer* rendererSdl);
	~MainMenu();

	void notifyPlayPressed();

	void init();
	void handleEvent();
	void render();
	void logout();

private:
	std::unique_ptr<SDL_Texture, SDLDeleter> startButtonTexture;
	std::unique_ptr<SDL_Texture, SDLDeleter> startButtonHoverTexture;
	std::unique_ptr<SDL_Texture, SDLDeleter> statsButtonTexture;
	std::unique_ptr<SDL_Texture, SDLDeleter> statsButtonHoverTexture;
	std::unique_ptr<SDL_Texture, SDLDeleter> exitButtonTexture;
	std::unique_ptr<SDL_Texture, SDLDeleter> exitButtonHoverTexture;
	std::unique_ptr<SDL_Texture, SDLDeleter> backgroundTexture;

	SDL_Rect startButtonRect;
	SDL_Rect statsButtonRect;
	SDL_Rect exitButtonRect;

	std::string username;
	SDL_Texture* loadTexture(const char* filePath) const;

	Audio audioMM;
	SDL_Renderer* rendererSdl;
};

#endif