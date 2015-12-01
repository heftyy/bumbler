#include "server_status.h"

std::string server_status::to_string() {
    return name + "\t" + std::to_string(players_online) + "/" + std::to_string(players_max) + "\t" +
           std::to_string(ping);
}
