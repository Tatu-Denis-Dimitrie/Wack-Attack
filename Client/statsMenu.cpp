#include "statsMenu.h"
#include <future>
#include <chrono>

bool StatsMenu::isActiveStats = false;

StatsMenu::StatsMenu(const std::string& user, SDL_Renderer* rendererSdl)
	: rendererSdl(rendererSdl), username(user), selectedButton(-1), initialized(false),
	backgroundTexture(nullptr),
	minusButtonTexture(nullptr), minusButtonHoverTexture(nullptr),
	plusButtonTexture(nullptr), plusButtonHoverTexture(nullptr),
	returnButtonHoverTexture(nullptr), returnButtonTexture(nullptr)
{
	init();
}

StatsMenu::~StatsMenu() {
	clean();
}

void StatsMenu::init() {
	if (!initialized) {
		minusButtonTexture = std::unique_ptr<SDL_Texture, SDLDeleterSM>(loadTexture("assets/Minus1.png"));
		minusButtonHoverTexture = std::unique_ptr<SDL_Texture, SDLDeleterSM>(loadTexture("assets/Minus2.png"));
		plusButtonTexture = std::unique_ptr<SDL_Texture, SDLDeleterSM>(loadTexture("assets/Plus1.png"));
		plusButtonHoverTexture = std::unique_ptr<SDL_Texture, SDLDeleterSM>(loadTexture("assets/Plus2.png"));
		returnButtonTexture = std::unique_ptr<SDL_Texture, SDLDeleterSM>(loadTexture("assets/Return1.png"));
		returnButtonHoverTexture = std::unique_ptr<SDL_Texture, SDLDeleterSM>(loadTexture("assets/Return2.png"));
		backgroundTexture = std::unique_ptr<SDL_Texture, SDLDeleterSM>(loadTexture("assets/background.png"));

		if (!minusButtonTexture || !minusButtonHoverTexture ||
			!plusButtonTexture || !plusButtonHoverTexture ||
			!returnButtonTexture || !returnButtonHoverTexture) {
			mycerr::display("Failed to load button textures!", mycerr::Color::Red);
		}

		scoreFont = std::unique_ptr<TTF_Font, SDLDeleterSM>(TTF_OpenFont("assets/OpenSans-Regular.ttf", 32));
		scoreFontSmaller = std::unique_ptr<TTF_Font, SDLDeleterSM>(TTF_OpenFont("assets/OpenSans-Regular.ttf", 12));
		if (!scoreFont) {
			mycerr::display("Failed to load font!", mycerr::Color::Red);
		}

		SDL_Color color = { 255, 255, 255, 255 };
		points = 0;
		winningPoints = 0;
		bulletSpeed = 0;
		bulletCooldown = 0;

		try {
			auto scores = fetchUserScoreFromServer(username);
			points = scores.points;
			winningPoints = scores.winningPoints;
			bulletSpeed = scores.bulletSpeed;
			bulletCooldown = scores.bulletCooldown;
		}
		catch (const std::exception& e) {
			mycerr::display("Error fetching scores!", mycerr::Color::Red);
			points = 0;
			winningPoints = 0;
			bulletSpeed = 0;
			bulletCooldown = 0;
		}

		std::string scoreText = username + "'s Points: " + std::to_string(points);
		SDL_Surface* scoreSurface = TTF_RenderText_Solid(scoreFont.get(), scoreText.c_str(), color);

		if (scoreSurface) {
			pointsTexture.reset(SDL_CreateTextureFromSurface(rendererSdl, scoreSurface));
			SDL_FreeSurface(scoreSurface);
		}
		else {
			std::cerr << "Failed to render score text: " << TTF_GetError() << std::endl;
		}

		std::string scoreWinText = username + "'s Winning Points: " + std::to_string(winningPoints);
		SDL_Surface* scoreWinSurface = TTF_RenderText_Solid(scoreFont.get(), scoreWinText.c_str(), color);

		if (scoreWinSurface) {
			winningPointsTexture.reset(SDL_CreateTextureFromSurface(rendererSdl, scoreWinSurface));
			SDL_FreeSurface(scoreWinSurface);
		}
		else {
			std::cerr << "Failed to render winningPoints text: " << TTF_GetError() << std::endl;
		}

		std::string bulletSpeedText = "Bullet Speed: " + std::to_string(bulletSpeed);
		SDL_Surface* bulletSpeedSurface = TTF_RenderText_Solid(scoreFontSmaller.get(), bulletSpeedText.c_str(), color);

		if (bulletSpeedSurface) {
			bulletSpeedTexture.reset(SDL_CreateTextureFromSurface(rendererSdl, bulletSpeedSurface));
			SDL_FreeSurface(bulletSpeedSurface);
		}
		else {
			std::cerr << "Failed to render bulletSpeed text: " << TTF_GetError() << std::endl;
		}

		std::string reloadSpeedText = "Reload Speed: " + std::to_string(bulletCooldown);
		SDL_Surface* reloadSpeedSurface = TTF_RenderText_Solid(scoreFontSmaller.get(), reloadSpeedText.c_str(), color);

		if (reloadSpeedSurface) {
			bulletCooldownTexture.reset(SDL_CreateTextureFromSurface(rendererSdl, reloadSpeedSurface));
			SDL_FreeSurface(reloadSpeedSurface);
		}
		else {
			std::cerr << "Failed to render bulletCooldown text: " << TTF_GetError() << std::endl;
		}

		int windowWidth, windowHeight;
		SDL_GetRendererOutputSize(rendererSdl, &windowWidth, &windowHeight);

		int textWidth, textHeight;
		if (pointsTexture) {
			SDL_QueryTexture(pointsTexture.get(), nullptr, nullptr, &textWidth, &textHeight);
			scoreRect = { (windowWidth - textWidth) / 2, (windowHeight - textHeight) / 2 - 195, textWidth, textHeight };
		}
		else {
			scoreRect = { 50, 50, 200, 50 };
		}

		if (winningPointsTexture) {
			SDL_QueryTexture(winningPointsTexture.get(), nullptr, nullptr, &textWidth, &textHeight);
			scoreWinRect = { (windowWidth - textWidth) / 2, (windowHeight - textHeight) / 2 + 20, textWidth, textHeight };
		}
		else {
			scoreWinRect = { 50, 150, 200, 50 };
		}

		int spaceBetweenButtons = 40;
		int textX = (windowWidth - textWidth) / 2;

		if (bulletSpeedTexture) {
			SDL_QueryTexture(bulletSpeedTexture.get(), nullptr, nullptr, &textWidth, &textHeight);
			bulletSpeedRect = { (windowWidth - textWidth) / 2, (windowHeight - textHeight) / 2 + 150, textWidth, textHeight };
		}

		if (bulletCooldownTexture) {
			SDL_QueryTexture(bulletCooldownTexture.get(), nullptr, nullptr, &textWidth, &textHeight);
			reloadSpeedRect = { (windowWidth - textWidth) / 2, bulletSpeedRect.y + textHeight + spaceBetweenButtons - 300, textWidth, textHeight };
		}

		int buttonWidth = 200;
		int buttonHeight = 50;
		int buttonSize = 60;

		minusButtonRect = { (windowWidth - buttonSize) / 2 - 100, (windowHeight - buttonHeight) / 2 + 140, buttonSize, buttonSize };
		plusButtonRect = { (windowWidth - buttonSize) / 2 + 100, (windowHeight - buttonHeight) / 2 + 140, buttonSize, buttonSize };

		minusButtonRect2 = { (windowWidth - buttonSize) / 2 - 100, minusButtonRect.y - 240, buttonSize, buttonSize };
		plusButtonRect2 = { (windowWidth - buttonSize) / 2 + 100, minusButtonRect.y - 240, buttonSize, buttonSize };

		returnButtonRect = { 10, 10, 50, 50 };

		initialized = true;
		audioSM.Init();
	}
}

void StatsMenu::updateScoreText() {
	SDL_Color color = { 255, 255, 255, 255 };

	std::string scoreText = username + "'s Points: " + std::to_string(points);
	SDL_Surface* scoreSurface = TTF_RenderText_Solid(scoreFont.get(), scoreText.c_str(), color);

	if (scoreSurface) {
		pointsTexture.reset(SDL_CreateTextureFromSurface(rendererSdl, scoreSurface));
		SDL_FreeSurface(scoreSurface);
	}
	else {
		std::cerr << "Failed to render points text: " << TTF_GetError() << std::endl;
	}

	std::string scoreWinText = username + "'s Winning Points: " + std::to_string(winningPoints);
	SDL_Surface* scoreWinSurface = TTF_RenderText_Solid(scoreFont.get(), scoreWinText.c_str(), color);

	if (scoreWinSurface) {
		winningPointsTexture.reset(SDL_CreateTextureFromSurface(rendererSdl, scoreWinSurface));
		SDL_FreeSurface(scoreWinSurface);
	}
	else {
		std::cerr << "Failed to render winningPoints text: " << TTF_GetError() << std::endl;
	}

	std::string bulletSpeedText = "Bullet Speed: " + std::to_string(bulletSpeed);
	SDL_Surface* bulletSpeedSurface = TTF_RenderText_Solid(scoreFontSmaller.get(), bulletSpeedText.c_str(), color);

	if (bulletSpeedSurface) {
		bulletSpeedTexture.reset(SDL_CreateTextureFromSurface(rendererSdl, bulletSpeedSurface));
		SDL_FreeSurface(bulletSpeedSurface);
	}
	else {
		std::cerr << "Failed to render bulletSpeed text: " << TTF_GetError() << std::endl;
	}

	std::string reloadSpeedText = "Bullet Cooldown: " + std::to_string(bulletCooldown);
	SDL_Surface* reloadSpeedSurface = TTF_RenderText_Solid(scoreFontSmaller.get(), reloadSpeedText.c_str(), color);

	if (reloadSpeedSurface) {
		bulletCooldownTexture.reset(SDL_CreateTextureFromSurface(rendererSdl, reloadSpeedSurface));
		SDL_FreeSurface(reloadSpeedSurface);
	}
	else {
		std::cerr << "Failed to render bulletCooldown text: " << TTF_GetError() << std::endl;
	}
}

SDL_Texture* StatsMenu::loadTexture(const char* filePath) {
	SDL_Surface* surface = IMG_Load(filePath);
	if (!surface) {
		std::cerr << "Failed to load texture: " << filePath << std::endl;
		return nullptr;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(rendererSdl, surface);
	SDL_FreeSurface(surface);

	return texture;
}

void StatsMenu::handleEvent() {
	SDL_Event event;
	SDL_PollEvent(&event);

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	switch (event.type) {
	case SDL_QUIT:
		isActiveStats = false;
		logout();
		break;

	case SDL_MOUSEMOTION:
		if (mouseX >= minusButtonRect.x && mouseX <= minusButtonRect.x + minusButtonRect.w &&
			mouseY >= minusButtonRect.y && mouseY <= minusButtonRect.y + minusButtonRect.h) {
			selectedButton = 2;
		}
		else if (mouseX >= plusButtonRect.x && mouseX <= plusButtonRect.x + plusButtonRect.w &&
			mouseY >= plusButtonRect.y && mouseY <= plusButtonRect.y + plusButtonRect.h) {
			selectedButton = 3;
		}
		else if (mouseX >= minusButtonRect2.x && mouseX <= minusButtonRect2.x + minusButtonRect2.w &&
			mouseY >= minusButtonRect2.y && mouseY <= minusButtonRect2.y + minusButtonRect2.h) {
			selectedButton = 4;
		}
		else if (mouseX >= plusButtonRect2.x && mouseX <= plusButtonRect2.x + plusButtonRect2.w &&
			mouseY >= plusButtonRect2.y && mouseY <= plusButtonRect2.y + plusButtonRect2.h) {
			selectedButton = 5;
		}
		else if (mouseX >= returnButtonRect.x && mouseX <= returnButtonRect.x + returnButtonRect.w &&
			mouseY >= returnButtonRect.y && mouseY <= returnButtonRect.y + returnButtonRect.h) {
			selectedButton = 6;
		}
		else {
			selectedButton = -1;
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT) {
			if (selectedButton == 2) {
				handleMinusButton2();
				audioSM.PlaySoundEffect("assets/downGrade.mp3");
			}
			else if (selectedButton == 3) {
				handlePlusButton2();
				audioSM.PlaySoundEffect("assets/upgradeSound.mp3");
			}
			else if (selectedButton == 4) {
				handleMinusButton();
				audioSM.PlaySoundEffect("assets/downGrade.mp3");
			}
			else if (selectedButton == 5) {
				handlePlusButton();
				audioSM.PlaySoundEffect("assets/upgradeSound.mp3");
			}
			else if (selectedButton == 6) {
				audioSM.PlaySoundEffect("assets/duckPlay.mp3");
				isActiveStats = false;
				MainMenu::isActive = true;
			}
		}
		break;

	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			isActiveStats = false;
		}
		break;

	default:
		break;
	}
}

void StatsMenu::render() {
	SDL_SetRenderDrawColor(rendererSdl, 0, 0, 0, 255);
	SDL_RenderClear(rendererSdl);

	if (backgroundTexture) {
		int windowWidth, windowHeight;
		SDL_GetRendererOutputSize(rendererSdl, &windowWidth, &windowHeight);
		SDL_Rect bgRect = { 0, 0, windowWidth, windowHeight };
		SDL_RenderCopy(rendererSdl, backgroundTexture.get(), nullptr, &bgRect);
	}

	if (pointsTexture) {
		SDL_RenderCopy(rendererSdl, pointsTexture.get(), nullptr, &scoreRect);
	}

	if (winningPointsTexture) {
		SDL_RenderCopy(rendererSdl, winningPointsTexture.get(), nullptr, &scoreWinRect);
	}

	if (bulletSpeedTexture) {
		SDL_RenderCopy(rendererSdl, bulletSpeedTexture.get(), nullptr, &bulletSpeedRect);
	}

	if (bulletCooldownTexture) {
		SDL_RenderCopy(rendererSdl, bulletCooldownTexture.get(), nullptr, &reloadSpeedRect);
	}

	if (selectedButton == 2) {
		SDL_RenderCopy(rendererSdl, minusButtonHoverTexture.get(), nullptr, &minusButtonRect);
	}
	else {
		SDL_RenderCopy(rendererSdl, minusButtonTexture.get(), nullptr, &minusButtonRect);
	}

	if (selectedButton == 3) {
		SDL_RenderCopy(rendererSdl, plusButtonHoverTexture.get(), nullptr, &plusButtonRect);
	}
	else {
		SDL_RenderCopy(rendererSdl, plusButtonTexture.get(), nullptr, &plusButtonRect);
	}
	if (selectedButton == 4) {
		SDL_RenderCopy(rendererSdl, minusButtonHoverTexture.get(), nullptr, &minusButtonRect2);
	}
	else {
		SDL_RenderCopy(rendererSdl, minusButtonTexture.get(), nullptr, &minusButtonRect2);
	}

	if (selectedButton == 5) {
		SDL_RenderCopy(rendererSdl, plusButtonHoverTexture.get(), nullptr, &plusButtonRect2);
	}
	else {
		SDL_RenderCopy(rendererSdl, plusButtonTexture.get(), nullptr, &plusButtonRect2);
	}

	if (selectedButton == 6) {
		SDL_RenderCopy(rendererSdl, returnButtonHoverTexture.get(), nullptr, &returnButtonRect);
	}
	else {
		SDL_RenderCopy(rendererSdl, returnButtonTexture.get(), nullptr, &returnButtonRect);
	}

	SDL_RenderPresent(rendererSdl);
}

void StatsMenu::handlePlusButton() {
	mycout::display("Upgrade button pressed!", mycout::Color::Green);

	auto response = callUpgradeAPI(username);

	if (response.status_code == 200) {
		try {
			auto scores = fetchUserScoreFromServer(username);
			points = scores.points;
			winningPoints = scores.winningPoints;
			bulletSpeed = scores.bulletSpeed;
			bulletCooldown = scores.bulletCooldown;

			updateScoreText();
		}
		catch (const std::exception& e) {
			std::cerr << "Error fetching scores: " << e.what() << std::endl;
		}
	}
	else if (response.status_code == 400) {
		mycout::display("Not enough score to upgrade!", mycout::Color::Red);
	}
}

void StatsMenu::handleMinusButton() {
	mycout::display("Downgrade button pressed!", mycout::Color::Green);

	auto response = callDowngradeAPI(username);

	if (response.status_code == 200) {
		try {
			auto scores = fetchUserScoreFromServer(username);
			points = scores.points;
			winningPoints = scores.winningPoints;
			bulletSpeed = scores.bulletSpeed;
			bulletCooldown = scores.bulletCooldown;

			updateScoreText();
		}
		catch (const std::exception& e) {
			std::cerr << "Error fetching scores: " << e.what() << std::endl;
		}
	}
}

void StatsMenu::handlePlusButton2() {
	mycout::display("Upgrade button pressed!", mycout::Color::Green);

	auto response = callUpgradeAPI2(username);

	if (response.status_code == 200) {
		try {
			auto scores = fetchUserScoreFromServer(username);
			points = scores.points;
			winningPoints = scores.winningPoints;
			bulletSpeed = scores.bulletSpeed;
			bulletCooldown = scores.bulletCooldown;

			updateScoreText();
		}
		catch (const std::exception& e) {
			std::cerr << "Error fetching scores: " << e.what() << std::endl;
		}
	}
}

void StatsMenu::handleMinusButton2() {
	mycout::display("Downgrade button pressed!", mycout::Color::Green);

	auto response = callDowngradeAPI2(username);

	if (response.status_code == 200) {
		try {
			auto scores = fetchUserScoreFromServer(username);
			points = scores.points;
			winningPoints = scores.winningPoints;
			bulletSpeed = scores.bulletSpeed;
			bulletCooldown = scores.bulletCooldown;

			updateScoreText();
		}
		catch (const std::exception& e) {
			std::cerr << "Error fetching scores: " << e.what() << std::endl;
		}
	}
	else if (response.status_code == 400) {
		mycout::display("Not enough score to upgrade!", mycout::Color::Red);
	}
}

cpr::Response StatsMenu::callUpgradeAPI(const std::string& username)
{
	const std::string url = SERVER_URL + "/upgrade-bulletCooldown";
	cpr::Response response = cpr::Post(
		cpr::Url{ url },
		cpr::Body{ R"({"username":")" + username + R"("})" },
		cpr::Header{ {"Content-Type", "application/json"} }
	);

	return response;
}

cpr::Response StatsMenu::callDowngradeAPI(const std::string& username)
{
	const std::string url = SERVER_URL + "/downgrade-bulletCooldown";
	cpr::Response response = cpr::Post(
		cpr::Url{ url },
		cpr::Body{ R"({"username":")" + username + R"("})" },
		cpr::Header{ {"Content-Type", "application/json"} }
	);

	return response;
}

cpr::Response StatsMenu::callUpgradeAPI2(const std::string& username)
{
	const std::string url = SERVER_URL + "/upgrade-bulletSpeed";
	cpr::Response response = cpr::Post(
		cpr::Url{ url },
		cpr::Body{ R"({"username":")" + username + R"("})" },
		cpr::Header{ {"Content-Type", "application/json"} }
	);

	return response;
}

cpr::Response StatsMenu::callDowngradeAPI2(const std::string& username)
{
	const std::string url = SERVER_URL + "/downgrade-bulletSpeed";
	cpr::Response response = cpr::Post(
		cpr::Url{ url },
		cpr::Body{ R"({"username":")" + username + R"("})" },
		cpr::Header{ {"Content-Type", "application/json"} }
	);

	return response;
}

void StatsMenu::logout() {
	cpr::Post(cpr::Url{ SERVER_URL + "/logout" },
		cpr::Body{ crow::json::wvalue{{"username", username}}.dump() });
}

void StatsMenu::clean() {
}