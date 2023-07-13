cmake_minimum_required(VERSION 3.24)
project(CodeHub)

set(CMAKE_CXX_STANDARD 23)

# Include boost
find_package(Boost COMPONENTS program_options REQUIRED)

add_library(global
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
        src/filemanager/FilesManager.cpp

        src/serializer/JsonSerializer.cpp

        src/validation/Validator.cpp

        src/web/WebService.cpp

        src/vcs/VersionControlSystem.cpp
        )

#add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion)

target_include_directories(global PUBLIC include)
target_include_directories(global PRIVATE src)

add_executable(
        ${PROJECT_NAME}
        main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        cpr
        ${Boost_LIBRARIES}
        global
        )
