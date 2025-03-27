#pragma once
#include <iostream>
#include <vector>
#include <string>

class Bullet {
public:
	Bullet() = default;

	Bullet(int coordX, int coordY, int m_bulletSpeed);

	int GetCoordX() const;
	void SetCoordX(int coordX);

	int GetCoordY() const;
	void SetCoordY(int coordY);

	std::string GetBulletDirection() const;
	void SetBulletDirection(std::string bulletDirection);

	int GetBulletSpeed() const;
	void SetBulletSpeed(int bulletSpeed);

	std::string GetUser() const;
	void SetUser(std::string username);

private:
	int m_x, m_y;
	int m_bulletSpeed;
	std::string m_bulletDirection;
	std::string m_username;
};