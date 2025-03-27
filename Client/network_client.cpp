#include "network_client.h"
#include "player.h"

void sendLoginRequest(const std::string& username)
{
	cpr::Response response = cpr::Post(
		cpr::Url{ SERVER_URL + "/login" },
		cpr::Header{ {"Content-Type", "application/json"} },
		cpr::Body{ "{\"username\": \"" + username + "\"}" });

	if (response.status_code == 200) {
		std::cout << "Login request successful: " << response.text << std::endl;
	}
	else {
		std::cerr << "Login request failed: " << response.status_code << " - " << response.text << std::endl;
	}
}

void sendMoveRequest(const std::string& username, const std::string& direction) {
	cpr::Response response = cpr::Post(
		cpr::Url{ SERVER_URL + "/move-player" },
		cpr::Header{ {"Content-Type", "application/json"} },
		cpr::Body{ "{\"username\": \"" + username + "\", \"direction\": \"" + direction + "\"}" });

	if (response.status_code == 200) {
		std::cout << "Send move request successful " << response.text << std::endl;
	}
	else {
		std::cerr << "Failed to send move request. Status: " << response.status_code << ", Message: " << response.text << std::endl;
	}
}

void sendCreateBulletRequest(const std::string& username)
{
	std::string body = "{ \"username\": \"" + username + "\" }";
	std::cout << "JSON body sent: " << body << std::endl;

	cpr::Response response = cpr::Post(
		cpr::Url{ SERVER_URL + "/create_bullet" },
		cpr::Header{ { "Content-Type", "application/json" } },
		cpr::Body{ body });

	if (response.status_code == 200) {
		std::cout << "Bullet creation successful: " << response.text << std::endl;
	}
	else {
		std::cerr << "Bullet creation failed: " << response.status_code << " - " << response.text << std::endl;
	}
}

void sendDeleteBulletRequest(const std::string& username, int index)
{
	cpr::Response response = cpr::Post(
		cpr::Url{ SERVER_URL + "/delete_bullets" },
		cpr::Header{ { "Content-Type", "application/json" } },
		cpr::Body{ "{ \"username\": \"" + username + "\", \"index\": " + std::to_string(index) + " }" });

	if (response.status_code == 200) {
		std::cout << "Bullet delete successful: " << response.text << std::endl;
	}
	else {
		std::cerr << "Bullet delete failed: " << response.status_code << " - " << response.text << std::endl;
	}
}

void sendGenerateMapRequest(int& number)
{
	nlohmann::json requestJson;
	requestJson["nr"] = number;

	cpr::Response response = cpr::Post(
		cpr::Url{ SERVER_URL + "/generate_map" },
		cpr::Header{ { "Content-Type", "application/json" } },
		cpr::Body{ requestJson.dump() });

	if (response.status_code == 200) {
		auto jsonResponse = nlohmann::json::parse(response.text);
		int randomNumber = jsonResponse["random_number"];
		std::cout << "Received random number: " << randomNumber << std::endl;
		number = randomNumber;
	}
	else {
		std::cerr << "Failed to communicate with server. HTTP code: " << response.status_code
			<< ", Message: " << response.text << std::endl;
	}
}

void sendChangeHealthRequest(const std::string& username)
{
	std::string body = "{ \"username\": \"" + username + "\" }";
	std::cout << "JSON body sent: " << body << std::endl;

	cpr::Response response = cpr::Post(
		cpr::Url{ SERVER_URL + "/change-health" },
		cpr::Header{ { "Content-Type", "application/json" } },
		cpr::Body{ body });

	if (response.status_code == 200) {
		std::cout << "Health Change done " << response.text << std::endl;
	}
	else {
		std::cerr << "Health Change failed: " << response.status_code << " - " << response.text << std::endl;
	}
}

UserStats fetchUserScoreFromServer(const std::string& username) {
	const std::string url = SERVER_URL + "/connected-user/score";
	cpr::Response response = cpr::Post(
		cpr::Url{ url },
		cpr::Body{ R"({"username":")" + username + R"("})" },
		cpr::Header{ {"Content-Type", "application/json"} }
	);

	if (response.status_code == 200) {
		auto json_response = crow::json::load(response.text);
		if (json_response && json_response.has("points") && json_response.has("winningPoints") && json_response.has("bulletSpeed") && json_response.has("bulletCooldown")) {
			int points = json_response["points"].i();
			int winningPoints = json_response["winningPoints"].i();
			int bulletSpeed = json_response["bulletSpeed"].i();
			int bulletCooldown = json_response["bulletCooldown"].i();
			return { points, winningPoints, bulletSpeed, bulletCooldown };
		}
	}

	throw std::runtime_error("Failed to points. Status code: " + std::to_string(response.status_code));
}