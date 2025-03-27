#include "bullet.h"

Bullet::Bullet(int coordX, int coordY, SDL_Renderer* rendererSdl, int bulletSpeed)
	: m_x(coordX), m_y(coordY), rendererSdl(rendererSdl), m_bulletSpeed(bulletSpeed)
{
	bulletRect.x = m_x;
	bulletRect.y = m_y;
	bulletRect.w = 16;
	bulletRect.h = 16;
}
Bullet::Bullet(int coordX, int coordY, SDL_Renderer* rendererSdl)
	: m_x(coordX), m_y(coordY), rendererSdl(rendererSdl)
{
	bulletRect.x = m_x;
	bulletRect.y = m_y;
	bulletRect.w = 16;
	bulletRect.h = 16;
}

float Bullet::GetCoordX() const
{
	return m_x;
}

void Bullet::SetCoordX(float coordX)
{
	m_x = coordX;
	bulletRect.x = coordX;
}

int Bullet::GetCoordY() const
{
	return m_y;
}

void Bullet::SetCoordY(int coordY)
{
	m_y = coordY;
	bulletRect.y = coordY;
}

std::string Bullet::GetBulletDirection() const
{
	return m_bulletDirection;
}

void Bullet::SetBulletDirection(std::string bulletDirection)
{
	m_bulletDirection = bulletDirection;
}

std::string Bullet::GetUser() const
{
	return m_username;
}

void Bullet::SetUser(std::string username)
{
	m_username = username;
}

SDL_Rect Bullet::getCollisionBox() const
{
	return { bulletRect.x, bulletRect.y, bulletRect.w, bulletRect.h };
}

void Bullet::update()
{
	if (m_bulletDirection == "right")
		m_x += m_bulletSpeed;
	else if (m_bulletDirection == "left")
		m_x -= m_bulletSpeed;
	else if (m_bulletDirection == "up")
		m_y -= m_bulletSpeed;
	else if (m_bulletDirection == "down")
		m_y += m_bulletSpeed;

	bulletRect.x = m_x;
	bulletRect.y = m_y;
}

void Bullet::loadTexture(const char* fileName)
{
	SDL_Surface* bulletSurface = IMG_Load(fileName);
	bulletTexture = SDL_CreateTextureFromSurface(rendererSdl, bulletSurface);
	SDL_FreeSurface(bulletSurface);
}

void Bullet::render()
{
	SDL_RenderCopy(rendererSdl, bulletTexture, NULL, &bulletRect);
}