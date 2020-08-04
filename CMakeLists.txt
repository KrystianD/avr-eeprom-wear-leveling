cmake_minimum_required(VERSION 3.16)
project(avr_eeprom_wear_leveling)

option(ENABLE_TESTS "Enable tests" OFF)

if(ENABLE_TESTS)
    find_package(GTest)
    include_directories(test/mock)
    add_executable(tests test/tests_byte.cpp test/tests_bit.cpp test/tests_utils.cpp)
    target_link_libraries(tests gtest gtest_main)
endif()
