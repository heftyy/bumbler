#include <boost/log/trivial.hpp>
#include <atan/actor_system/actor_system.h>
#include <communication/serializable_types.h>
#include <logger/logger.h>

#include "out_actor.h"
#include "remote_server_actor.h"

int main(int argc, char *argv[]) {

	std::shared_ptr<actor_system> system = actor_system::create_system("actor_system", 4444);

	actor_ref l_actor = actor::create_actor<out_actor>("out_actor", system);

    std::string r_actor_name = "remote_server_actor";
    std::string r_actor_location = "out_router$server_system@localhost:4445";

    actor_ref r_actor = actor::create_actor<remote_server_actor>(r_actor_name, r_actor_location, system);

    std::string msg = "BLAM";

    std::future<std::string> f = l_actor.future<std::string>(5);

    if(f.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
        DEBUG << "future result = " << f.get();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    actor_ref temp = system->get_actor("temp/a");

    DEBUG << "temp/a is none ? " << temp.is_none();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    system->stop(true);

    INFO << "EXIT";

    return 0;
}
