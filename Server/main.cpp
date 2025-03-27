#include <iostream>
#include "routing.h"
#include "database/database.h"

int main()
{
	Database database("battle_city.sqlite");
	Routing Server;
	Server.Run(database);
	return 0;
}