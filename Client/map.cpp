#include "map.h"

bool IsCollidable(TileType type) {
	switch (type) {
	case TileType::WALL:
		return true;
	case TileType::BOX:
		return true;
	case TileType::BOMB:
		return true;
	case TileType::ROAD:
		return false;
	case TileType::LIFE:
		return false;
	}
}

Map::Map(SDL_Renderer* renderer)
	: renderer(renderer) {
	wall = LoadTexture("assets/wall.png");
	box = LoadTexture("assets/box.png");
	road = LoadTexture("assets/road.png");
	bomb = LoadTexture("assets/bomb.png");
	life = LoadTexture("assets/health.png");

	int number = 2;
	sendGenerateMapRequest(number);
	switch (number) {
	case 1:
		filePath = "assets/map1.txt";
		break;
	case 2:
		filePath = "assets/map2.txt";
		break;
	default:

		break;
	}
	lastModifiedTime = std::filesystem::last_write_time(filePath);
	LoadMap(filePath);
}

Map::~Map() {
	SDL_DestroyTexture(wall);
	SDL_DestroyTexture(box);
	SDL_DestroyTexture(road);
	SDL_DestroyTexture(bomb);
	SDL_DestroyTexture(life);
}

SDL_Texture* Map::LoadTexture(const std::string& filePath) {
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	if (!surface) {
		std::cerr << "Failed to load texture: " << filePath << " Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void Map::LoadMap(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open map file: " << filePath << std::endl;
		return;
	}

	map.clear();
	std::string line;

	while (std::getline(file, line)) {
		std::vector<TileType> row;
		for (char c : line) {
			switch (c) {
			case '0':
				row.push_back(TileType::ROAD);
				break;
			case '1':
				row.push_back(TileType::WALL);
				break;
			case '2':
				row.push_back(TileType::BOX);
				break;
			default:
				std::cerr << "Unknown tile type in map file: " << c << std::endl;
				break;
			}
		}
		map.push_back(row);
	}

	file.close();
}

void Map::DrawMap() {
	if (!wall || !box || !road || !bomb || !life) {
		std::cerr << "Textures not loaded properly. Skipping drawing...\n";
		return;
	}
	for (size_t row = 0; row < map.size(); ++row) {
		for (size_t col = 0; col < map[row].size(); ++col) {
			TileType type = map[row][col];
			SDL_Rect src = { 0, 0, TILE_SIZE, TILE_SIZE };
			SDL_Rect dest = { static_cast<int>(col * TILE_SIZE), static_cast<int>(row * TILE_SIZE), TILE_SIZE, TILE_SIZE };
			switch (type) {
			case TileType::ROAD:
				SDL_RenderCopy(renderer, road, &src, &dest);
				break;
			case TileType::WALL:
				SDL_RenderCopy(renderer, wall, &src, &dest);
				break;
			case TileType::BOX:
				SDL_RenderCopy(renderer, box, &src, &dest);
				break;
			case TileType::BOMB:
				SDL_RenderCopy(renderer, bomb, &src, &dest);
				break;
			case TileType::LIFE:
				SDL_RenderCopy(renderer, life, &src, &dest);
			default:
				break;
			}
		}
	}
}

void Map::AddBomb(int row, int col) {
	if (row < 0 || row >= static_cast<int>(map.size()) || col < 0 || col >= static_cast<int>(map[row].size())) {
		std::cerr << "Invalid position for bomb: (" << row << ", " << col << ")\n";
		return;
	}
	map[row][col] = TileType::BOMB;
}

void Map::AddPowerUp(int row, int col) {
	if (row < 0 || row >= static_cast<int>(map.size()) || col < 0 || col >= static_cast<int>(map[row].size())) {
		std::cerr << "Invalid position for power-up: (" << row << ", " << col << ")\n";
		return;
	}

	map[row][col] = TileType::LIFE;
}

void Map::GetBombCoordinates() {
	nlohmann::json requestJson;
	requestJson["walls"] = nlohmann::json::array();

	for (size_t row = 0; row < map.size(); ++row) {
		for (size_t col = 0; col < map[row].size(); ++col) {
			if (map[row][col] == TileType::WALL) {
				requestJson["walls"].push_back({ {"row", static_cast<int>(row)}, {"col", static_cast<int>(col)} });
			}
		}
	}

	cpr::Response response = cpr::Post(
		cpr::Url{ SERVER_URL + "/process-walls" },
		cpr::Header{ { "Content-Type", "application/json" } },
		cpr::Body{ requestJson.dump() });

	if (response.status_code == 200) {
		auto jsonResponse = nlohmann::json::parse(response.text);

		if (jsonResponse.contains("selected_walls")) {
			for (const auto& coord : jsonResponse["selected_walls"]) {
				int row = coord["row"];
				int col = coord["col"];

				std::cout << "Selected bomb at (" << row << ", " << col << ") by server." << std::endl;
				AddBomb(row, col);
			}
		}
		else {
			std::cerr << "Invalid response format from server: Missing 'selected_walls' key.\n";
		}
	}
	else {
		std::cerr << "Failed to communicate with server. HTTP code: " << response.status_code << ", Message: " << response.text << std::endl;
	}
}

void Map::GetPowerUpCoordinates() {
	nlohmann::json requestJson;
	requestJson["roads"] = nlohmann::json::array();

	for (size_t row = 0; row < map.size(); ++row) {
		for (size_t col = 0; col < map[row].size(); ++col) {
			if (map[row][col] == TileType::ROAD) {
				requestJson["roads"].push_back({
					{"row", static_cast<int>(row)},
					{"col", static_cast<int>(col)}
					});
			}
		}
	}

	cpr::Response response = cpr::Post(
		cpr::Url{ SERVER_URL + "/process-roads" },
		cpr::Header{ { "Content-Type", "application/json" } },
		cpr::Body{ requestJson.dump() }
	);

	if (response.status_code == 200) {
		auto jsonResponse = nlohmann::json::parse(response.text);

		if (jsonResponse.contains("selected_roads")) {
			for (const auto& coord : jsonResponse["selected_roads"]) {
				int row = coord["row"];
				int col = coord["col"];

				std::cout << "Selected power-up at (" << row << ", " << col << ") by server." << std::endl;

				AddPowerUp(row, col);
			}
		}
		else {
			std::cerr << "Invalid response format from server: Missing 'selected_roads' key.\n";
		}
	}
	else {
		std::cerr << "Failed to communicate with server. HTTP code: " << response.status_code
			<< ", Message: " << response.text << std::endl;
	}
}

void Map::ExplodeBomb(int row, int col) {
	if (row < 0 || row >= static_cast<int>(map.size()) || col < 0 || col >= static_cast<int>(map[row].size())) {
		std::cerr << "Invalid bomb position for explosion: (" << row << ", " << col << ")\n";
		return;
	}

	if (row > 0) {
		map[row - 1][col] = TileType::ROAD;
	}

	if (row < map.size() - 1) {
		map[row + 1][col] = TileType::ROAD;
	}

	if (col > 0) {
		map[row][col - 1] = TileType::ROAD;
	}

	if (col < map[row].size() - 1) {
		map[row][col + 1] = TileType::ROAD;
	}

	std::cout << "Bomb exploded at (" << row << ", " << col << ") and surrounding tiles converted to road." << std::endl;
}