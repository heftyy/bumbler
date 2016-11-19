props
---
props is used for creating actors. props is the interface and typed_propsis the implementation.

```c++
typed_props<ActorType, TypedActor : UntypedActor>

template<typename RouterPool, typename ...RouterPoolArgs>
typed_props& with_router(RouterPoolArgs&&... pool_args);

template<typename Mailbox, typename ...MailboxArgs>
typed_props& with_mailbox(MailboxArgs&&... mailbox_args);

typed_props& with_network_actor(const std::string network_actor_ref);
typed_props& with_network_actor(const actor_ref& network_actor_ref);
```

creating actors
---
test_actor is the class shown in actor documentation.

* local_actor - a regular actor type. Each local actor has a lightweight thread that reads incoming messages and runs the tasks in the dispatcher supplied by the actor_system.
```c++
auto system1 = actor_system::create_system("test_system1", 4555);
auto props = typed_props<local_actor, test_actor>();
auto test_actor_ref1 = system1->actor_of(props, "test_actor1");
```
* router - routers create multiple instances of local_actors that you can send messages via one actor_ref.
Available router pools:
  random_pool,
  round_robin_pool,
  smallest_mailbox_pool.
You can send a broadcast message to reach all local_actors in the router.
```c++
auto system1 = actor_system::create_system("test_system1", 4555, 1);
auto router_props = typed_props<router, test_actor>();
router_props.with_router<round_robin_pool>(4);
auto r1 = system1->actor_of(router_props, "test_router1");
r1.tell(broadcast<int>(88));
```

Using different types of mailboxes
---

Each actor has a mailbox thats responsible for storing and ordering the messages. You can set it using
```c++
props.with_mailbox<MailboxType>(Args...)
```
Available mailboxes:
  fifo_mailbox
  lifo_mailbox
  priority_mailbox.
```c++
auto local_actor_fifo_props = typed_props<local_actor, test_actor>();
local_actor_fifo_props.with_mailbox<priority_mailbox>();
```
fifo_mailbox is the default one.
