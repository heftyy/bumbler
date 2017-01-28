# actor_system

 * holds a set of actors created in the actor_system
 * has a scheduler which you can use to run tasks with a delay and/or an interval
 * has a dispatcher ( thread_pool ) 

```c++
static std::shared_ptr<actor_system> create_system(const std::string& name, int port, int thread_pool_size = 5);

/*
 * if wait is true the dispatcher will wait for the all tasks to finish and actors in the system will
 * go through all of their messages
 *
 * if wait is false system exits as soon as possible ignoring messages in the queue
 */
void stop(bool wait = false);

int stop_actor(std::string actor_name, bool wait = false);
int tell_actor(std::unique_ptr<message> msg, bool from_remote = false);
int ask_actor(std::unique_ptr<message> msg, const ResponseFun& response_fn);

const actor_ref get_actor_ref(std::string actor_name);
std::unique_ptr<actor_channel> get_actor_channel(const std::string& actor_name);

template<typename Props, typename ...ActorArgs>
actor_ref actor_of(Props&& props, ActorArgs&&... actor_args);

template<typename T>
std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, long initial_delay_ms, long interval_ms = 0) const;

template<typename T>
std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms, long interval_ms = 0) const;

template<typename T>
std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, long initial_delay_ms = 0) const;

template<typename T>
std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms = 0) const;

const std::shared_ptr<udp_server> get_server() const;
const std::shared_ptr<scheduler> get_scheduler() const;
const std::shared_ptr<dispatcher> get_dispatcher() const;
const std::string system_name() const;

bool started();
bool stopped();
```
