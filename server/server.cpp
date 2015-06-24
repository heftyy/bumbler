// server.cpp : Defines the entry point for the console application.
//

#include <atan/actor_system/actor_system.h>
#include "src/server_actor.h"
#include "../shared/communication/serializable_types.h"

int main(int argc, char *argv[]) {
    try {
        std::shared_ptr<actor_system> system = std::shared_ptr<actor_system>(new actor_system("server_system", 4445));
        system->init();

        actor_ref local_actor = actor::create_actor<server_actor>("server_print_actor", system);

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

