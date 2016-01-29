#include "router.h"

namespace bumbler {

void router::stop_actor(bool wait) {
    router_pool_->stop(wait);
    abstract_actor::stop_actor(wait);
}

void router::set_router_pool(std::unique_ptr<router_pool> pool) {
    this->router_pool_ = std::move(pool);
}

void router::tell(std::unique_ptr<message> msg, bool remote) {
    this->router_pool_->tell(std::move(msg));
}

}
