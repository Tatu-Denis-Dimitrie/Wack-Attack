#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <ranges>
#include <functional>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <crow.h>
#include "database/database.h"
#include "player.h"

class Routing
{
public:
	void Run(Database& database);
private:
	crow::SimpleApp m_app;
	std::string logged_in_user;
};
