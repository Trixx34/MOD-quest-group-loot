cmake_minimum_required(VERSION 3.20)
project(quest_group_loot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The SDK can come from a local checkout (FOREVER_MODULE_SDK_SOURCE) or from GitHub.
set(FOREVER_MODULE_SDK_SOURCE "" CACHE PATH "Path to a forever-module-sdk checkout (optional)")
set(FOREVER_MODULE_SDK_BUILD_TESTS OFF CACHE BOOL "" FORCE)
if(FOREVER_MODULE_SDK_SOURCE)
    add_subdirectory("${FOREVER_MODULE_SDK_SOURCE}" "${CMAKE_BINARY_DIR}/forever-module-sdk")
else()
    include(FetchContent)
    FetchContent_Declare(forever_module_sdk
        GIT_REPOSITORY https://github.com/Trixx34/forever-module-sdk
        GIT_TAG main)
    FetchContent_MakeAvailable(forever_module_sdk)
endif()

add_library(quest_share_policy STATIC src/share_policy.cpp)
target_include_directories(quest_share_policy PUBLIC src)

add_library(quest_share_settings STATIC src/share_settings.cpp)
target_include_directories(quest_share_settings PUBLIC src)
target_link_libraries(quest_share_settings PUBLIC quest_share_policy)

add_executable(quest-group-loot src/main.cpp)
target_link_libraries(quest-group-loot PRIVATE quest_share_policy quest_share_settings forever_module_sdk)

enable_testing()
add_executable(policy_test tests/policy_test.cpp)
target_link_libraries(policy_test PRIVATE quest_share_policy)
add_test(NAME policy COMMAND policy_test)

add_executable(settings_test tests/settings_test.cpp)
target_link_libraries(settings_test PRIVATE quest_share_settings)
add_test(NAME settings COMMAND settings_test)
