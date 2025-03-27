#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <map>
#include <SDL.h>
#include <SDL_image.h>
#include "bullet.h"
#include "network_client.h"

class Player {
public:
	Player(float x, float y, std::string skin, SDL_Renderer* rendererSdl, bool isReady, int bulletSpeed, int bulletCooldown);
	~Player();
	std::string GetName() const;
	void SetName(const std::string& name);

	double GetSpeed() const;

	int GetCoordX() const;
	void SetCoordX(int coordPlayerX);

	int GetCoordY() const;
	void SetCoordY(int coordPlayerY);

	int GetPlayerHealth() const;
	void SetPlayerHealth(int playerHealth);

	int GetBulletSpeed() const;
	void SetBulletSpeed(int bulletSpeed);

	std::string GetSkin() const;

	std::string GetPlayerOrientation() const;
	void SetPlayerOrientation(const std::string& playerOrientation);

	bool GetReady() const;
	void SetReady(bool ready);

	std::vector<Bullet>& GetBullets();
	void addBullet();
	void removeBullet(size_t index);

	SDL_Rect getCollisionBox() const;

	bool handleInput();
	void moveDown();
	void moveUp();
	void moveLeft();
	void moveRight();

	void render();
	void update();
	void clean();

	void SetTextureByOrientation(const std::string& skin);
	void loadNewTexture(const char* fileName, SDL_Texture*& texture);
	void loadTexture(const char* fileName);
private:
	int m_x, m_y;
	std::string m_skin;
	std::string m_playerOrientation;

	int m_movementSpeed;
	int m_playerHealth;

	std::string m_name;
	std::vector<Bullet> m_bullets;
	int m_bulletSpeed;
	double m_bulletCooldown;
	int lastBulletTime;
	double bulletCooldownIndex;

	bool isReady = false;

	Uint32 lastMovementTime;
	const Uint8* keyStates = SDL_GetKeyboardState(NULL);

	SDL_Renderer* rendererSdl;
	SDL_Texture* playerTexture;
	SDL_Texture* textureLeft;
	SDL_Texture* textureRight;
	SDL_Texture* textureUp;
	SDL_Texture* textureDown;

	SDL_Texture* textureLeftBlue;
	SDL_Texture* textureRightBlue;
	SDL_Texture* textureUpBlue;
	SDL_Texture* textureDownBlue;

	SDL_Texture* textureLeftRed;
	SDL_Texture* textureRightRed;
	SDL_Texture* textureUpRed;
	SDL_Texture* textureDownRed;

	SDL_Texture* textureLeftGreen;
	SDL_Texture* textureRightGreen;
	SDL_Texture* textureUpGreen;
	SDL_Texture* textureDownGreen;

	SDL_Rect playerRect;
};
