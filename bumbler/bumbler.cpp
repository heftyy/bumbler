// bumbler.cpp : Defines the entry point for the console application.
//

#include <atan/actor_system/actor_system.h>
#include <wx/wx.h>
#include "src/bumbler_client_actor.h"

int main(int argc, char* argv[])
{
	actor_system actor_system("client_system", 8558);

	std::string address = "127.0.0.1:8556";
	std::string rpi_actor_ref = "server_actor$bumbler_server@" + address;

	auto client_actor = std::shared_ptr<bumbler_client_actor>(new bumbler_client_actor(rpi_actor_ref, actor_system));
	actor_system.add_actor(client_actor);

	client_actor->tell(1, "hello");

	actor_system.stop();

	return 0;
}

