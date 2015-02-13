// server.cpp : Defines the entry point for the console application.
//

#include <atan/actor_system/actor_system.h>
#include "src/server_actor.h"

int main(int argc, char* argv[])
{
	try
	{
		actor_system actor_system("bumbler_server", 8556);
        actor_system.setup();
		auto actor = std::shared_ptr<server_actor>(new server_actor(actor_system));
		actor_system.add_actor(actor);

		std::string input;
		while (1)
		{
			std::cin >> input;
			if (input.compare("quit") == 0)
			{
				break;
			}
		}

		actor_system.stop();
	}
	catch (std::runtime_error &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}

