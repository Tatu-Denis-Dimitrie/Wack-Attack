#pragma once
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>
#include <cpr/cpr.h>
#include <SDL.h>
#include <SDL_image.h>d
#include <SDL_ttf.h>
#include <crow/json.h>
#include "loginScreen.h"
#include "mainMenu.h"
#include "statsMenu.h"
#include "endScreen.h"
#include "network_client.h"
#include "audio.h"
#include "player.h"
#include "map.h"
import cout;
import timer;

class Game {
public:
	Game();
	~Game();

	void initializer(const char* title, int x, int y, int width, int height, bool fullscreen);
	void run();
	void updater();
	void renderer();
	void handleEvents();
	void clean();

	void initializeObjects();
	void cleanObjects();
	void CheckEndGameCondition();
	void sendWinPoints(const std::string& username);
	std::string getRandomSkin();

	bool running();

	inline static bool isRunning = false;

	void sendLogoutRequest();
	void fetchAndUpdatePlayers();
	void showAllPlayers();

	void CheckBulletCollision();
	void sendUpdatePoints(const std::string& username);
	bool CheckPlayerCollision();

	void startGameTimer();
	void stopGameTimer();

	void SendMapUpdateRequest(int row, int col, const std::string& tileType);
	void CheckForMapUpdates();
private:
	SDL_Renderer* rendererSdl;
	SDL_Event event;
	std::unordered_map<std::string, Player*> players;
	std::string username;
	Player* player;
	Map* map;
	double cdIndex, spIndex;
	Uint32 lastPlayerUpdateTime = 0;

	SDL_Window* window;
	Audio audio;
	Timer timer;
};
