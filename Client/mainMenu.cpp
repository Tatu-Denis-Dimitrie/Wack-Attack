#include "mainMenu.h"
#include "statsMenu.h"
#include "player.h"
#include "audio.h"

bool MainMenu::isActive = true;

MainMenu::MainMenu(const std::string& user, SDL_Renderer* rendererSdl) : rendererSdl(rendererSdl), username(user), pos(0), startButtonTexture(nullptr), startButtonHoverTexture(nullptr),
exitButtonHoverTexture(nullptr), exitButtonTexture(nullptr), backgroundTexture(nullptr), statsButtonTexture(nullptr), statsButtonHoverTexture(nullptr)
{
	Game::isRunning = false;
}

MainMenu::~MainMenu()
{
}

void MainMenu::init() {
	if (!initialized) {
		startButtonTexture = std::unique_ptr<SDL_Texture, SDLDeleter>(loadTexture("assets/PlayButton.png"));
		startButtonHoverTexture = std::unique_ptr<SDL_Texture, SDLDeleter>(loadTexture("assets/PlayButton2.png"));

		statsButtonTexture = std::unique_ptr<SDL_Texture, SDLDeleter>(loadTexture("assets/StatsButton.png"));
		statsButtonHoverTexture = std::unique_ptr<SDL_Texture, SDLDeleter>(loadTexture("assets/StatsButton2.png"));

		exitButtonTexture = std::unique_ptr<SDL_Texture, SDLDeleter>(loadTexture("assets/ExitButton.png"));
		exitButtonHoverTexture = std::unique_ptr<SDL_Texture, SDLDeleter>(loadTexture("assets/ExitButton2.png"));

		backgroundTexture = std::unique_ptr<SDL_Texture, SDLDeleter>(loadTexture("assets/background.png"));

		if (!backgroundTexture) {
			mycerr::display("Failed to load background texture!", mycerr::Color::Red);
		}
		if (!startButtonTexture || !startButtonHoverTexture) {
			mycerr::display("Failed to load start button textures!", mycerr::Color::Red);
		}
		if (!statsButtonTexture || !statsButtonHoverTexture) {
			mycerr::display("Failed to load stats button textures!", mycerr::Color::Red);
		}
		if (!exitButtonTexture || !exitButtonHoverTexture) {
			mycerr::display("Failed to load exit button textures!", mycerr::Color::Red);
		}

		int windowWidth, windowHeight;
		SDL_GetRendererOutputSize(rendererSdl, &windowWidth, &windowHeight);

		int buttonWidth = 125;
		int buttonHeight = 50;

		startButtonRect = { (windowWidth - buttonWidth) / 2, (windowHeight - buttonHeight) / 2 - 40, buttonWidth, buttonHeight };
		statsButtonRect = { (windowWidth - buttonWidth) / 2, (windowHeight - buttonHeight) / 2 + 30, buttonWidth, buttonHeight };
		exitButtonRect = { (windowWidth - buttonWidth) / 2, (windowHeight - buttonHeight) / 2 + 100, buttonWidth, buttonHeight };

		initialized = true;
	}
	audioMM.Init();
}

void MainMenu::logout() {
	cpr::Post(cpr::Url{ SERVER_URL + "/logout" },
		cpr::Body{ crow::json::wvalue{{"username", username}}.dump() });
}

SDL_Texture* MainMenu::loadTexture(const char* filePath) const
{
	SDL_Surface* surface = IMG_Load(filePath);
	if (!surface) {
		mycerr::display("Failed to load texture!", mycerr::Color::Red);
		return nullptr;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(rendererSdl, surface);
	if (!texture) {
		mycerr::display("Failed to create texture from surface!", mycerr::Color::Red);
	}

	SDL_FreeSurface(surface);
	return texture;
}
void MainMenu::handleEvent()
{
	init();
	SDL_Event event;
	SDL_PollEvent(&event);

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	switch (event.type) {
	case SDL_QUIT:
		logout();
		audioMM.PlaySoundEffect("assets/duckPlay.mp3");
		mycout::display("Quit event detected. Exiting Main Menu...", mycout::Color::Red);
		MainMenu::isActive = false;
		Game::isRunning = false;
		break;

	case SDL_MOUSEMOTION:
		if (mouseX >= startButtonRect.x && mouseX <= startButtonRect.x + startButtonRect.w &&
			mouseY >= startButtonRect.y && mouseY <= startButtonRect.y + startButtonRect.h) {
			if (pos != 0) {
				mycout::display("Mouse hovered over Start Button.", mycout::Color::Green);
			}
			pos = 0;
		}
		else if (mouseX >= exitButtonRect.x && mouseX <= exitButtonRect.x + exitButtonRect.w &&
			mouseY >= exitButtonRect.y && mouseY <= exitButtonRect.y + exitButtonRect.h) {
			if (pos != 1) {
				mycout::display("Mouse hovered over Exit Button.", mycout::Color::Green);
			}
			pos = 1;
		}
		else if (mouseX >= statsButtonRect.x && mouseX <= statsButtonRect.x + statsButtonRect.w &&
			mouseY >= statsButtonRect.y && mouseY <= statsButtonRect.y + statsButtonRect.h) {
			if (pos != 2) {
				mycout::display("Mouse hovered over Stats Button.", mycout::Color::Green);
			}
			pos = 2;
		}
		else
		{
			pos = -1;
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT) {
			if (pos == 0) {
				audioMM.PlaySoundEffect("assets/duckPlay.mp3");
				mycout::display("Start Button clicked. Starting the game...", mycout::Color::Yellow);
				notifyPlayPressed();
			}
			if (pos == 1) {
				audioMM.PlaySoundEffect("assets/duckPress.mp3");
				mycout::display("Exit Button clicked. Exiting the game...", mycout::Color::Yellow);
				MainMenu::isActive = false;
				Game::isRunning = false;
				logout();
			}
			if (pos == 2) {
				audioMM.PlaySoundEffect("assets/duckPress.mp3");
				mycout::display("Stats Button clicked. Navigating to stats page...", mycout::Color::Yellow);
				MainMenu::isActive = false;
				StatsMenu::isActiveStats = true;
			}
		}
		break;

	default:
		break;
	}
}

void MainMenu::render() {
	SDL_SetRenderDrawColor(rendererSdl, 0, 0, 0, 255);
	SDL_RenderClear(rendererSdl);

	if (backgroundTexture) {
		int windowWidth, windowHeight;
		SDL_GetRendererOutputSize(rendererSdl, &windowWidth, &windowHeight);
		SDL_Rect bgRect = { 0, 0, windowWidth, windowHeight };
		SDL_RenderCopy(rendererSdl, backgroundTexture.get(), nullptr, &bgRect);
	}
	if (pos == 0) {
		SDL_RenderCopy(rendererSdl, startButtonHoverTexture.get(), nullptr, &startButtonRect);
	}
	else {
		SDL_RenderCopy(rendererSdl, startButtonTexture.get(), nullptr, &startButtonRect);
	}

	if (pos == 1) {
		SDL_RenderCopy(rendererSdl, exitButtonHoverTexture.get(), nullptr, &exitButtonRect);
	}
	else {
		SDL_RenderCopy(rendererSdl, exitButtonTexture.get(), nullptr, &exitButtonRect);
	}
	if (pos == 2) {
		SDL_RenderCopy(rendererSdl, statsButtonHoverTexture.get(), nullptr, &statsButtonRect);
	}
	else {
		SDL_RenderCopy(rendererSdl, statsButtonTexture.get(), nullptr, &statsButtonRect);
	}

	SDL_RenderPresent(rendererSdl);
}

void MainMenu::notifyPlayPressed() {
	if (!username.empty()) {
		auto response = cpr::Post(
			cpr::Url{ SERVER_URL + "/update-player-status" },
			cpr::Body{ crow::json::wvalue{
				{"username", username},
				{"ready", true}
			}.dump() },
			cpr::Header{ {"Content-Type", "application/json"} }
		);

		if (response.status_code == 200) {
			auto json_response = crow::json::load(response.text);

			if (!json_response) {
				std::cerr << "Invalid server response." << std::endl;
				return;
			}

			bool canStartGame = json_response["canStartGame"].b();
			bool canStartGameV2 = json_response["canStartGameV2"].b();

			if (canStartGame && canStartGameV2) {
				mycout::display("Enough players are ready! Starting the game...", mycout::Color::Green);
				MainMenu::isActive = false;
				Game::isRunning = true;
			}
			else if (canStartGame == false) {
				mycout::display("Waiting for more players to be ready...", mycout::Color::Red);
			}
			else if (canStartGameV2 == false)
			{
				mycout::display("We're sorry but the game is full for now! Wait for the game to END and try again!Thank you and have fun !", mycout::Color::Red);
			}
		}
		else {
			mycerr::display("Failed to update player status!", mycerr::Color::Red);
		}
	}
	else {
		mycerr::display("Username is empty. Cannot update status.", mycerr::Color::Red);
	}
}