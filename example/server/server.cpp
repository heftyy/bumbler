// server.cpp : Defines the entry point for the console application.
//

#include <bumbler/actor_system/actor_system.h>
#include <communication/serializable_types.h>
#include <bumbler/actor/routing/round_robin_pool.h>
#include <bumbler/actor/props/typed_props.h>
#include "out_router.h"

int main(int argc, char *argv[]) {
    try {
        auto system1 = actor_system::create_system("server_system", 4445);

        auto p = typed_props<router, out_router>();
        p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(2);

        auto l_router = system1->actor_of(p, "test_actor1");

        std::string input;
        while (1) {
            std::cin >> input;
            if (input.compare("quit") == 0) {
                break;
            }
        }

        system1->stop(stop_mode::WAIT_FOR_QUEUE);
    }
    catch (std::runtime_error& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

