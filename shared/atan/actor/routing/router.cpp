#include "router.h"
#include "../../actor_system/actor_system.h"

void router::tell(std::unique_ptr<message> msg, bool remote) {
    this->router_pool_->tell(std::move(msg));
}
