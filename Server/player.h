#pragma once
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "bullet.h"

class Player
{
public:
	Player(std::string username, float x, float y, int bulletSpeed);
	Player(float x, float y);

	Player(Player&& other) noexcept;
	Player& operator=(Player&& other) noexcept;

	std::string GetName() const;
	void SetName(const std::string& name);

	int GetPoints() const;
	void SetPoints(int points);

	int GetWinningPoints() const;
	void SetWinningPoints(int winningPoints);

	int GetBulletSpeed() const;
	void SetBulletSpeed(int bulletSpeed);

	double GetBulletCooldown() const;
	void SetBulletCooldown(float bulletCooldown);

	int GetCoordX() const;
	void SetCoordX(int coordPlayerX);

	int GetCoordY() const;
	void SetCoordY(int coordPlayerY);

	int GetPlayerHealth() const;
	void SetPlayerHealth(int playerHealth);

	float GetMovementSpeed() const;
	void SetMovementSpeed(float movementSpeed);

	std::string GetPlayerOrientation() const;
	void SetPlayerOrientation(std::string playerOrientation);

	bool IsReady() const;
	void SetReady(bool ready);

	std::vector<Bullet>& GetBullets();
	void addBullet(Bullet bullet);
	void removeBullet(size_t index);

	void update();
private:
	int m_x, m_y;
	std::string m_name;
	int m_points;
	int m_winningPoints;
	int m_bulletSpeed;
	int m_bulletCooldown;

	int m_playerHealth;
	bool isReady;

	int m_movementSpeed;
	std::string m_playerOrientation;

	std::vector<Bullet> m_bullets;
	int lastBulletTime;
	int bulletCooldown;
};
