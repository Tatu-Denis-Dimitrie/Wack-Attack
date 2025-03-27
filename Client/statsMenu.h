#ifndef STATSMENU_H
#define STATSMENU_H

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "game.h"
#include "network_client.h"
#include "mainMenu.h"
#include <memory>
#include "audio.h"
#include <iomanip>
import cout;

struct SDLDeleterSM {
	void operator()(SDL_Texture* texture) const {
		if (texture) {
			SDL_DestroyTexture(texture);
		}
	}
	void operator()(TTF_Font* font) const {
		if (font) {
			TTF_CloseFont(font);
		}
	}
};

class StatsMenu {
public:
	static bool isActiveStats;
	int selectedButton;
	bool initialized;

	StatsMenu(const std::string& user, SDL_Renderer* rendererSdl);
	~StatsMenu();

	void init();
	void handleEvent();
	void render();
	void clean();

	cpr::Response callUpgradeAPI(const std::string& username);
	cpr::Response callDowngradeAPI(const std::string& username);
	cpr::Response callUpgradeAPI2(const std::string& username);
	cpr::Response callDowngradeAPI2(const std::string& username);

	void updateScoreText();

	void logout();

private:
	std::string username;

	void handleMinusButton();
	void handlePlusButton();
	void handleMinusButton2();
	void handlePlusButton2();

	int points;
	int winningPoints;
	int bulletSpeed;
	int bulletCooldown;

	bool minusButtonHovered = false;
	bool plusButtonHovered = false;

	std::unique_ptr<TTF_Font, SDLDeleterSM> scoreFont;
	std::unique_ptr<TTF_Font, SDLDeleterSM> scoreFontSmaller;

	std::unique_ptr<SDL_Texture, SDLDeleterSM> pointsTexture;
	SDL_Rect scoreRect;

	std::unique_ptr<SDL_Texture, SDLDeleterSM> winningPointsTexture;
	SDL_Rect scoreWinRect;

	std::unique_ptr<SDL_Texture, SDLDeleterSM> bulletSpeedTexture;
	SDL_Rect bulletSpeedRect;

	std::unique_ptr<SDL_Texture, SDLDeleterSM> bulletCooldownTexture;
	SDL_Rect reloadSpeedRect;

	std::unique_ptr<SDL_Texture, SDLDeleterSM> minusButtonTexture;
	std::unique_ptr<SDL_Texture, SDLDeleterSM> minusButtonHoverTexture;
	std::unique_ptr<SDL_Texture, SDLDeleterSM> plusButtonTexture;
	std::unique_ptr<SDL_Texture, SDLDeleterSM> plusButtonHoverTexture;

	std::unique_ptr<SDL_Texture, SDLDeleterSM> returnButtonTexture;
	std::unique_ptr<SDL_Texture, SDLDeleterSM> returnButtonHoverTexture;

	std::unique_ptr<SDL_Texture, SDLDeleterSM> backgroundTexture;

	SDL_Rect returnButtonRect;

	SDL_Rect minusButtonRect;
	SDL_Rect plusButtonRect;

	SDL_Rect minusButtonRect2;
	SDL_Rect plusButtonRect2;

	SDL_Texture* loadTexture(const char* filePath);

	Audio audioSM;
	SDL_Renderer* rendererSdl;
};

#endif
