#include <boost/log/trivial.hpp>
#include <atan/actor_system/actor_system.h>

#include "out_actor.h"
#include "out_router.h"
#include "remote_server_actor.h"

int main(int argc, char *argv[]) {

	std::shared_ptr<actor_system> system = std::shared_ptr<actor_system>(new actor_system("bumbler_system", 4444));
	system->init();

	actor_ref l_actor = actor::create_actor<out_actor>("out_actor", system);

    std::string r_actor_name = "remote_server_actor";
    std::string r_actor_location = "server_print_actor$server_system@127.0.0.1:4445";

//    actor_ref r_actor = actor::create_actor<remote_server_actor>(r_actor_name, r_actor_location, system);

    std::string msg = "BLAM";

    l_actor.tell(msg);
    l_actor.kill();

    actor_ref l_router = actor::create_router<out_router>("out_router", system);
    actor_ref sender = actor_ref::none();

//    l_router.tell(broadcast<std::string>(l_router, sender, msg));

    l_router.kill();

    BOOST_LOG_TRIVIAL(debug) << "router exists = " << l_router.exists();

    /*
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    BOOST_LOG_TRIVIAL(debug) << "ROUTER DONE";

    for(int i = 0; i < 1000; i++) {
        l_actor.tell(msg);
    }

    BOOST_LOG_TRIVIAL(debug) << "ACTOR DONE";
     */

//    r_actor.tell(1);
//    r_actor.tell("aaaaa");

//    r_actor.tell(3.14f, l_actor);

//    r_actor.tell(3.14f);
//    system->schedule(msg, l_actor, r_actor, 500, 900);

    std::chrono::milliseconds sleep_duration(2000);
    std::this_thread::sleep_for(sleep_duration);

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
