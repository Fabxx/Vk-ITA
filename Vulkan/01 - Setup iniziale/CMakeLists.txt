# Versione minima richiesta per CMake
cmake_minimum_required(VERSION 3.30.5)

# Nome del progetto, versione del progetto e linguaggi usati
project(
    App
    VERSION 0.1.0
    LANGUAGES CXX C
)

# Cerca tutti i file sorgente e di intestazione per la compilazione e il linkaggio.

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS include/*.hpp include/*.h)

add_executable(App 
   ${SRC_FILES}
   ${HEADER_FILES}
)

# Configura i compilatori MSVC o GCC/G++
if (MSVC)
    target_compile_options(App PRIVATE /W4)
else()
    target_compile_options(App PRIVATE -Wall -Wextra -pedantic)
endif()

# Flag per impostare lo standard del linguaggio e non trattare gli avvisi come errori.
set_target_properties(App PROPERTIES CXX_STANDARD 20 COMPILE_WARNING_AS_ERROR OFF )

# Ottieni Vulkan dall'SDK installato.
find_package(Vulkan REQUIRED)

# Ottieni VK Bootstrap e compilalo.
include(FetchContent)

FetchContent_Declare(
    fetch_vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG        v1.4.313 # usiamo un tag anziché master per usare release stable.
)

FetchContent_MakeAvailable(fetch_vk_bootstrap)

# Ottieni SDL2
set(SDL2_VERSION 2.32.4)

FetchContent_Declare(
    SDL2
    URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.32.4.tar.gz
    FIND_PACKAGE_ARGS ${SDL2_VERSION} EXACT
)

FetchContent_MakeAvailable(SDL2)

# Ottieni Vulkan memory allocator e la libreria equivalente in C++

set(VulkanMemoryAllocator_VERSION 3.2.1)

FetchContent_Declare(
    VulkanMemoryAllocator
    URL https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/v3.2.1.tar.gz
    FIND_PACKAGE_ARGS ${VulkanMemoryAllocatorVERSION} EXACT
)

FetchContent_MakeAvailable(VulkanMemoryAllocator)

set(VulkanMemoryAllocator-Hpp_VERSION 3.2.1)

FetchContent_Declare(
    VulkanMemoryAllocatorHpp
    URL https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/archive/refs/tags/v3.2.1.tar.gz
    FIND_PACKAGE_ARGS ${VulkanMemoryAllocatorHppVERSION} EXACT
)

FetchContent_MakeAvailable(VulkanMemoryAllocatorHpp)

# Linka le librerie all'eseguibile.
target_link_libraries(App PRIVATE 
Vulkan::Vulkan 
vk-bootstrap::vk-bootstrap 
SDL2::SDL2 
GPUOpen::VulkanMemoryAllocator 
VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp)
