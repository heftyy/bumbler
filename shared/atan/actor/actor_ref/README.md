# actor_ref

tell
---
```c++
void tell(char* data, actor_ref sender = actor_ref::none());

template<typename T>
void tell(T&& data, actor_ref sender = actor_ref::none()) const;

template<typename T>
void tell(const typed_message<T>& msg) const;
```

Tell method will send a message to the actor.

There are 3 special types of messages:
 * broadcast - used for routers, send messages to all actors in the router
 * stop_actor - stops the actor after all the messages in the queue have been read and tasks are finished. Sending this message will synchrnously wait for the actor to stop.
 * kill_actor - clears the message queue and stops the actor as soon as possible. Sending this message will synchrnously wait for the actor to stop.
```c++
actor_ref.tell(broadcast<int>(88));
actor_ref.tell(stop_actor<int>(5));
actor_ref.tell(kill_actor<int>(5));
```
Current data send in stop_actor and kill_actor messages is ignored.

Futures
---
Futures are used to synchronously wait for the reply from an actor. Futures work with all actor types.

In the case below test_actor will reply with std::string when it receives an int.

```c++
auto system1 = actor_system::create_system("test_system1", 4555);
const actor_ref test_actor_ref1 = local_actor::create<test_actor>("test_actor1", system1);

std::future<std::string> f1 = test_actor_ref1.future<std::string>(1);
auto status1 = f1.wait_for(std::chrono::seconds(1));
```
