# scheduler

Scheduler is used to send messages with a delay and/or an interval. Messages sent with the scheduler can be 
of any type, including the special types ( broadcast, stop_actor, kill_actor).

You can use the helper methods in actor system for the scheduler.

```c++
template<typename T>
std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, long initial_delay_ms, long interval_ms = 0) const;
template<typename T>
std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms, long interval_ms = 0) const;
template<typename T>
std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, long initial_delay_ms = 0) const;
template<typename T>
std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms = 0) const;
```

cancellale
---
Every time you call the schedule method it returns a ```shared_ptr<cancellable> ```.
This object is tied to the task run in the scheduler.
If every instance of the shared_ptr<cancellable> is destroyed before the task was run, the task will be cancelled.

You can cancel delayed tasks and tasks with the interval at any time.
```c++
//cancel before the message is ever sent
auto c1 = system1->schedule(53, r1, actor_ref::none(), 500, 0 );
c1->cancel();

//cancel after the message is sent 4 times (0/300/600/900 ms)
 auto c2 = system1->schedule(52, r1, actor_ref::none(), 0, 300);
std::this_thread::sleep_for(std::chrono::seconds(1));
c2.reset();
```

The sheduler stores a ```weak_ptr<cancellable>``` so the user has full control over the objects lifetime
```c++
shared_ptr<cancellable> c1->cancel();
```
is equal to
```c++
shared_ptr<cancellable> c1.reset();
```
