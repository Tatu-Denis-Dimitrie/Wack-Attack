#include "player.h"

Player::Player(std::string username, float x, float y, int bulletSpeed)
	: m_name(username), m_x(x), m_y(y), m_bulletSpeed(bulletSpeed) {
	m_bulletSpeed *= 16;
	m_playerHealth = 3;
	m_movementSpeed = 32;
	lastBulletTime = 0;
	bulletCooldown = 1500;
	isReady = false;
}

Player::Player(float x, float y)
	: m_x(x), m_y(y) {
}

Player::Player(Player&& other) noexcept
	: m_x(other.m_x), m_y(other.m_y), m_name(std::move(other.m_name)),
	m_points(other.m_points), m_winningPoints(other.m_winningPoints),
	m_bulletSpeed(other.m_bulletSpeed), m_bulletCooldown(other.m_bulletCooldown),
	m_playerHealth(other.m_playerHealth),
	isReady(other.isReady), m_movementSpeed(other.m_movementSpeed),
	m_playerOrientation(std::move(other.m_playerOrientation)),
	m_bullets(std::move(other.m_bullets)), lastBulletTime(other.lastBulletTime),
	bulletCooldown(other.bulletCooldown)
{
}

Player& Player::operator=(Player&& other) noexcept
{
	if (this != &other)
	{
		m_x = other.m_x;
		m_y = other.m_y;
		m_name = std::move(other.m_name);
		m_points = other.m_points;
		m_winningPoints = other.m_winningPoints;
		m_bulletSpeed = other.m_bulletSpeed;
		m_bulletCooldown = other.m_bulletCooldown;
		m_playerHealth = other.m_playerHealth;
		isReady = other.isReady;
		m_movementSpeed = other.m_movementSpeed;
		m_playerOrientation = std::move(other.m_playerOrientation);
		m_bullets = std::move(other.m_bullets);
		lastBulletTime = other.lastBulletTime;
		bulletCooldown = other.bulletCooldown;

		other.m_x = other.m_y = 0;
		other.m_points = other.m_winningPoints = 0;
		other.m_bulletSpeed = other.m_bulletCooldown = 0;
		other.m_playerHealth = 0;
		other.lastBulletTime = 0;
		other.bulletCooldown = 0;
		other.isReady = false;
	}
	return *this;
}

std::string Player::GetName() const
{
	return m_name;
}

void Player::SetName(const std::string& name)
{
	m_name = name;
}

int Player::GetPoints() const
{
	return m_points;
}

void Player::SetPoints(int points)
{
	m_points = points;
}

int Player::GetWinningPoints() const
{
	return m_winningPoints;
}

void Player::SetWinningPoints(int winningPoints)
{
	m_winningPoints = winningPoints;
}

int Player::GetBulletSpeed() const
{
	return m_bulletSpeed;
}

void Player::SetBulletSpeed(int bulletSpeed)
{
	m_bulletSpeed = bulletSpeed;
}

double Player::GetBulletCooldown() const
{
	return m_bulletCooldown;
}

void Player::SetBulletCooldown(float bulletCooldown)
{
	m_bulletCooldown = bulletCooldown;
}

int Player::GetCoordX() const
{
	return m_x;
}

void Player::SetCoordX(int coordPlayerX)
{
	m_x = coordPlayerX;
}

int Player::GetCoordY() const
{
	return m_y;
}

void Player::SetCoordY(int coordPlayerY)
{
	m_y = coordPlayerY;
}

int Player::GetPlayerHealth() const
{
	return m_playerHealth;
}

void Player::SetPlayerHealth(int playerHealth)
{
	m_playerHealth = playerHealth;
}

float Player::GetMovementSpeed() const
{
	return m_movementSpeed;
}

void Player::SetMovementSpeed(float movementSpeed)
{
	m_movementSpeed = movementSpeed;
}

std::string Player::GetPlayerOrientation() const
{
	return m_playerOrientation;
}

void Player::SetPlayerOrientation(std::string playerOrientation)
{
	m_playerOrientation = playerOrientation;
}

bool Player::IsReady() const
{
	return isReady;
}

void Player::SetReady(bool ready)
{
	isReady = ready;
}

std::vector<Bullet>& Player::GetBullets()
{
	return m_bullets;
}

void Player::addBullet(Bullet bullet)
{
	m_bullets.push_back(bullet);
}

void Player::removeBullet(size_t index) {
	if (index < m_bullets.size()) {
		m_bullets.erase(m_bullets.begin() + index);
	}
}