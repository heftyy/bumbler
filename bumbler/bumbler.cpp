#include <boost/log/trivial.hpp>
#include <atan/actor_system/actor_system.h>

#include "out_actor.h"
#include "remote_server_actor.h"

int main(int argc, char *argv[]) {

	std::shared_ptr<actor_system> system = std::shared_ptr<actor_system>(new actor_system("bumbler_system", 4444));
	system->init();

	actor_ref l_actor = actor::create_actor<out_actor>("out_actor", system);

    std::string r_actor_name = "remote_server_actor";
    std::string r_actor_location = "server_print_actor$server_system@localhost:4445";

	actor_ref r_actor = actor::create_actor<remote_server_actor>(r_actor_name, r_actor_location, system);

	std::string msg = "message";
    l_actor.tell(5);

    r_actor.tell("foo");

	//    message<std::string> msg(local_actor, actor_ref::none(), "MESSAGE FROM SCHEDULER", 1);
	//    std::shared_ptr<cancellable> cancellable = system->schedule(msg, 500, 200);

	std::chrono::milliseconds sleep_duration(1000000);
	std::this_thread::sleep_for(sleep_duration);

	//    cancellable->cancel();

	system->stop(true);

	BOOST_LOG_TRIVIAL(info) << "EXIT";

	return 0;

	/*
	wxApp::SetInstance(new MyApp());
	wxEntryStart(argc, argv);
	wxTheApp->OnInit();

	std::cout << "bumbler test" << std::endl;

	wxTheApp->OnRun();
	wxTheApp->OnExit();
	wxEntryCleanup();

	return 0;
	*/
}
