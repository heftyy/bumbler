cmake_minimum_required(VERSION 3.1)

set(APP_SOURCE_FILES
    bumbler.cpp
    out_router.h
    out_actor.h
    remote_server_actor.h
)

add_executable(app ${APP_SOURCE_FILES})

target_link_libraries(app ${Boost_LIBRARIES})
target_link_libraries(app bumbler shared)
