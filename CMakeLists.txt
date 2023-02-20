cmake_minimum_required(VERSION 3.24)
project(CodeHub)

set(CMAKE_CXX_STANDARD 23)

#Include cpr library for http requests
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

#Folders required for tests
add_subdirectory(tests/lib/googletest)
include_directories(tests/lib/googletest/googletest/include)

add_executable(CodeHub
        main.cpp
        entity/file/File.cpp
        entity/commit/Commit.cpp
        entity/repository/Repository.cpp
        serializer/JsonSerializer.cpp
        WebService.cpp
        entity/filemanager/ConfigManager.cpp
        entity/filemanager/CommitsManager.cpp
        entity/filemanager/IgnoreFileManager.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

##Tests
#add_executable(Tests
#        message/FieldMessage.cpp
#        message/FieldMessageSerializer.cpp
#        tests/FieldMessageFixture.cpp)
#
##Link with GTests 'main' function
#target_link_libraries(Tests gtest gtest_main)