#include "player.h"

Player::Player(float x, float y, std::string skin, SDL_Renderer* rendererSdl, bool ready, int bulletSpeed, int bulletCooldown)
	: m_x(x), m_y(y), m_skin(skin), rendererSdl(rendererSdl), isReady(ready), m_bulletSpeed(bulletSpeed), bulletCooldownIndex(bulletCooldown)
{
	playerRect.h = 32;
	playerRect.w = 32;
	playerRect.x = m_x;
	playerRect.y = m_y;
	m_movementSpeed = 32;
	m_playerHealth = 3;
	m_bulletCooldown = 5000 - (bulletCooldownIndex * 1000);
	m_bulletSpeed *= 16;
	lastMovementTime = 0;
	lastBulletTime = 0;
	m_playerOrientation = "right";

	loadNewTexture("assets/player_right.png", textureRight);
	loadNewTexture("assets/player_left.png", textureLeft);
	loadNewTexture("assets/player_up.png", textureUp);
	loadNewTexture("assets/player_down.png", textureDown);

	loadNewTexture("assets/player_blue_right.png", textureRightBlue);
	loadNewTexture("assets/player_blue_left.png", textureLeftBlue);
	loadNewTexture("assets/player_blue_up.png", textureUpBlue);
	loadNewTexture("assets/player_blue_down.png", textureDownBlue);

	loadNewTexture("assets/player_red_right.png", textureRightRed);
	loadNewTexture("assets/player_red_left.png", textureLeftRed);
	loadNewTexture("assets/player_red_up.png", textureUpRed);
	loadNewTexture("assets/player_red_down.png", textureDownRed);

	loadNewTexture("assets/player_green_right.png", textureRightGreen);
	loadNewTexture("assets/player_green_left.png", textureLeftGreen);
	loadNewTexture("assets/player_green_up.png", textureUpGreen);
	loadNewTexture("assets/player_green_down.png", textureDownGreen);

	loadTexture(skin.c_str());
}

Player::~Player()
{
	if (playerTexture) {
		SDL_DestroyTexture(playerTexture);
		playerTexture = nullptr;
	}
}

double Player::GetSpeed() const
{
	return m_bulletSpeed;
}

std::string Player::GetName() const
{
	return m_name;
}

void Player::SetName(const std::string& name)
{
	m_name = name;
}

int Player::GetCoordX() const
{
	return m_x;
}

void Player::SetCoordX(int coordPlayerX)
{
	m_x = coordPlayerX;
	playerRect.x = coordPlayerX;
}

int Player::GetCoordY() const
{
	return m_y;
}

void Player::SetCoordY(int coordPlayerY)
{
	m_y = coordPlayerY;
	playerRect.y = coordPlayerY;
}

int Player::GetPlayerHealth() const
{
	return m_playerHealth;
}

void Player::SetPlayerHealth(int playerHealth)
{
	m_playerHealth = playerHealth;
}

int Player::GetBulletSpeed() const
{
	return m_bulletSpeed;
}

void Player::SetBulletSpeed(int bulletSpeed)
{
	m_bulletSpeed = bulletSpeed;
}

std::string Player::GetSkin() const
{
	return m_skin;
}

std::string Player::GetPlayerOrientation() const
{
	return m_playerOrientation;
}

void Player::SetPlayerOrientation(const std::string& playerOrientation)
{
	m_playerOrientation = playerOrientation;
}

bool Player::GetReady() const
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

void Player::SetTextureByOrientation(const std::string& skin)
{
	if (skin == "assets/player_blue.png")
	{
		if (m_playerOrientation == "up") {
			playerTexture = textureUpBlue;
		}
		else if (m_playerOrientation == "down") {
			playerTexture = textureDownBlue;
		}
		else if (m_playerOrientation == "left") {
			playerTexture = textureLeftBlue;
		}
		else if (m_playerOrientation == "right") {
			playerTexture = textureRightBlue;
		}
	}
	else if (skin == "assets/player_green.png")
	{
		if (m_playerOrientation == "up") {
			playerTexture = textureUpGreen;
		}
		else if (m_playerOrientation == "down") {
			playerTexture = textureDownGreen;
		}
		else if (m_playerOrientation == "left") {
			playerTexture = textureLeftGreen;
		}
		else if (m_playerOrientation == "right") {
			playerTexture = textureRightGreen;
		}
	}
	else if (skin == "assets/player_red.png")
	{
		if (m_playerOrientation == "up") {
			playerTexture = textureUpRed;
		}
		else if (m_playerOrientation == "down") {
			playerTexture = textureDownRed;
		}
		else if (m_playerOrientation == "left") {
			playerTexture = textureLeftRed;
		}
		else if (m_playerOrientation == "right") {
			playerTexture = textureRightRed;
		}
	}
}

void Player::loadNewTexture(const char* fileName, SDL_Texture*& texture)
{
	if (!texture) {
		SDL_Surface* surface = IMG_Load(fileName);
		if (!surface) {
			std::cerr << "Failed to load surface for " << fileName << ": " << IMG_GetError() << std::endl;
			return;
		}

		texture = SDL_CreateTextureFromSurface(rendererSdl, surface);
		SDL_FreeSurface(surface);

		if (!texture) {
			std::cerr << "Failed to create texture for " << fileName << ": " << SDL_GetError() << std::endl;
		}
	}
}

void Player::loadTexture(const char* fileName)
{
	if (!playerTexture) {
		SDL_Surface* playerSurface = IMG_Load(fileName);
		if (!playerSurface) {
			std::cerr << "Failed to load surface for " << fileName << ": " << IMG_GetError() << std::endl;
			return;
		}

		playerTexture = SDL_CreateTextureFromSurface(rendererSdl, playerSurface);
		SDL_FreeSurface(playerSurface);

		if (!playerTexture) {
			std::cerr << "Failed to create texture for " << fileName << ": " << SDL_GetError() << std::endl;
		}
	}
}

void Player::update()
{
	playerRect.x = m_x;
	playerRect.y = m_y;
}

bool Player::handleInput()
{
	bool actionPerformed = false;

	if (keyStates[SDL_SCANCODE_S] && !actionPerformed) {
		moveDown();
		actionPerformed = true;
	}

	if (keyStates[SDL_SCANCODE_W] && !actionPerformed) {
		moveUp();
		actionPerformed = true;
	}

	if (keyStates[SDL_SCANCODE_A] && !actionPerformed) {
		moveLeft();
		actionPerformed = true;
	}

	if (keyStates[SDL_SCANCODE_D] && !actionPerformed) {
		moveRight();
		actionPerformed = true;
	}

	if (keyStates[SDL_SCANCODE_SPACE]) {
		int currentTime = SDL_GetTicks();
		if (currentTime - lastBulletTime >= m_bulletCooldown)
		{
			addBullet();
			lastBulletTime = currentTime;
		}
	}
	return actionPerformed;
}

void Player::moveDown() {
	m_playerOrientation = "down";
	m_y += m_movementSpeed;
	playerTexture = textureDown;
}

void Player::moveUp() {
	m_playerOrientation = "up";
	m_y -= m_movementSpeed;
	playerTexture = textureUp;
}

void Player::moveLeft() {
	m_playerOrientation = "left";
	m_x -= m_movementSpeed;
	playerTexture = textureLeft;
}

void Player::moveRight() {
	m_playerOrientation = "right";
	m_x += m_movementSpeed;
	playerTexture = textureRight;
}

void Player::addBullet()
{
	if (m_playerOrientation == "up") {
		Bullet bullet(m_x, m_y - 16, rendererSdl, m_bulletSpeed);
		bullet.SetBulletDirection(m_playerOrientation);
		bullet.SetUser(m_name);
		bullet.loadTexture("assets/bullet.png");
		m_bullets.push_back(bullet);
		sendCreateBulletRequest(m_name);
	}
	else if (m_playerOrientation == "down") {
		Bullet bullet(m_x, m_y + 16, rendererSdl, m_bulletSpeed);
		bullet.SetBulletDirection(m_playerOrientation);
		bullet.SetUser(m_name);
		bullet.loadTexture("assets/bullet.png");
		m_bullets.push_back(bullet);
		sendCreateBulletRequest(m_name);
	}
	else if (m_playerOrientation == "left") {
		Bullet bullet(m_x - 16, m_y, rendererSdl, m_bulletSpeed);
		bullet.SetBulletDirection(m_playerOrientation);
		bullet.SetUser(m_name);
		bullet.loadTexture("assets/bullet.png");
		m_bullets.push_back(bullet);
		sendCreateBulletRequest(m_name);
	}
	else if (m_playerOrientation == "right") {
		Bullet bullet(m_x + 16, m_y, rendererSdl, m_bulletSpeed);
		bullet.SetBulletDirection(m_playerOrientation);
		bullet.SetUser(m_name);
		bullet.loadTexture("assets/bullet.png");
		m_bullets.push_back(bullet);
		sendCreateBulletRequest(m_name);
	}
}

void Player::removeBullet(size_t index) {
	if (index < m_bullets.size()) {
		m_bullets.erase(m_bullets.begin() + index);
	}
}

SDL_Rect Player::getCollisionBox() const
{
	return { playerRect.x, playerRect.y, playerRect.w, playerRect.h };
}

void Player::render() {
	SDL_RenderCopy(rendererSdl, playerTexture, nullptr, &playerRect);
}

void Player::clean()
{
	if (playerTexture) {
		SDL_DestroyTexture(playerTexture);
		playerTexture = nullptr;
	}
}