#include <atan/actor_system/actor_system.h>
#include <atan/interruptible_thread.h>
#include <logger/logger.h>
#include "out_actor.h"

int main(int argc, char* argv[])
{
//    system_a s;

    std::shared_ptr<actor_system> system = std::shared_ptr<actor_system>(new actor_system("bumbler_system", 4444));
    system->init();

	actor_ref local_actor = actor::create_actor<out_actor>("out_actor", system, 10);

    local_actor.tell(1, "THIS IS A MESSAGE", actor_ref::none());

    message msg(local_actor, actor_ref::none(), "MESSAGE FROM SCHEDULER", 1);
    system->schedule_once(msg, 500);

    std::chrono::milliseconds sleep_duration(1000);
    std::this_thread::sleep_for(sleep_duration);

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

