#include "routing.h"

const std::string SERVER_URL = "http://localhost:18080";

template <typename Collection, typename Predicate>
auto find_if(Collection& collection, Predicate predicate) {
	return std::ranges::find_if(collection, predicate);
}

template <typename... Args>
void log(const std::string& message, Args&&... args) {
	((std::cout << message << ": " << std::forward<Args>(args) << "\n"), ...);
}

std::pair<float, float> getCornerCoordinates(int cornerIndex) {
	const float MAP_WIDTH = 800; // Ajustează în funcție de dimensiunea reală
	const float MAP_HEIGHT = 640; // Ajustează în funcție de dimensiunea reală
	const float OFFSET = 32;
	switch (cornerIndex) {
	case 0:
		return { OFFSET, OFFSET };
	case 1:
		return { MAP_WIDTH - (OFFSET * 2), OFFSET };
	case 2:
		return { OFFSET, MAP_HEIGHT - (OFFSET * 2) };
	case 3:
		return { MAP_WIDTH - (OFFSET * 2), MAP_HEIGHT - (OFFSET * 2) };
	default:
		return { 0, 0 };
	}
}

void Routing::Run(Database& database)
{
	srand(static_cast<unsigned int>(time(NULL)));
	std::vector<Player> connectedUsers;

	CROW_ROUTE(m_app, "/")([]() {
		return "Available /'s \n " +
			SERVER_URL + "/login \n" +
			SERVER_URL + "/players \n" +
			SERVER_URL + "/connected-users \n" +
			SERVER_URL + "/connected-user/score \n";
		});

	CROW_ROUTE(m_app, "/login")
		.methods("POST"_method, "GET"_method)
		([&database, &connectedUsers](const crow::request& req) {
		if (req.method == crow::HTTPMethod::POST) {
			auto body = crow::json::load(req.body);
			std::string username = body["username"].s();

			log("Received login request", username);

			auto existingUsers = database.GetUsers();
			bool userExists = false;
			User logged_in_user;

			auto it = find_if(existingUsers, [&username](const User& user) {
				return user.GetUsername() == username;
				});

			if (it != existingUsers.end()) {
				userExists = true;
				logged_in_user = *it;
			}
			int cornerIndex = connectedUsers.size() % 4;
			auto [spawnX, spawnY] = getCornerCoordinates(cornerIndex);
			Player player = Player(username, spawnX, spawnY, logged_in_user.GetBulletSpeed());
			if (userExists) {
				logged_in_user.SetWinningPoints(logged_in_user.GetWinningPoints() + 100);
				logged_in_user.SetPoints(logged_in_user.GetPoints() + 1000);
				database.SaveUser(logged_in_user);

				connectedUsers.push_back(std::move(player));
				log("Login successful", logged_in_user.GetUsername());
				log("Points", logged_in_user.GetPoints());
				log("WinningPoints", logged_in_user.GetWinningPoints());

				return crow::response(200, crow::json::wvalue{
					{"username", username},
					{"points", logged_in_user.GetPoints()},
					{"winningPoints", logged_in_user.GetWinningPoints()},
					{"bulletSpeed", logged_in_user.GetBulletSpeed()},
					{"bulletCooldown", logged_in_user.GetBulletCooldown()}
					});
			}
			else if (!userExists) {
				User user(username, 0, 0, 1, 1); //username, points, winningPoints, bulletSpeed, bulletCooldown
				database.SaveUser(user);
				connectedUsers.push_back(std::move(player));

				log("New user created and logged in", username);

				return crow::response(200, crow::json::wvalue{
					{"username", user.GetUsername()},
					{"points", user.GetPoints()},
					{"winningPoints", user.GetWinningPoints()},
					{"bulletSpeed", user.GetBulletSpeed()},
					{"bulletCooldown", user.GetBulletCooldown()}
					});
			}
		}
		else if (req.method == crow::HTTPMethod::GET) {
			std::vector<crow::json::wvalue> users_json;

			std::ranges::for_each(database.GetUsers(), [&users_json](const User& user) {
				users_json.push_back(crow::json::wvalue{
					{"username", user.GetUsername()},
					{"points", user.GetPoints()},
					{"winningPoints", user.GetWinningPoints()},
					{"bulletSpeed", user.GetBulletSpeed()},
					{"bulletCooldown", user.GetBulletCooldown()}
					});
				});
			crow::json::wvalue response_json = users_json;
			return crow::response{ response_json };
		}

		return crow::response(405, "Method Not Allowed");
			});

	CROW_ROUTE(m_app, "/players/<string>")
		.methods("GET"_method)
		([&database, &connectedUsers](const crow::request& req, std::string username) {
		auto player_it = std::find_if(connectedUsers.begin(), connectedUsers.end(),
			[&username](const Player& p) {
				return p.GetName() == username;
			});

		if (player_it != connectedUsers.end()) {
			Player& player = *player_it;

			crow::json::wvalue response_json{
				{"username", player.GetName()},
				{"x", player.GetCoordX()},
				{"y", player.GetCoordY()},
				{"bulletSpeed",player.GetBulletSpeed()},
				{"bulletCooldown",player.GetBulletCooldown()}
			};

			return crow::response{ response_json };
		}

		return crow::response(404, "Player not found");
			});

	CROW_ROUTE(m_app, "/connected-users")
		.methods("GET"_method)
		([&connectedUsers]() {
		std::vector<crow::json::wvalue> users_json;

		std::ranges::for_each(connectedUsers, [&users_json](const Player& player) {
			users_json.push_back(crow::json::wvalue{
				{"username", player.GetName()},
				});
			});
		crow::json::wvalue response_json = users_json;
		return crow::response{ response_json };
			});

	CROW_ROUTE(m_app, "/connected-user/score")
		.methods("GET"_method)
		([&connectedUsers]() {
		std::vector<crow::json::wvalue> users_json;

		std::ranges::for_each(connectedUsers, [&users_json](const Player& player) {
			users_json.push_back(crow::json::wvalue{
				{"username", player.GetName()},
				{"points", player.GetPoints()},
				{"winningPoints", player.GetWinningPoints()},
				{"bulletSpeed", player.GetBulletSpeed()},
				{"bulletCooldown", player.GetBulletCooldown()}
				});
			});

		crow::json::wvalue response_json = users_json;
		return crow::response{ response_json };
			});

	CROW_ROUTE(m_app, "/connected-user/score")
		.methods("POST"_method)
		([&database, &connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username")) {
			return crow::response(400, "Invalid request. Missing username.");
		}

		std::string username = body["username"].s();

		auto player = std::find_if(connectedUsers.begin(), connectedUsers.end(),
			[&username](const Player& player) {
				return player.GetName() == username;
			});
		if (player != connectedUsers.end()) {
			auto databaseUsers = database.GetUsers();
			auto user = std::ranges::find_if(databaseUsers, [&username](const User& user) {
				return user.GetUsername() == username;
				});
			if (user != databaseUsers.end()) {
				player->SetPoints(user->GetPoints());
				player->SetWinningPoints(user->GetWinningPoints());
				player->SetBulletSpeed(user->GetBulletSpeed());
				player->SetBulletCooldown(user->GetBulletCooldown());
			}

			return crow::response(200, crow::json::wvalue{
				{"username", username},
				{"points", player->GetPoints()},
				{"winningPoints", player->GetWinningPoints()},
				{"bulletSpeed", player->GetBulletSpeed()},
				{"bulletCooldown", player->GetBulletCooldown()}
				});
		}

		return crow::response(404, "User not found.");
			});

	CROW_ROUTE(m_app, "/players")
		.methods("GET"_method)
		([&connectedUsers]() {
		std::vector<crow::json::wvalue> players_json;

		std::ranges::for_each(connectedUsers, [&players_json](Player& player) {
			// Actualizarea gloanțelor pentru fiecare jucător
			auto& bullets = player.GetBullets(); // Obține gloanțele jucătorului
			if (!bullets.empty()) {
				for (auto& bullet : bullets) {
					const auto& direction = bullet.GetBulletDirection();

					// Actualizează coordonatele glonțului pe baza direcției
					if (direction == "up") {
						bullet.SetCoordY(bullet.GetCoordY() - bullet.GetBulletSpeed());
					}
					else if (direction == "down") {
						bullet.SetCoordY(bullet.GetCoordY() + bullet.GetBulletSpeed());
					}
					else if (direction == "left") {
						bullet.SetCoordX(bullet.GetCoordX() - bullet.GetBulletSpeed());
					}
					else if (direction == "right") {
						bullet.SetCoordX(bullet.GetCoordX() + bullet.GetBulletSpeed());
					}
					log("Bullet coordinates:", bullet.GetCoordX(), bullet.GetCoordY());
				}
			}

			players_json.push_back(crow::json::wvalue{
				{"username", player.GetName()},
				{"isReady",player.IsReady()},
				{"health", player.GetPlayerHealth()},
				{"x", player.GetCoordX()},
				{"y", player.GetCoordY()},
				{"bulletSpeed", player.GetBulletSpeed()},
				{"playerOrientation", player.GetPlayerOrientation()},
				{"bullets", [&bullet_json = player.GetBullets()]() {
					std::vector<crow::json::wvalue> bullets_data;
					for (const auto& bullet : bullet_json) {
						bullets_data.push_back(crow::json::wvalue{
							{"x", bullet.GetCoordX()},
							{"y", bullet.GetCoordY()},
							{"direction", bullet.GetBulletDirection()}
						});
					}
					return bullets_data;
				}()}
				});
			});

		return crow::response{ crow::json::wvalue{
			{"players", players_json}
		} };
			});

	CROW_ROUTE(m_app, "/update-player")
		.methods("POST"_method)
		([&connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		std::string username = body["username"].s();
		int x = body["x"].i();
		int y = body["y"].i();

		auto it = std::find_if(connectedUsers.begin(), connectedUsers.end(),
			[&username](const Player& player) {
				return player.GetName() == username;
			});

		if (it != connectedUsers.end()) {
			it->SetCoordX(x);
			it->SetCoordY(y);
			std::cout << "Updated location for " << username << ": (" << x << ", " << y << ")" << std::endl;
			return crow::response(200, "Player location updated.");
		}

		return crow::response(404, "Player not found.");
			});

	CROW_ROUTE(m_app, "/logout")
		.methods("POST"_method)
		([&connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username")) {
			std::cout << "Invalid request body!" << std::endl;
			return crow::response(400, "Invalid request. Missing username.");
		}

		std::string username = body["username"].s();
		std::cout << "Logout request for user: " << username << std::endl;

		auto it = std::remove_if(connectedUsers.begin(), connectedUsers.end(),
			[&username](const Player& player) { return player.GetName() == username; });

		if (it != connectedUsers.end()) {
			it->SetReady(false);
			connectedUsers.erase(it, connectedUsers.end());
			std::cout << "User logged out: " << username << std::endl;

			return crow::response(200, "User logged out successfully.");
		}

		std::cout << "User not found: " << username << std::endl;
		return crow::response(404, "User not found.");
			});

	CROW_ROUTE(m_app, "/move-player")
		.methods("POST"_method)
		([&connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username") || !body.has("direction")) {
			return crow::response(400, "Invalid request. Missing username or direction.");
		}

		std::string username = body["username"].s();
		std::string direction = body["direction"].s();

		auto it = std::find_if(connectedUsers.begin(), connectedUsers.end(),
			[&username](const Player& player) {
				return player.GetName() == username;
			});

		if (it != connectedUsers.end()) {
			float movementSpeed = it->GetMovementSpeed();

			if (direction == "up") {
				it->SetCoordY(it->GetCoordY() - movementSpeed);
				it->SetPlayerOrientation("up");
			}
			else if (direction == "down") {
				it->SetCoordY(it->GetCoordY() + movementSpeed);
				it->SetPlayerOrientation("down");
			}
			else if (direction == "left") {
				it->SetCoordX(it->GetCoordX() - movementSpeed);
				it->SetPlayerOrientation("left");
			}
			else if (direction == "right") {
				it->SetCoordX(it->GetCoordX() + movementSpeed);
				it->SetPlayerOrientation("right");
			}
			else {
				return crow::response(400, "Invalid direction. Must be one of: up, down, left, right.");
			}

			return crow::response(200, crow::json::wvalue{
				{"username", username},
				{"x", it->GetCoordX()},
				{"y", it->GetCoordY()}
				});
		}
		return crow::response(404, "Player not found.");
			});
	CROW_ROUTE(m_app, "/update-player-status")
		.methods("POST"_method)
		([&connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username") || !body.has("ready")) {
			return crow::response(400, crow::json::wvalue{ {"error", "Invalid request. Missing username or ready status."} });
		}

		std::string username = body["username"].s();
		bool ready = body["ready"].b();

		auto it = std::find_if(connectedUsers.begin(), connectedUsers.end(),
			[&username](const Player& player) {
				return player.GetName() == username;
			});

		if (it != connectedUsers.end()) {
			it->SetReady(ready);

			int readyPlayersCount = std::count_if(connectedUsers.begin(), connectedUsers.end(),
				[](const Player& player) {
					return player.IsReady();
				});

			bool canStartGame = (readyPlayersCount >= 2);
			bool canStartGameV2 = (readyPlayersCount <= 4);

			crow::json::wvalue response;
			response["message"] = "Player status updated successfully.";
			response["canStartGame"] = canStartGame;
			response["canStartGameV2"] = canStartGameV2;
			response["readyPlayers"] = readyPlayersCount;
			return crow::response(200, response);
		}

		return crow::response(404, crow::json::wvalue{ {"error", "Player not found."} });
			});

	CROW_ROUTE(m_app, "/create_bullet")
		.methods("POST"_method)
		([&connectedUsers, &database](const crow::request& req) {
		try {
			std::cout << "Request body: " << req.body << std::endl;

			auto body = crow::json::load(req.body);
			if (!body) {
				return crow::response(400, "Invalid JSON format");
			}

			if (!body.has("username")) {
				return crow::response(400, "Missing required field: username");
			}

			std::string username = body["username"].s();

			auto it = std::find_if(connectedUsers.begin(), connectedUsers.end(), [&username](const Player& player) {
				return player.GetName() == username;
				});

			if (it == connectedUsers.end()) {
				return crow::response(404, "User not found");
			}

			int coordX = it->GetCoordX();
			int coordY = it->GetCoordY();
			int bulletSpeed = it->GetBulletSpeed();
			std::string direction = it->GetPlayerOrientation();

			if (direction == "up") {
				coordY -= 16;
			}
			else if (direction == "down") {
				coordY += 16;
			}
			else if (direction == "left") {
				coordX -= 16;
			}
			else if (direction == "right") {
				coordX += 16;
			}

			Bullet bullet(coordX, coordY, bulletSpeed * 8);
			bullet.SetBulletDirection(direction);
			bullet.SetUser(it->GetName());
			it->addBullet(bullet);

			for (Bullet bullet : it->GetBullets())
			{
				std::cout << '\n' << '\n';
				std::cout << it->GetName() << ": " << bullet.GetCoordX() << ' ' << bullet.GetCoordY() << ' ' << bullet.GetBulletDirection();
				std::cout << '\n' << '\n';
			}

			crow::json::wvalue response_body;
			response_body["message"] = "Bullet created successfully";

			return crow::response(200, response_body);
		}
		catch (const std::exception& e) {
			return crow::response(500, std::string("Error: ") + e.what());
		}
			});

	CROW_ROUTE(m_app, "/delete_bullets")
		.methods("POST"_method)
		([&connectedUsers](const crow::request& req) {
		try {
			std::cout << "Request body: " << req.body << std::endl;

			auto body = crow::json::load(req.body);
			if (!body) {
				return crow::response(400, "Invalid JSON format");
			}

			if (!body.has("username") || !body.has("index")) {
				return crow::response(400, "Missing required fields: username or index");
			}

			std::string username = body["username"].s();
			int index = body["index"].i();

			auto it = std::find_if(connectedUsers.begin(), connectedUsers.end(), [&username](const Player& player) {
				return player.GetName() == username;
				});

			if (it == connectedUsers.end()) {
				return crow::response(404, "User not found");
			}

			it->removeBullet(index);

			crow::json::wvalue response_body;
			response_body["message"] = "Bullets deleted successfully";
			return crow::response(200, response_body);
		}
		catch (const std::exception& e) {
			return crow::response(500, std::string("Error: ") + e.what());
		}
			});

	CROW_ROUTE(m_app, "/change-health")
		.methods("POST"_method)
		([&connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username")) {
			std::cout << "Invalid request body!" << std::endl;
			return crow::response(400, "Invalid request. Missing username.");
		}

		std::string username = body["username"].s();

		auto it = std::find_if(connectedUsers.begin(), connectedUsers.end(), [&username](const Player& player) {
			return player.GetName() == username; });

		if (it != connectedUsers.end()) {
			it->SetPlayerHealth(it->GetPlayerHealth() - 1);
			if (it->GetPlayerHealth() <= 0) {
				it->SetReady(false);
				return crow::response(200, "Player removed");
			}
			else {
				return crow::response(200, "Player health decreased to " + std::to_string(it->GetPlayerHealth()));
			}
		}

		return crow::response(404, "Player not found.");
			});

	CROW_ROUTE(m_app, "/upgrade-bulletCooldown")
		.methods("POST"_method)
		([&database, &connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username")) {
			return crow::response(400, "Invalid request. Missing username.");
		}

		std::string username = body["username"].s();
		auto databaseUsers = database.GetUsers();

		auto it = std::find_if(databaseUsers.begin(), databaseUsers.end(),
			[&username](const User& user) {
				return user.GetUsername() == username;
			});

		if (it != databaseUsers.end()) {
			User& user = *it;

			int points = user.GetPoints();
			if (points < 500) {
				return crow::response(400, "Not enough points for upgrade.");
			}

			if (points >= 500 && user.GetBulletCooldown() < 4) {
				user.SetBulletCooldown(user.GetBulletCooldown() + 1);

				user.SetPoints(points - 500);
				database.SaveUser(user);

				return crow::response(200, crow::json::wvalue{
					{"username", username},
					{"reloadSpeed", user.GetBulletCooldown()},
					{"bulletSpeed", user.GetBulletSpeed()},
					{"score", user.GetPoints()},
					{"scoreWin", user.GetWinningPoints()}
					});
			}
		}

		return crow::response(404, "User not found.");
			});

	CROW_ROUTE(m_app, "/downgrade-bulletCooldown")
		.methods("POST"_method)
		([&database, &connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username")) {
			return crow::response(400, "Invalid request. Missing username.");
		}

		std::string username = body["username"].s();
		auto databaseUsers = database.GetUsers();

		auto it = std::find_if(databaseUsers.begin(), databaseUsers.end(),
			[&username](const User& user) {
				return user.GetUsername() == username;
			});

		if (it != databaseUsers.end()) {
			User& user = *it;

			int score = user.GetPoints();
			if (score < 0) {
				return crow::response(400, "Not enough points for downgrade.");
			}

			if (score >= 0 && user.GetBulletCooldown() > 1) {
				user.SetBulletCooldown(user.GetBulletCooldown() - 1);
				user.SetPoints(score + 500);
				database.SaveUser(user);

				return crow::response(200, crow::json::wvalue{
					{"username", username},
					{"reloadSpeed", user.GetBulletCooldown()},
					{"bulletSpeed", user.GetBulletSpeed()},
					{"score", user.GetPoints()},
					{"scoreWin", user.GetWinningPoints()}
					});
			}
		}

		return crow::response(404, "User not found.");
			});
	CROW_ROUTE(m_app, "/upgrade-bulletSpeed")
		.methods("POST"_method)
		([&database, &connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username")) {
			return crow::response(400, "Invalid request. Missing username.");
		}

		std::string username = body["username"].s();
		auto databaseUsers = database.GetUsers();

		auto it = std::find_if(databaseUsers.begin(), databaseUsers.end(),
			[&username](const User& user) {
				return user.GetUsername() == username;
			});

		if (it != databaseUsers.end()) {
			User& user = *it;

			int score = user.GetWinningPoints();
			if (score < 10) {
				return crow::response(400, "Not enough points for upgrade.");
			}

			if (score >= 10 && user.GetBulletSpeed() < 4) {
				user.SetBulletSpeed(user.GetBulletSpeed() + 1);

				user.SetWinningPoints(score - 10);
				database.SaveUser(user);

				return crow::response(200, crow::json::wvalue{
					{"username", username},
					{"reloadSpeed", user.GetBulletCooldown()},
					{"bulletSpeed", user.GetBulletSpeed()},
					{"score", user.GetPoints()},
					{"scoreWin", user.GetWinningPoints()}
					});
			}
		}

		return crow::response(404, "User not found.");
			});

	CROW_ROUTE(m_app, "/downgrade-bulletSpeed")
		.methods("POST"_method)
		([&database, &connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username")) {
			return crow::response(400, "Invalid request. Missing username.");
		}

		std::string username = body["username"].s();
		auto databaseUsers = database.GetUsers();

		auto it = std::find_if(databaseUsers.begin(), databaseUsers.end(),
			[&username](const User& user) {
				return user.GetUsername() == username;
			});

		if (it != databaseUsers.end()) {
			User& user = *it;

			int score = user.GetWinningPoints();
			if (score < 0) {
				return crow::response(400, "Not enough points for downgrade.");
			}

			if (score >= 0 && user.GetBulletSpeed() > 1) {
				user.SetBulletSpeed(user.GetBulletSpeed() - 1);
				user.SetWinningPoints(score + 10);
				database.SaveUser(user);

				return crow::response(200, crow::json::wvalue{
					{"username", username},
					{"reloadSpeed", user.GetBulletCooldown()},
					{"bulletSpeed", user.GetBulletSpeed()},
					{"score", user.GetPoints()},
					{"scoreWin", user.GetWinningPoints()}
					});
			}
		}

		return crow::response(404, "User not found.");
			});

	CROW_ROUTE(m_app, "/process-walls").methods("POST"_method)([](const crow::request& req) {
		static std::optional<crow::json::wvalue> cachedResponse;

		try {
			if (cachedResponse) {
				return crow::response(200, cachedResponse->dump());
			}

			auto requestJson = crow::json::load(req.body);
			if (!requestJson || !requestJson.has("walls") || requestJson["walls"].t() != crow::json::type::List) {
				return crow::response(400, "Invalid JSON format. Missing 'walls' key or not an array.");
			}

			auto walls = requestJson["walls"];
			if (walls.size() < 3) {
				return crow::response(400, "Not enough walls to select from.");
			}

			std::vector<crow::json::rvalue> wallsVec;
			for (size_t i = 0; i < walls.size(); ++i) {
				wallsVec.push_back(walls[i]);
			}

			std::shuffle(wallsVec.begin(), wallsVec.end(), std::mt19937{ std::random_device{}() });

			crow::json::wvalue responseJson;
			responseJson["selected_walls"] = crow::json::wvalue::list();

			for (size_t i = 0; i < 3 && i < wallsVec.size(); ++i) {
				responseJson["selected_walls"][i] = wallsVec[i];
			}

			cachedResponse = responseJson;

			return crow::response(200, responseJson.dump());
		}
		catch (const std::exception& ex) {
			return crow::response(500, std::string("Error: ") + ex.what());
		}
		});

	CROW_ROUTE(m_app, "/process-roads").methods("POST"_method)([](const crow::request& req) {
		static std::optional<crow::json::wvalue> cachedResponse;

		try {
			if (cachedResponse) {
				return crow::response(200, cachedResponse->dump());
			}

			auto requestJson = crow::json::load(req.body);
			if (!requestJson || !requestJson.has("roads") || requestJson["roads"].t() != crow::json::type::List) {
				return crow::response(400, "Invalid JSON format. Missing 'roads' key or not an array.");
			}

			auto roads = requestJson["roads"];
			if (roads.size() < 5) {
				return crow::response(400, "Not enough roads to select from.");
			}

			std::vector<crow::json::rvalue> roadsVec;
			for (size_t i = 0; i < roads.size(); ++i) {
				roadsVec.push_back(roads[i]);
			}

			std::shuffle(roadsVec.begin(), roadsVec.end(), std::mt19937{ std::random_device{}() });

			crow::json::wvalue responseJson;
			responseJson["selected_roads"] = crow::json::wvalue::list();

			for (size_t i = 0; i < 5 && i < roadsVec.size(); ++i) {
				responseJson["selected_roads"][i] = roadsVec[i];
			}
			cachedResponse = responseJson;

			return crow::response(200, responseJson.dump());
		}
		catch (const std::exception& ex) {
			return crow::response(500, std::string("Error: ") + ex.what());
		}
		});

	CROW_ROUTE(m_app, "/generate_map").methods("POST"_method)([](const crow::request& req) {
		static std::optional<int> cachedRandomNumber;

		try {
			auto requestJson = crow::json::load(req.body);

			if (!requestJson || !requestJson.has("nr") || requestJson["nr"].t() != crow::json::type::Number) {
				return crow::response(400, "Invalid JSON format. Missing 'nr' key or not an integer.");
			}

			int nr = requestJson["nr"].i();

			if (!cachedRandomNumber) {
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> dis(1, nr);

				cachedRandomNumber = dis(gen);
			}

			crow::json::wvalue responseJson;
			responseJson["random_number"] = *cachedRandomNumber;

			return crow::response(200, responseJson.dump());
		}
		catch (const std::exception& ex) {
			return crow::response(500, std::string("Error: ") + ex.what());
		}
		});

	CROW_ROUTE(m_app, "/update-map")
		.methods(crow::HTTPMethod::Get, crow::HTTPMethod::Post)([](const crow::request& req) {
		static std::pair<int, int> lastMapUpdate = { -1, -1 };
		static std::string lastTileType = "";

		if (req.method == crow::HTTPMethod::Post) {
			try {
				auto jsonBody = nlohmann::json::parse(req.body);
				int row = jsonBody.at("row").get<int>();
				int col = jsonBody.at("col").get<int>();
				std::string tileType = jsonBody.at("tileType").get<std::string>();

				lastMapUpdate = { row, col };
				lastTileType = tileType;

				return crow::response(200, "Update received successfully");
			}
			catch (const std::exception& e) {
				return crow::response(400, std::string("Invalid request body: ") + e.what());
			}
		}
		else if (req.method == crow::HTTPMethod::Get) {
			if (lastMapUpdate.first == -1 && lastMapUpdate.second == -1) {
				return crow::response(204, "No updates available");
			}

			nlohmann::json responseJson;
			responseJson["row"] = lastMapUpdate.first;
			responseJson["col"] = lastMapUpdate.second;
			responseJson["tileType"] = lastTileType;

			return crow::response(200, responseJson.dump());
		}
		return crow::response(405, "Method Not Allowed");
			});
	CROW_ROUTE(m_app, "/update-points")
		.methods("POST"_method)
		([&database, &connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username")) {
			return crow::response(400, "Invalid request. Missing username.");
		}

		std::string username = body["username"].s();

		auto player_it = std::find_if(connectedUsers.begin(), connectedUsers.end(),
			[&username](const Player& p) {
				return p.GetName() == username;
			});

		if (player_it != connectedUsers.end()) {
			Player& player = *player_it;
			int currentPoints = player.GetPoints();
			int updatedPoints = currentPoints + 200;

			player.SetPoints(updatedPoints);

			auto databaseUsers = database.GetUsers();
			auto user = std::ranges::find_if(databaseUsers, [&username](const User& user) {
				return user.GetUsername() == username;
				});
			if (user != databaseUsers.end()) {
				user->SetPoints(updatedPoints);
				database.SaveUser(*user);
			}

			return crow::response(200, crow::json::wvalue{
				{"username", username},
				{"points", updatedPoints}
				});
		}

		return crow::response(404, "Player not found.");
			});
	CROW_ROUTE(m_app, "/update-winningpoints")
		.methods("POST"_method)
		([&database, &connectedUsers](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body || !body.has("username")) {
			return crow::response(400, "Invalid request. Missing username.");
		}

		std::string username = body["username"].s();

		auto player_it = std::find_if(connectedUsers.begin(), connectedUsers.end(),
			[&username](const Player& p) {
				return p.GetName() == username;
			});

		if (player_it != connectedUsers.end()) {
			Player& player = *player_it;
			int currentWinningPoints = player.GetWinningPoints();
			int updatedWinningPoints = currentWinningPoints + 2;

			player.SetWinningPoints(updatedWinningPoints);

			auto databaseUsers = database.GetUsers();
			auto user = std::ranges::find_if(databaseUsers, [&username](const User& user) {
				return user.GetUsername() == username;
				});
			if (user != databaseUsers.end()) {
				user->SetWinningPoints(updatedWinningPoints);
				database.SaveUser(*user);
			}

			return crow::response(200, crow::json::wvalue{
				{"username", username},
				{"winningpoints", updatedWinningPoints}
				});
		}

		return crow::response(404, "Player not found.");
			});

	m_app.port(18080).multithreaded().run();
}