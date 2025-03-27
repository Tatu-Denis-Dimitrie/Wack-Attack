#pragma once
#include <iostream>
#include <string>
#include <cpr/cpr.h>
#include <SDL_ttf.h>
#include <SDL.h>
#include <memory>
#include <regex>
#include "game.h"
#include "audio.h"

struct SDLDeleterLS {
	void operator()(SDL_Texture* texture) const {
		SDL_DestroyTexture(texture);
	}
	void operator()(TTF_Font* font) const {
		TTF_CloseFont(font);
	}
};

class LoginScreen {
public:
	LoginScreen(SDL_Renderer* rendererSdl);
	~LoginScreen();

	void init();
	void handleEvents();
	void render();
	void clean();
	bool isActive;
	bool isLoggedIn;

	std::string getUsername() const;

private:
	std::unique_ptr<SDL_Texture, SDLDeleterLS> loadTexture(const char* filePath);
	void updateInputTextTexture();
	void submitLogin();

	std::string username;
	bool initialized = false;
	bool buttonHovered = false;

	SDL_Rect inputBoxRect;
	SDL_Rect submitButtonRect;

	std::regex validUsernameRegex = std::regex("[a-zA-Z0-9]");
	std::string inputText;
	SDL_Color textColor = { 0, 0, 0 };

	std::unique_ptr<SDL_Texture, SDLDeleterLS> backgroundTexture;
	std::unique_ptr<SDL_Texture, SDLDeleterLS> submitButtonTexture;
	std::unique_ptr<SDL_Texture, SDLDeleterLS> submitButtonHoverTexture;
	std::unique_ptr<SDL_Texture, SDLDeleterLS> inputTextTexture;

	std::unique_ptr<TTF_Font, SDLDeleterLS> font;

	bool userNameLocked;

	Audio audioLS;
	SDL_Renderer* rendererSdl;
};