// server.cpp : Defines the entry point for the console application.
//

#include <atan/actor_system/actor_system.h>
#include <communication/serializable_types.h>
#include <atan/actor/routing/round_robin_router.h>
#include "out_router.h"

int main(int argc, char *argv[]) {
    try {
        std::shared_ptr<actor_system> system = actor_system::create_system("server_system", 4445);

        actor_ref l_router = round_robin_router::create<out_router>("out_router", system, 5);

        std::string input;
        while (1) {
            std::cin >> input;
            if (input.compare("quit") == 0) {
                break;
            }
        }

        system->stop(true);
    }
    catch (std::runtime_error& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

