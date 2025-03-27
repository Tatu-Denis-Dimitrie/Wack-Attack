#include "bullet.h"

Bullet::Bullet(int coordX, int coordY, int m_bulletSpeed)
	: m_x(coordX), m_y(coordY), m_bulletSpeed(m_bulletSpeed) {
}

int Bullet::GetCoordX() const
{
	return m_x;
}

void Bullet::SetCoordX(int coordX)
{
	m_x = coordX;
}

int Bullet::GetCoordY() const
{
	return m_y;
}

void Bullet::SetCoordY(int coordY)
{
	m_y = coordY;
}

std::string Bullet::GetBulletDirection() const
{
	return m_bulletDirection;
}

void Bullet::SetBulletDirection(std::string bulletDirection)
{
	m_bulletDirection = bulletDirection;
}

int Bullet::GetBulletSpeed() const
{
	return m_bulletSpeed;
}

void Bullet::SetBulletSpeed(int bulletSpeed)
{
	m_bulletSpeed = bulletSpeed;
}

std::string Bullet::GetUser() const
{
	return m_username;
}

void Bullet::SetUser(std::string username)
{
	m_username = username;
}