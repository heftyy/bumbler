#include <boost/log/trivial.hpp>
#include <bumbler/messages/message.h>
#include <bumbler/actor_system/actor_system.h>
#include <bumbler/actor/props/typed_props.h>
#include <bumbler/logger/logger.h>
#include <communication/serializable_types.h>

#include "out_actor.h"
#include "remote_server_actor.h"

int main(int argc, char *argv[]) {

	auto system = actor_system::create_system("actor_system", 4444);

    auto props_local = bumbler::typed_props<local_actor, out_actor>();

    auto la1 = system->actor_of(props_local, "test_actor1");
    auto ra1 = actor_ref("test_actor1$actor_system@localhost:4444");

    std::future<std::string> f = la1.ask<std::string>(5);

    if(f.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
        INFO << "future result = " << f.get();
    }

    f = ra1.ask<std::string>(6);

    if(f.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
        INFO << "future result = " << f.get();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    system->stop(stop_mode::WAIT_FOR_QUEUE);

    INFO << "EXIT";

    return 0;
}
