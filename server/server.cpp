// server.cpp : Defines the entry point for the console application.
//

#include <atan/actor_system/actor_system.h>
#include <communication/serializable_types.h>
#include "out_router.h"

int main(int argc, char *argv[]) {
    try {
        std::shared_ptr<actor_system> system = std::make_shared<actor_system>("server_system", 4445);
        system->init();

        actor_ref l_router = actor::create_router<out_router>("out_router", system);

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

