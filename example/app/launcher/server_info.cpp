#include "my_app.h"

server_info::server_info(std::string ip, int port, std::shared_ptr<actor_system> actor_system) : port_(port), ip_(ip) {
    boost::uuids::basic_random_generator<boost::mt19937> gen;
    boost::uuids::uuid uuid = gen();
    std::string uuid_string = boost::uuids::to_string(uuid);

    std::string rpi_actor_ref = "server_actor$bumbler_server@" + ip + ":" + std::to_string(port);
    server_connection_actor_ = actor::create_actor<server_connection_actor>(
            "SERVER_CONNECTION_ACTOR_" + uuid_string,
            rpi_actor_ref, actor_system,
            [this](std::string ip) {
                MyApp::frame_servers->UpdateServer(ip, to_string());
            });
}
