cmake_minimum_required(VERSION 3.16)
project(VehicleRentalSystem CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

set(CORE_SOURCES
    src/Vehicle.cpp
    src/Car.cpp
    src/Bicycle.cpp
    src/Motorcycle.cpp
    src/Transaction.cpp
    src/Inventory.cpp
)

add_library(vehicle_core ${CORE_SOURCES})
target_include_directories(vehicle_core PUBLIC include)
target_link_libraries(vehicle_core PUBLIC nlohmann_json::nlohmann_json)

add_executable(vehicle_app src/main.cpp)
target_link_libraries(vehicle_app PRIVATE vehicle_core)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
    )
    FetchContent_MakeAvailable(doctest)

    add_executable(vehicle_tests tests/test_inventory.cpp)
    target_link_libraries(vehicle_tests PRIVATE vehicle_core doctest::doctest)

    add_test(NAME vehicle_tests COMMAND vehicle_tests)
endif()
