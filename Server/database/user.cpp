#include "user.h"

User::User(const std::string& username, int points, int winningPoints, float bulletSpeed, float bulletCooldown)
	: m_username(username), m_points(points), m_winningPoints(winningPoints), m_bulletSpeed(bulletSpeed), m_bulletCooldown(bulletCooldown) {
}

int User::GetId() const {
	return m_id;
}

void User::SetId(int id) {
	m_id = id;
}

std::string User::GetUsername() const {
	return m_username;
}

void User::SetUsername(const std::string& username) {
	m_username = username;
}

int User::GetPoints() const
{
	return m_points;
}

void User::SetPoints(int points)
{
	m_points = points;
}

int User::GetWinningPoints() const
{
	return m_winningPoints;
}

void User::SetWinningPoints(int winningPoints)
{
	m_winningPoints = winningPoints;
}

int User::GetBulletSpeed() const
{
	return m_bulletSpeed;
}

void User::SetBulletSpeed(int bulletSpeed)
{
	m_bulletSpeed = bulletSpeed;
}

double User::GetBulletCooldown() const
{
	return m_bulletCooldown;
}

void User::SetBulletCooldown(float bulletCooldown)
{
	m_bulletCooldown = bulletCooldown;
}
