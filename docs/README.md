#### bumbler - Multithreading and networking library based on the actor model.

This library is written in c++11 and heavily relies on boost.

Basic concepts
---
 * [actor_system](/actor_system.md) - main structure that contains all the elements in the library (actors, dispatcher, scheduler, server etc...)
 * [actors](/actor.md) - created in a specific actor_system via the factory methods that return an actor_ref. Each actor has a lightweight thread that reads incoming messages.
 * [actor_ref](/actor_ref.md) - small objects that let you interact with actors.
 * [scheduler](/scheduler.md) - part of the actor_system that lets you schedule tasks to run with a certain delay and/or repeat after a given interval
 * dispatcher - used internally by the actor_system and actors the run tasks in a thread pool, thread pool size is specified when creating the actor_system
 * messages - can be anything (builtin types / user created object)

TODO
---
 * add namespaces
 * supervision strategy
