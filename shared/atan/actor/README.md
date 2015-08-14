# actors

untyped_actor
---
This is the class users will have to override to create actors. The most import method is
```c++
virtual void on_receive(boost::any data) = 0;
```
All messages received by the actor will land here. You can also override on_error go get the message and the exception
when one was thrown in on_receive
```c++
virtual void on_error(boost::any data, std::exception ex);
```

You can find the type of the message by calling
```c++
template<typename T>
bool is_type(boost::any& data);
```
and then cast it with
```c++
template<typename T>
T cast_message(boost::any& data);
```

Reply can be used to send a message back from the actor, there are 2 overloads for reply
```c++
void reply(const char* data);

template<typename T>
void reply(T&& data);
```

The last 2 important methods are for getting the sender and getting self, they will return actor_ref objects.
```c++
actor_ref get_self();
actor_ref get_sender();
```

A simple example of an untyped_actor
```c++
class test_actor : public untyped_actor {
public:
    test_actor() {}

    ~test_actor() {}
protected:
    void on_receive(boost::any data) {
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] received message from " << get_sender().actor_name;

        if(is_type<int>(data)) {
            int in = cast_message<int>(data);
            BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] message was " << in;
            reply("BLAM");
        }

        if(is_type<float>(data)) {
            float in = cast_message<float>(data);
            BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] message was " << in;
        }

        if(is_type<std::string>(data)) {
            std::string in = cast_message<std::string>(data);
            BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] message was " << in;
            reply(::stop_actor<std::string>("DIE!"));
        }

        //do some calculations...
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
};
```

Creating different actor types
---

 * local_actor - a regular actor type. Each local actor has a lightweight thread that reads incoming messages and runs the tasks in the dispatcher supplied by the actor_system.
```c++
auto system1 = actor_system::create_system("test_system1", 4555);
const actor_ref la1 = local_actor::create<test_actor>("test_actor1", system1);
```
 * remote_actor - used to send messages to a remote application. Messages send to this actor have their target replaced with the network address of the actor it points to. Each remote actor has a lightweight thread that reads incoming messages from the network and runs the tasks in the dispatcher supplied by the actor_system.
```c++
auto system1 = actor_system::create_system("test_system1", 4555);
auto system2 = actor_system::create_system("test_system2", 4556);

const actor_ref test_actor_ref1 = local_actor::create<test_actor>("test_actor1", system1);
const actor_ref test_actor_ref2 = local_actor::create<test_actor>("test_actor2", system2);

actor_ref remote_test_actor1 = remote_actor::create<remote_test_actor>("remote_test_actor1", system1, actor_ref("test_actor2$test_system2@localhost:4556"));
```
 * router - routers create multiple instances of local_actors that you can send messages via one actor_ref. There are multiple types of routers : random_router, round_robin_router, mailbox_router( not done yet ). You can send a broadcast message to reach all local_actors in the router.
```c++
auto system1 = actor_system::create_system("test_system1", 4555, 1);
const actor_ref r1 = round_robin_router::create<test_actor>("test_router1", system1, 2);
r1.tell(broadcast<int>(88));
```

