#include <atan/actor_system/actor_system.h>
#include "out_actor.h"

int main(int argc, char* argv[])
{
//    system_a s;

    std::shared_ptr<actor_system> system = std::shared_ptr<actor_system>(new actor_system("bumbler_system", 4444));
    system->init();

	actor_ref local_actor = actor::create_actor<out_actor>("out_actor", system);

    std::string msg = "message";
    local_actor.tell(int(5));

//    message<std::string> msg(local_actor, actor_ref::none(), "MESSAGE FROM SCHEDULER", 1);
//    std::shared_ptr<cancellable> cancellable = system->schedule(msg, 500, 200);

    std::chrono::milliseconds sleep_duration(1000);
    std::this_thread::sleep_for(sleep_duration);

//    cancellable->cancel();

    system->stop();

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

