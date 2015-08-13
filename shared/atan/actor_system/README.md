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

const actor_ref get_actor(std::string actor_name);

template<typename T>
std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, long initial_delay_ms, long interval_ms) const;

template<typename T>
std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms, long interval_ms) const;

template<typename T>
std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, long initial_delay_ms) const;

template<typename T>
std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms) const;

const std::shared_ptr<udp_server> get_server();
const std::shared_ptr<scheduler> get_scheduler();
const std::shared_ptr<dispatcher> get_dispatcher();
const std::string system_name();

bool started();
bool stopped();
```
