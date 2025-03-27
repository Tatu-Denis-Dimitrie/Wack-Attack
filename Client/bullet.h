#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

class Bullet {
public:
	Bullet(int coordX, int coordY, SDL_Renderer* rendererSdl, int m_bulletSpeed);
	Bullet(int coordX, int coordY, SDL_Renderer* rendererSdl);

	float GetCoordX() const;
	void SetCoordX(float coordX);

	int GetCoordY() const;
	void SetCoordY(int coordY);

	std::string GetBulletDirection() const;
	void SetBulletDirection(std::string bulletDirection);

	std::string GetUser() const;
	void SetUser(std::string username);

	SDL_Rect getCollisionBox() const;

	void update();
	void render();
	void loadTexture(const char* fileName);
private:
	int m_x, m_y;
	int m_bulletSpeed;
	double bulletSpeedIndex;
	std::string m_bulletDirection;
	std::string m_username;

	SDL_Renderer* rendererSdl;
	SDL_Rect bulletRect;
	SDL_Texture* bulletTexture;
};