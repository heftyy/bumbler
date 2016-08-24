cmake_minimum_required(VERSION 3.1)

#I like to keep test files in a separate source directory called test
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

#Run through each source
foreach (testSrc ${TEST_SRCS})
    #Extract the filename without an extension (NAME_WE)
    get_filename_component(testName ${testSrc} NAME_WE)

    #Add compile target
    add_executable(${testName} ${testSrc})

    #link to Boost libraries AND your targets and dependencies
    target_link_libraries(${testName} ${Boost_LIBRARIES} bumbler)

    #Move testing binaries into test
    #set_target_properties(${testName} PROPERTIES
    #        RUNTIME_OUTPUT_DIRECTORY "build/test")

    #Finally add it to test execution -
    #Notice the WORKING_DIRECTORY and COMMAND
    add_test(NAME ${testName} COMMAND ${testName})
endforeach (testSrc)
