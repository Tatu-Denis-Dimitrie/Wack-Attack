#include "game.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 640;

Game::Game()
	: player(nullptr), map(nullptr), rendererSdl(nullptr), window(nullptr) {
}

Game::~Game()
{
	delete map;
	delete player;
}
void Game::initializer(const char* title, int x, int y, int width, int height, bool fullscreen)
{
	int windowManager = 0;
	if (fullscreen == true)
	{
		windowManager = SDL_WINDOW_FULLSCREEN;
	}
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		mycerr::display("Could not init SDL: " + std::string(SDL_GetError()), mycerr::Color::Red);
	}
	else if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		mycout::display("The sys are init!", mycout::Color::Green);
		mycout::display("Salut", mycout::Color::Green);
		window = SDL_CreateWindow(title, x, y, width, height, windowManager);
		if (window != 0)
		{
			mycout::display("The window was created succesfully !", mycout::Color::Green);

			SDL_Surface* icon = SDL_LoadBMP("assets/icon.bmp");
			if (icon != nullptr)
			{
				SDL_SetWindowIcon(window, icon);
				SDL_FreeSurface(icon);
			}
			else
			{
				mycerr::display("Could not load icon: " + std::string(SDL_GetError()), mycerr::Color::Red);
			}
		}
		else
		{
			mycerr::display("Could not init window: " + std::string(SDL_GetError()), mycerr::Color::Red);
		}

		rendererSdl = SDL_CreateRenderer(window, -1, 0);
		if (rendererSdl != 0)
		{
			mycout::display("The renderer has been created succesfully!", mycout::Color::Green);
			SDL_SetRenderDrawColor(rendererSdl, 69, 69, 69, 0);
		}
		else
		{
			mycerr::display("Could not init renderer: " + std::string(SDL_GetError()), mycerr::Color::Red);
		}
	}
	else
	{
		isRunning = false;
	}
}

void Game::run()
{
	using Clock = std::chrono::high_resolution_clock;
	Timer time;
	initializer("Wack Attack", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, false);
	audio.Init();
	audio.PlayMusic("assets/mainMenu.mp3");
	Mix_VolumeMusic(1);
	LoginScreen loginScreen(rendererSdl);
	loginScreen.init();

	while (true) {
		while (loginScreen.isActive) {
			loginScreen.handleEvents();
			loginScreen.render();
			if (loginScreen.isLoggedIn) {
				mycout::display("Username: " + loginScreen.getUsername(), mycout::Color::Cyan);
				username = loginScreen.getUsername();
				mycout::display("Login successful, proceeding to game...", mycout::Color::Cyan);
				break;
			}
		}

		if (!loginScreen.isLoggedIn) {
			mycerr::display("Login failed or was canceled!", mycerr::Color::Red);
			return;
		}

		MainMenu mainMenu(username, rendererSdl);
		StatsMenu statsMenu(username, rendererSdl);
		while (mainMenu.isActive)
		{
			mainMenu.handleEvent();
			mainMenu.render();
			Uint32 currentTime = SDL_GetTicks();
			if (!mainMenu.isActive && StatsMenu::isActiveStats) {
				mycout::display("Navigating to stats page...", mycout::Color::Yellow);
				while (StatsMenu::isActiveStats) {
					statsMenu.handleEvent();
					statsMenu.render();
				}
			}
			else
				if (!mainMenu.isActive && isRunning) {
					mycout::display("Exiting Main Menu and starting game...", mycout::Color::Cyan);

					initializeObjects();
					break;
				}
		}

		if (!isRunning) {
			mycout::display("Game was not started from Main Menu. Exiting...", mycout::Color::Red);
			return;
		}

		Uint32 lastHandleInputTime = SDL_GetTicks();
		audio.StopMusic();
		audio.PlayMusic("assets/inGame.mp3");
		Mix_VolumeMusic(1);
		startGameTimer();
		while (running())
		{
			handleEvents();
			Uint32 currentTime = SDL_GetTicks();
			if (currentTime - lastHandleInputTime >= 650) {
				SDL_Rect playerRect = player->getCollisionBox();
				if (player->handleInput()) {
					if (CheckPlayerCollision()) {
						mycout::display("Coliziune detectată între jucător și un obiect colizibil!", mycout::Color::Red);
						player->SetCoordX(playerRect.x);
						player->SetCoordY(playerRect.y);
					}
					else {
						sendMoveRequest(username, player->GetPlayerOrientation());
					}
				}
				lastHandleInputTime = currentTime;
			}
			updater();
			renderer();

			static Uint32 frameCount = 0;
			static Uint32 startTime = SDL_GetTicks();

			frameCount++;
			Uint32 elapsedTime = SDL_GetTicks() - startTime;
			if (elapsedTime >= 1000) {
				mycout::display("FPS: " + std::to_string(frameCount), mycout::Color::Magenta);
				frameCount = 0;
				startTime = SDL_GetTicks();
			}
		}
		stopGameTimer();
		mycout::display("Durata jocului a fost de : " + std::string(timer.formatTime()), mycout::Color::Blue);
		cleanObjects();
		EndScreen endScreen(rendererSdl);
		endScreen.init();
		while (EndScreen::isActiveEND) {
			endScreen.handleEvents();
			endScreen.render();
		}
	}
	clean();
}

void Game::initializeObjects()
{
	int x, y, bulletSpeed, bulletCooldown;
	auto response = cpr::Get(cpr::Url{ SERVER_URL + "/players/" + this->username });
	if (response.status_code == 200) {
		auto json_response = crow::json::load(response.text);
		if (json_response) {
			x = json_response["x"].i();
			y = json_response["y"].i();
			bulletSpeed = json_response["bulletSpeed"].i();
			bulletCooldown = json_response["bulletCooldown"].i();
		}
		else
		{
			mycerr::display("Error parsing Json", mycerr::Color::Red);
		}
	}
	else
	{
		mycout::display("Player not found or error in request.");
	}
	map = new Map(rendererSdl);
	map->GetBombCoordinates();
	map->GetPowerUpCoordinates();

	std::string skin = "assets/player.png";
	player = new Player(x, y, skin, rendererSdl, true, bulletSpeed, bulletCooldown);
	player->SetName(username);
	mycout::display("Playser username" + player->GetName());
}

void Game::handleEvents()
{
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		sendLogoutRequest();
		isRunning = false;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_x) {
			sendLogoutRequest();
			isRunning = false;
		}
		else if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			sendLogoutRequest();
			isRunning = false;
		}
		break;
	default:
		break;
	}
}

void Game::updater() {
	Uint32 currentTime = SDL_GetTicks();

	if (currentTime - lastPlayerUpdateTime >= 650) {
		player->update();
		fetchAndUpdatePlayers();
		if (!player->GetBullets().empty()) {
			CheckBulletCollision();
		}
		for (auto& bullet : player->GetBullets()) {
			bullet.update();
		}
		CheckForMapUpdates();
		CheckEndGameCondition();
		showAllPlayers();
		lastPlayerUpdateTime = currentTime;
	}
}

void Game::renderer() {
	SDL_RenderClear(rendererSdl);
	map->DrawMap();
	if (player->GetReady()) {
		player->render();
	}
	for (const auto& [username, player] : players) {
		if (player->GetReady()) {
			player->render();
			for (auto bullet : player->GetBullets()) {
				bullet.render();
			}
		}
	}

	for (auto& bullet : player->GetBullets())
		bullet.render();

	SDL_RenderPresent(rendererSdl);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(rendererSdl);
	SDL_Quit();

	mycout::display("Game has been closed!");
}

void Game::cleanObjects()
{
	if (player) {
		delete player;
		player = nullptr;
	}
	if (map) {
		delete map;
		map = nullptr;
	}
	for (auto& [username, playerObj] : players) {
		delete playerObj;
	}
	players.clear();
	mycout::display("All objects have been cleaned!", mycout::Color::Green);
}

bool Game::running()
{
	return isRunning;
}

std::string Game::getRandomSkin() {
	static std::vector<std::string> skins = {
		"assets/player_blue.png",
		"assets/player_green.png",
		"assets/player_red.png"
	};

	if (skins.empty()) {
		mycerr::display("No skins left!", mycerr::Color::Red);
		return "assets/player.png";
	}

	std::srand(std::time(0));
	int index = std::rand() % skins.size();
	std::string selectedSkin = skins[index];

	skins.erase(skins.begin() + index);

	return selectedSkin;
}

void Game::sendLogoutRequest() {
	cpr::Post(cpr::Url{ SERVER_URL + "/logout" },
		cpr::Body{ crow::json::wvalue{{"username", player->GetName()}}.dump() });

	auto it = players.find(player->GetName());
	if (it != players.end()) {
		delete it->second;
		players.erase(it);
	}
	for (auto it = players.begin(); it != players.end(); ++it) {
		if (it->first == player->GetName())
		{
			delete it->second;
			players.erase(it);
		}
	}
}

void Game::fetchAndUpdatePlayers() {
	auto response = cpr::Get(cpr::Url{ SERVER_URL + "/players" });
	if (response.status_code != 200) {
		mycerr::display("Failed to fetch players from server. Status code: " + std::to_string(response.status_code), mycerr::Color::Red);
		return;
	}

	auto json_response = crow::json::load(response.text);
	if (!json_response || !json_response.has("players")) {
		mycerr::display("Invalid response format from server.", mycerr::Color::Red);
		return;
	}

	std::unordered_set<std::string> current_players;

	for (const auto& player_data : json_response["players"]) {
		if (!player_data.has("username") || !player_data.has("x") || !player_data.has("y")) {
			mycerr::display("Player data is incomplete. Skipping...", mycerr::Color::Red);
			continue;
		}

		std::string username = player_data["username"].s();
		int x = player_data["x"].i();
		int y = player_data["y"].i();
		int bulletSpeed = player_data["bulletSpeed"].i();
		bool isReady = player_data["isReady"].b();
		int health = player_data["health"].i();
		std::string playerOrientation = player_data["playerOrientation"].s();

		if (username == this->username) {
			player->SetPlayerHealth(health);
			if (!isReady) {
				player->SetReady(false);
				player->GetBullets().clear();
			}
			player->SetPlayerOrientation(playerOrientation);
			player->SetTextureByOrientation(player->GetSkin());
			continue;
		}

		current_players.insert(username);

		if (auto it = players.find(username); it != players.end()) { // Update player
			it->second->SetCoordX(x);
			it->second->SetCoordY(y);
			it->second->SetReady(isReady);
			it->second->SetPlayerHealth(health);
			it->second->SetPlayerOrientation(playerOrientation);
			it->second->SetTextureByOrientation(it->second->GetSkin());

			if (player_data.has("bullets")) {
				auto& bullets = it->second->GetBullets();
				const auto& bullets_data = player_data["bullets"];
				bullets.clear();

				// Adăugăm noile gloanțe primite de la server
				for (const auto& bullet_data : bullets_data) {
					int bullet_x = bullet_data["x"].i();
					int bullet_y = bullet_data["y"].i();
					std::string direction = bullet_data["direction"].s();

					// Creați un obiect Bullet și adăugați-l în lista de gloanțe
					mycout::display("Bullet x: " + std::to_string(bullet_x), mycout::Color::Cyan);
					mycout::display("Bullet y: " + std::to_string(bullet_y), mycout::Color::Cyan);
					mycout::display("Bullet speed: " + std::to_string(it->second->GetBulletSpeed()), mycout::Color::Blue);
					Bullet new_bullet(bullet_x, bullet_y, rendererSdl, it->second->GetBulletSpeed());
					new_bullet.SetBulletDirection(direction);
					new_bullet.loadTexture("assets/bullet.png");
					bullets.push_back(new_bullet);
				}
			}
		}
		else {
			std::string skin;
			skin = getRandomSkin();
			players[username] = new Player(x, y, skin, rendererSdl, isReady, bulletSpeed, 1);
			players[username]->SetPlayerOrientation(playerOrientation);
		}
	}

	for (auto it = players.begin(); it != players.end();) { //Setrge din players la logout
		if (current_players.find(it->first) == current_players.end()) {
			delete it->second;
			it = players.erase(it);
		}
		else {
			++it;
		}
	}
}

void Game::CheckBulletCollision()
{
	int bulletIndex = 0;
	for (auto it = player->GetBullets().begin(); it != player->GetBullets().end(); ++bulletIndex) {
		SDL_Rect bulletRect = it->getCollisionBox();
		bool collisionDetected = false;

		for (int i = 0; i <= player->GetBulletSpeed() && !collisionDetected; i += 16) {
			if (it->GetBulletDirection() == "up") {
				bulletRect.y -= 16;
			}
			else if (it->GetBulletDirection() == "down") {
				bulletRect.y += 16;
			}
			else if (it->GetBulletDirection() == "left") {
				bulletRect.x -= 16;
			}
			else if (it->GetBulletDirection() == "right") {
				bulletRect.x += 16;
			}

			// Verificăm coliziunea cu jucători
			for (const auto& pair : players) {
				Player* otherPlayer = pair.second;
				if (otherPlayer != player) {
					SDL_Rect playerRect = otherPlayer->getCollisionBox();
					if (SDL_HasIntersection(&bulletRect, &playerRect)) {
						mycout::display("Bullet hit a player!");
						otherPlayer->SetPlayerHealth(otherPlayer->GetPlayerHealth() - 1);
						sendChangeHealthRequest(pair.first);
						mycout::display("Player " + pair.first + " health: " + std::to_string(otherPlayer->GetPlayerHealth()));
						sendUpdatePoints(player->GetName());
						collisionDetected = true;
						break;
					}
				}
			}

			if (!collisionDetected) {
				for (int row = 0; row < map->GetMap().size(); ++row) {
					for (int col = 0; col < map->GetMap()[row].size(); ++col) {
						TileType tile = map->GetMap()[row][col];
						if (IsCollidable(tile)) {
							SDL_Rect tileRect;
							tileRect.x = col * TILE_SIZE;
							tileRect.y = row * TILE_SIZE;
							tileRect.w = TILE_SIZE;
							tileRect.h = TILE_SIZE;

							if (SDL_HasIntersection(&bulletRect, &tileRect)) {
								mycout::display("Bullet hit a map object!");
								collisionDetected = true;
								if (tile == TileType::WALL) {
									map->GetMap()[row][col] = TileType::ROAD;
									SendMapUpdateRequest(row, col, "wall");
								}
								if (tile == TileType::BOMB) {
									map->GetMap()[row][col] = TileType::ROAD;
									map->ExplodeBomb(row, col);
									SendMapUpdateRequest(row, col, "bomb");
								}
								break;
							}
						}
					}
					if (collisionDetected)
						break;
				}
			}

			if (!collisionDetected) {
				for (auto& otherBullet : player->GetBullets()) {
					if (&otherBullet != &(*it)) {
						SDL_Rect otherBulletRect = otherBullet.getCollisionBox();
						if (SDL_HasIntersection(&bulletRect, &otherBulletRect)) {
							mycout::display("Bullet collided with another bullet!");
							collisionDetected = true;
							break;
						}
					}
				}
			}
		}
		if (collisionDetected) {
			it = player->GetBullets().erase(it);
			sendDeleteBulletRequest(player->GetName(), bulletIndex);
		}
		else {
			++it;
		}
	}
}

void Game::sendUpdatePoints(const std::string& username) {
	crow::json::wvalue jsonRequest;
	jsonRequest["username"] = username;

	cpr::Response response = cpr::Post(cpr::Url{ SERVER_URL + "/update-points" },
		cpr::Body{ jsonRequest.dump() },
		cpr::Header{ {"Content-Type", "application/json"} });

	if (response.status_code == 200) {
		mycout::display("Points update request sent for " + username, mycout::Color::Green);
	}
	else {
		mycout::display("Failed to send points update request for " + username, mycout::Color::Red);
	}
}

bool Game::CheckPlayerCollision()
{
	SDL_Rect playerRect;
	playerRect.x = player->GetCoordX();
	playerRect.y = player->GetCoordY();
	playerRect.w = TILE_SIZE;
	playerRect.h = TILE_SIZE;

	// Iterăm prin fiecare tile din hartă
	for (int row = 0; row < map->GetMap().size(); ++row) {
		for (int col = 0; col < map->GetMap()[row].size(); ++col) {
			TileType tile = map->GetMap()[row][col];

			SDL_Rect tileRect;
			tileRect.x = col * TILE_SIZE;
			tileRect.y = row * TILE_SIZE;
			tileRect.w = TILE_SIZE;
			tileRect.h = TILE_SIZE;

			// Verificăm dacă tile-ul este colizibil
			if (IsCollidable(tile)) {
				// Verificăm coliziunea între dreptunghiul jucătorului și dreptunghiul tile-ului
				if (SDL_HasIntersection(&playerRect, &tileRect)) {
					return true;  // Coliziune generală
				}
			}
			else {
				if (SDL_HasIntersection(&playerRect, &tileRect)) {
					if (tile == TileType::LIFE) {
						mycout::display("Power-up collected! Life increased.", mycout::Color::Magenta);
						player->SetPlayerHealth(player->GetPlayerHealth() + 1);

						map->GetMap()[row][col] = TileType::ROAD;
						SendMapUpdateRequest(row, col, "life");
						return false;
					}
				}
			}
		}
	}
	return false;
}

void Game::startGameTimer() {
	timer.start();
}

void Game::stopGameTimer()
{
	timer.stop();
}

void Game::SendMapUpdateRequest(int row, int col, const std::string& tileType)
{
	nlohmann::json requestJson;
	requestJson["row"] = row;
	requestJson["col"] = col;
	requestJson["tileType"] = tileType;
	cpr::Response response = cpr::Post(
		cpr::Url{ SERVER_URL + "/update-map" },
		cpr::Header{ { "Content-Type", "application/json" } },
		cpr::Body{ requestJson.dump() }
	);

	if (response.status_code == 200) {
		std::cout << "Tile update sent to server successfully.\n";
	}
	else {
		std::cerr << "Failed to send tile update to server. HTTP code: " << response.status_code << ", Message: " << response.text << std::endl;
	}
}

void Game::CheckForMapUpdates()
{
	cpr::Response response = cpr::Get(
		cpr::Url{ SERVER_URL + "/update-map" }
	);

	if (response.status_code == 200) {
		try {
			nlohmann::json jsonResponse = nlohmann::json::parse(response.text);
			int row = jsonResponse.at("row").get<int>();
			int col = jsonResponse.at("col").get<int>();
			std::string tileType = jsonResponse.at("tileType").get<std::string>();

			if (tileType == "bomb") {
				map->GetMap()[row][col] = TileType::ROAD;
				map->ExplodeBomb(row, col);
			}
			else {
				map->GetMap()[row][col] = TileType::ROAD;
			}
			std::cout << "Map updated at (" << row << ", " << col << ").\n";
		}
		catch (const std::exception& e) {
			std::cerr << "Failed to parse map update: " << e.what() << "\n";
		}
	}
	else if (response.status_code == 204) {
	}
	else {
		std::cerr << "Failed to get map update. HTTP code: " << response.status_code
			<< ", Message: " << response.text << "\n";
	}
}

void Game::CheckEndGameCondition() {
	int alivePlayers = 0;
	std::string lastAlivePlayer = player->GetName();

	if (player->GetPlayerHealth() > 0) {
		alivePlayers++;
	}

	for (const auto& [username, otherPlayer] : players) {
		if (otherPlayer && otherPlayer->GetPlayerHealth() > 0) {
			//std::cout << otherPlayer->GetPlayerHealth() << std::endl;;
			alivePlayers++;
			lastAlivePlayer = username;
		}
	}

	if (alivePlayers <= 1) {
		sendWinPoints(lastAlivePlayer);
		EndScreen::isActiveEND = true;
		isRunning = false;
	}
}

void Game::sendWinPoints(const std::string& username) {
	crow::json::wvalue jsonRequest;
	jsonRequest["username"] = username;

	cpr::Response response = cpr::Post(cpr::Url{ SERVER_URL + "/update-winningpoints" },
		cpr::Body{ jsonRequest.dump() },
		cpr::Header{ {"Content-Type", "application/json"} });

	if (response.status_code == 200) {
		mycout::display("Points update request sent for " + username, mycout::Color::Green);
	}
	else {
		mycout::display("Failed to send points update request for " + username, mycout::Color::Red);
	}
}

void Game::showAllPlayers() {
	mycout::display("Current players in the game:", mycout::Color::Blue);
	mycout::display("Username: " + player->GetName() + ", X: " + std::to_string(player->GetCoordX()) + ", Y: " + std::to_string(player->GetCoordY()), mycout::Color::Yellow);
	for (const auto& [username, player] : players) {
		if (player) {
			mycout::display("Username: " + username + ", X: " + std::to_string(player->GetCoordX()) + ", Y: " + std::to_string(player->GetCoordY()), mycout::Color::Yellow);
		}
		else {
			mycout::display("Username: " + username + " has no player object.", mycout::Color::Red);
		}
	}
}