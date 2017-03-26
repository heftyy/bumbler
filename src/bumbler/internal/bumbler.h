#pragma once

#include <functional>

namespace boost { class any; }

namespace bumbler {

enum class stop_mode {
    IGNORE_QUEUE, WAIT_FOR_QUEUE
};

using ResponseFun = std::function<void(const boost::any&)>;
using DispatchFun = std::function<int(class local_actor&)>;
using ActorCreateFun = std::function<std::unique_ptr<class abstract_actor>(const std::shared_ptr<class actor_system>& system, const std::string& name)>;
using TypedActorCreateFun = std::function<std::unique_ptr<class untyped_actor>(void)>;
using RouterPoolCreateFun = std::function<std::unique_ptr<class router_pool>(void)>;
using MailboxCreateFun = std::function<std::unique_ptr<class mailbox>(void)>;

}

