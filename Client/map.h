#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <SDL.h>
#include <SDL_image.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "network_client.h"

const int TILE_SIZE = 32;

enum class TileType {
	ROAD,
	WALL,
	BOX,
	BOMB,
	LIFE
};

bool IsCollidable(TileType type);

class Map {
public:
	Map(SDL_Renderer* renderer);
	~Map();

	void LoadMap(const std::string& filePath);
	void DrawMap();
	std::vector<std::vector<TileType>>& GetMap() {
		return map;
	}

	void AddBomb(int row, int col);
	void AddPowerUp(int row, int col);
	void GetBombCoordinates();
	void GetPowerUpCoordinates();
	void ExplodeBomb(int row, int col);

private:
	std::vector<std::vector<TileType>> map;
	std::string filePath;

	SDL_Texture* wall;
	SDL_Texture* box;
	SDL_Texture* road;
	SDL_Texture* bomb;
	SDL_Texture* life;
	SDL_Renderer* renderer;
	SDL_Texture* LoadTexture(const std::string& filePath);
	std::filesystem::file_time_type lastModifiedTime;
};