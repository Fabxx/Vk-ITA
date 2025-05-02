cmake_minimum_required(VERSION 3.30.5)

project(
    App # name of the project, which will also be the name of the visual studio solution if you use it
    VERSION 0.1.0 # any version number
    LANGUAGES CXX C # programming languages used by the project
)

# Default to Release build if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# MUST be set before target compile options to define executable name.

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS include/*.hpp include/*.h)

add_executable(App 
   ${SRC_FILES}
   ${HEADER_FILES}
)

# Compiler usage
if (MSVC)
    target_compile_options(App PRIVATE /W4)
else()
    target_compile_options(App PRIVATE -Wall -Wextra -pedantic)
endif()

# Language standard definition and warning flags
# sets warnings as errors. Does not allow to compile if warnings are considered errors
set_target_properties(App PROPERTIES CXX_STANDARD 20 COMPILE_WARNING_AS_ERROR OFF )

# add local libraries here. NOTE: Local folder libraries must have a CMakeLists.txt
#add_subdirectory(include)

# Get system libraries to link
find_package(Vulkan REQUIRED)

# Get VK Bootstrap
include(FetchContent)

FetchContent_Declare(
    fetch_vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG        v1.4.313 #suggest using a tag so the library doesn't update whenever new commits are pushed to a branch
)

FetchContent_MakeAvailable(fetch_vk_bootstrap)

set(SDL2_VERSION 2.32.4)

FetchContent_Declare(
    SDL2
    URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.32.4.tar.gz
    FIND_PACKAGE_ARGS ${SDL2_VERSION} EXACT
)

FetchContent_MakeAvailable(SDL2)


# add libraries to link, name must match library folder name.
target_link_libraries(App PRIVATE Vulkan::Vulkan vk-bootstrap::vk-bootstrap SDL2::SDL2)
