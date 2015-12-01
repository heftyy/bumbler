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
* [props](/props.md)
