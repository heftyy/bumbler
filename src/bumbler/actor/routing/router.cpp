#include "router.h"
#include "router_pool.h"
#include "../../messages/message.h"

namespace bumbler {

void router::stop_actor(bool wait) {
    router_pool_->stop(wait);
}

void router::set_router_pool(std::unique_ptr<router_pool> pool) {
    this->router_pool_ = std::move(pool);
}

void router::tell(std::unique_ptr<message> msg) {
    this->router_pool_->tell(std::move(msg));
}

}
