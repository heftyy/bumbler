#include <boost/log/trivial.hpp>
#include <atan/actor_system/actor_system.h>
#include <communication/serializable_types.h>
#include <logger/logger.h>

#include "out_actor.h"
#include "remote_server_actor.h"

int main(int argc, char *argv[]) {

	std::shared_ptr<actor_system> system = actor_system::create_system("actor_system", 4444);

    auto props_local = typed_props<local_actor, out_actor>();
    auto props_remote = typed_props<remote_actor, remote_server_actor>();
    props_remote.with_network_actor("test_actor1$actor_system@localhost:4444");

    const actor_ref la1 = system->actor_of(props_local, "test_actor1");
    const actor_ref ra1 = system->actor_of(props_remote, "remote_test_actor1");

    std::future<std::string> f = la1.ask<std::string>(5);

    if(f.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
        INFO << "future result = " << f.get();
    }

    f = ra1.ask<std::string>(6);

    if(f.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
        INFO << "future result = " << f.get();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    system->stop(true);

    INFO << "EXIT";

    return 0;
}
