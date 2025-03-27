#include "database.h"

Database::Database(const std::string& db_name)
	: m_db(MakeStorage(db_name)) {
	init();
}

void Database::init()
{
	m_db.sync_schema();
}

std::vector<User> Database::GetUsers()
{
	return m_db.get_all<User>();
}

void Database::SaveUser(const User& user)
{
	auto existing_users = GetUsers();
	bool user_exists = false;

	for (const auto& existing_user : existing_users) {
		if (existing_user.GetUsername() == user.GetUsername()) {
			user_exists = true;
			break;
		}
	}

	if (user_exists) {
		m_db.replace(user);
	}
	else {
		m_db.insert(user);
	}
}