#include <boost/log/trivial.hpp>
#include <atan/actor_system/actor_system.h>
#include <communication/serializable_types.h>

#include "out_actor.h"
#include "remote_server_actor.h"

int main(int argc, char *argv[]) {

	std::shared_ptr<actor_system> system = std::make_shared<actor_system>("bumbler_system", 4444);
	system->init();

	actor_ref l_actor = actor::create_actor<out_actor>("out_actor", system);

    std::string r_actor_name = "remote_server_actor";
    std::string r_actor_location = "out_router$server_system@127.0.0.1:4445";

    actor_ref r_actor = actor::create_actor<remote_server_actor>(r_actor_name, r_actor_location, system);

    std::string msg = "BLAM";

    std::future<std::string> f = r_actor.future<std::string>(5);

    if(f.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
        BOOST_LOG_TRIVIAL(debug) << "future result = " << f.get();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    actor_ref temp = system->get_actor("temp/a");

    BOOST_LOG_TRIVIAL(debug) << "temp/a is none ? " << temp.is_none();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    system->stop(true);

    BOOST_LOG_TRIVIAL(info) << "EXIT";

    return 0;
}
