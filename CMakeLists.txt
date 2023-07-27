cmake_minimum_required(VERSION 3.24)
project(CodeHub)

set(CMAKE_CXX_STANDARD 23)

# Include boost
find_package(Boost COMPONENTS program_options REQUIRED)

add_library(global
        src/file/File.cpp

        src/filecomparator/FileComparator.cpp

        src/log/Logger.cpp

        src/commit/Commit.cpp

        src/config/RepositoryConfig.cpp

        src/repository/Repository.cpp

        src/filemanager/IgnoreFileManager.cpp
        src/filemanager/RestoreFileManager.cpp
        src/filemanager/UserFileManager.cpp
        src/filemanager/FilesManager.cpp

        src/json/JsonSerializer.cpp

        src/validation/Validator.cpp

        src/web/WebService.cpp

        src/vcs/VersionControlSystem.cpp
        )

target_include_directories(global PUBLIC include)
target_include_directories(global PRIVATE src)

add_executable(
        ${PROJECT_NAME}
        main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        fmt
        cpr
        ${Boost_LIBRARIES}
        global
        )
