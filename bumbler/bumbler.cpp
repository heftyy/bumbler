#include <map>
#include <iostream>
#include <thread>

#include "out_actor.h"

using namespace std;

int main(int argc, char* argv[])
{
    std::shared_ptr<actor_system> system = std::shared_ptr<actor_system>(new actor_system("bumbler_system", 4444));
    system->init();

    std::cout << actor_system_storage::instance().get_system("bumbler_system")->system_name() << std::endl;

	actor_ref local_actor = actor::create_actor<out_actor>("out_actor", system, 10);

    local_actor.tell(1, "THIS IS A MESSAGE", actor_ref::none());

    std::chrono::milliseconds sleep_duration(1000);
    std::this_thread::sleep_for(sleep_duration);

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

