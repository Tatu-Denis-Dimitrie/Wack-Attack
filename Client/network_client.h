#pragma once
#include <iostream>
#include <cpr/cpr.h>
#include <crow.h>
#include <nlohmann/json.hpp>

const std::string SERVER_URL = "http://localhost:18080";

struct UserStats {
	int points;
	int winningPoints;
	int bulletSpeed;
	int bulletCooldown;
};

void sendLoginRequest(const std::string& username);
void sendMoveRequest(const std::string& username, const std::string& direction);
void sendCreateBulletRequest(const std::string& username);
void sendDeleteBulletRequest(const std::string& username, int bulletIndex);
void sendGenerateMapRequest(int& number);
void sendChangeHealthRequest(const std::string& username);
UserStats fetchUserScoreFromServer(const std::string& username);
