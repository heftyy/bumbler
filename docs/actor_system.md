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
void stop(stop_mode stop_mode = stop_mode::IGNORE_QUEUE);
int stop_actor(const identifier& actor_name, stop_mode stop_mode = stop_mode::IGNORE_QUEUE);

int tell_actor(std::unique_ptr<message> msg);

bool has_actor(const identifier& actor_ident);
actor_ref get_actor_ref(const identifier& actor_name);
std::unique_ptr<actor_channel> get_actor_channel(const identifier& actor_name);

size_t actor_mailbox_size(const identifier& actor_ident);

template<typename Props, typename ...ActorArgs>
actor_ref actor_of(Props&& props, ActorArgs&&... actor_args);
void add_actor(std::unique_ptr<abstract_actor> actor);

template<typename T>
std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, long initial_delay_ms, long interval_ms = 0) const;

template<typename T>
std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms, long interval_ms = 0) const;

template<typename T>
std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, long initial_delay_ms = 0) const;

template<typename T>
std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms = 0) const;

std::future<int> dispatch(const DispatchFun& dispatch_fun, class local_actor& actor);

void send_data(const std::string& data, const boost::asio::ip::udp::endpoint& target_endpoint);

const identifier system_key() const;

bool started();
bool stopped();
```
