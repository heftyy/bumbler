#### ATAN - Multithreading and networking library based on the actor model.

This library is written in c++11 and heavily relies on boost.

Basic concepts
---
 * [actor_system](https://github.com/heftyy/bumbler/tree/master/shared/atan/actor_system) - main structure that contains all the elements in the library (actors, dispatcher, scheduler, server etc...)
 * [actors](https://github.com/heftyy/bumbler/tree/master/shared/atan/actor) - created in a specific actor_system via the factory methods that return an actor_ref. Each actor has a lightweight thread that reads incoming messages.
 * [actor_ref](https://github.com/heftyy/bumbler/tree/master/shared/atan/actor/actor_ref) - small objects that let you interact with actors.
 * [scheduler](https://github.com/heftyy/bumbler/tree/master/shared/atan/scheduler) - part of the actor_system that lets you schedule tasks to run with a certein delay and/or repeat after a given interval
 * [dispatcher](https://github.com/heftyy/bumbler/tree/master/shared/atan/dispatcher) - used internally by the actor_system and actors the run tasks in a thread pool, thread pool size is specificed when creating the actor_system
 * [messages](https://github.com/heftyy/bumbler/tree/master/shared/atan/messages) - can be anything (builtin types / user created object) 
