#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sqlite_orm/sqlite_orm.h>
#include "../database/user.h"

namespace sql = sqlite_orm;

inline auto MakeStorage(const std::string& db_name) {
	return sql::make_storage(
		db_name,
		User::table()
	);
}

using Storage = decltype(MakeStorage(""));

class Database
{
public:
	Database(const std::string& db_name);
	~Database() = default;
	void init();
	void SaveUser(const User& user);
	std::vector<User> GetUsers();
private:
	Storage m_db;
	friend class Bullet;
};
