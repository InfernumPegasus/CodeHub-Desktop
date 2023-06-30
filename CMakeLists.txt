cmake_minimum_required(VERSION 3.24)
project(CodeHub)

set(CMAKE_CXX_STANDARD 23)

# Include boost
find_package(Boost COMPONENTS program_options REQUIRED)

add_library(mylib
        src/file/File.cpp

        src/filecomparator/FileComparator.cpp

        src/commit/Commit.cpp

        src/repository/Repository.cpp

        src/filemanager/CommitsManager.cpp
        src/filemanager/IgnoreFileManager.cpp
        src/filemanager/RepositoriesManager.cpp
        src/filemanager/RepositoryConfigManager.cpp
        src/filemanager/RestoreFileManager.cpp
        src/filemanager/UserFileManager.cpp

        src/serializer/JsonSerializer.cpp

        src/validation/Validator.cpp

        src/vcs/VersionControlSystem.cpp

        src/web/WebService.cpp
        )

target_include_directories(mylib PUBLIC include)
target_include_directories(mylib PRIVATE src)

add_executable(
        ${PROJECT_NAME}
        main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        cpr
        ${Boost_LIBRARIES}
        mylib
        )
