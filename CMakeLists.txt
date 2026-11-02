cmake_minimum_required(VERSION 3.16)

# CMake 4 dropped the compatibility shims for dependencies that still
# declare a very old `cmake_minimum_required` (doctest). Setting this
# variable keeps FetchContent working on both old and new CMake.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(VehicleRentalSystem VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_TESTS "Build the unit test suite" ON)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

include(FetchContent)

# nlohmann/json — single-file release tarball (fast, checksum-verified).
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
)
FetchContent_MakeAvailable(json)

# ------------------------------------------------------------- warnings

add_library(vehicle_warnings INTERFACE)
if(MSVC)
    target_compile_options(vehicle_warnings INTERFACE /W4 /permissive- /utf-8)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(vehicle_warnings INTERFACE /WX)
    endif()
else()
    target_compile_options(vehicle_warnings INTERFACE -Wall -Wextra -Wpedantic)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(vehicle_warnings INTERFACE -Werror)
    endif()
endif()

# ------------------------------------------------------------ core lib

set(CORE_SOURCES
    src/Cli.cpp
    src/Vehicle.cpp
    src/Car.cpp
    src/Bicycle.cpp
    src/Motorcycle.cpp
    src/Truck.cpp
    src/Transaction.cpp
    src/VehicleFactory.cpp
    src/Inventory.cpp
)

add_library(vehicle_core ${CORE_SOURCES})
target_include_directories(vehicle_core PUBLIC include)
target_link_libraries(vehicle_core PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(vehicle_core PRIVATE vehicle_warnings)
target_compile_definitions(vehicle_core PUBLIC APP_VERSION="${PROJECT_VERSION}")

# --------------------------------------------------------------- app

add_executable(vehicle_app src/main.cpp)
target_link_libraries(vehicle_app PRIVATE vehicle_core vehicle_warnings)

# -------------------------------------------------------------- tests

if(BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(doctest)

    add_executable(vehicle_tests
        tests/test_vehicles.cpp
        tests/test_inventory.cpp
        tests/test_factory.cpp
    )
    target_link_libraries(vehicle_tests PRIVATE vehicle_core doctest::doctest vehicle_warnings)

    add_test(NAME vehicle_tests COMMAND vehicle_tests)
endif()

# ------------------------------------------------------ clang-format

find_program(CLANG_FORMAT_EXE NAMES clang-format clang-format-19 clang-format-18 clang-format-17)
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE FORMAT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
    )
    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT_EXE} -i ${FORMAT_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Reformatting sources with clang-format"
    )
endif()
