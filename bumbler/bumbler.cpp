#include <boost/log/trivial.hpp>
#include <atan/actor_system/actor_system.h>
#include <communication/serializable_types.h>

#include "out_actor.h"
#include "out_router.h"
#include "remote_server_actor.h"
#include <atan/messages/commands/commands.h>


int main(int argc, char *argv[]) {

	std::shared_ptr<actor_system> system = std::shared_ptr<actor_system>(new actor_system("bumbler_system", 4444));
	system->init();

	actor_ref l_actor = actor::create_actor<out_actor>("out_actor", system);

    std::string r_actor_name = "remote_server_actor";
    std::string r_actor_location = "out_router$server_system@127.0.0.1:4445";

    std::string msg = "BLAM";

    actor_ref r_actor = actor::create_actor<remote_server_actor>(r_actor_name, r_actor_location, system);
//    for(int i = 0; i < 20; i++) r_actor.tell(msg);

//    actor_ref sender = actor_ref::none();
//    r_actor.tell(broadcast<std::string>(r_actor, sender, msg));

    actor_ref l_router = actor::create_router<out_router>("out_router", system);
    actor_ref sender = actor_ref::none();
//    typed_message<std::string> tm(l_router, sender, broadcast<std::string>("BLAM"));

//    l_router.tell(broadcast<std::string>(l_router, r_actor, msg));

    l_actor.tell("KAPOW", l_router);

//    l_router.tell(broadcast<std::string>("Aaaaa"));
//    l_router.tell(stop_actor<std::string>("DIE"));

//    r_actor.tell(broadcast<std::string>("BRRRR"), l_router);

    BOOST_LOG_TRIVIAL(debug) << "BEFORE SLEEP";

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
