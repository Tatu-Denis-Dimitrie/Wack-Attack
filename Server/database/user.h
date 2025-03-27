#pragma once
#include <string>
#include <sqlite_orm/sqlite_orm.h>

namespace sql = sqlite_orm;

class User {
public:
	User() = default;
	User(const std::string& username, int points, int winningPoints, float bulletSpeed, float bulletCooldown);

	int GetId() const;
	void SetId(int id);

	std::string GetUsername() const;
	void SetUsername(const std::string& username);

	int GetPoints() const;
	void SetPoints(int points);

	int GetWinningPoints() const;
	void SetWinningPoints(int winningPoints);

	int GetBulletSpeed() const;
	void SetBulletSpeed(int bulletSpeed);

	double GetBulletCooldown() const;
	void SetBulletCooldown(float bulletCooldown);

	static auto table() {
		return sql::make_table(
			"Users",
			sql::make_column("id", &User::m_id, sql::primary_key().autoincrement()),
			sql::make_column("username", &User::m_username),
			sql::make_column("points", &User::m_points),
			sql::make_column("winningPoints", &User::m_winningPoints),
			sql::make_column("bulletSpeed", &User::m_bulletSpeed),
			sql::make_column("bulletCooldown", &User::m_bulletCooldown)
		);
	}

private:
	int m_id;
	std::string m_username;
	int m_points;
	int m_winningPoints;
	int m_bulletSpeed;
	double m_bulletCooldown;
};
