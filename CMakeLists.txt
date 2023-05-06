cmake_minimum_required(VERSION 3.24)
project(CodeHub)

set(CMAKE_CXX_STANDARD 23)

#Include cpr library for http requests
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

# Include boost
find_package(Boost COMPONENTS program_options REQUIRED)

add_library(MANAGERS
        src/filemanager/RepositoryConfigManager.cpp
        src/filemanager/CommitsManager.cpp
        src/filemanager/IgnoreFileManager.cpp
        src/filemanager/RepositoriesManager.cpp
        src/filemanager/RestoreFileManager.cpp
        src/filemanager/UserFileManager.cpp)

add_executable(${PROJECT_NAME}
        main.cpp
        src/file/File.cpp
        src/commit/Commit.cpp
        src/repository/Repository.cpp

        src/serializer/JsonSerializer.cpp

        src/validation/Validator.cpp

        src/vcs/VersionControlSystem.cpp

        src/web/WebService.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr ${Boost_LIBRARIES} MANAGERS)
