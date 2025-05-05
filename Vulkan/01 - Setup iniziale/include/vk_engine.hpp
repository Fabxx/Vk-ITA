/**
 * @file vk_engine.hpp
 * @author Fabxx
 * @brief Questa classe contiene gli oggetti principali di vulkan e le funzioni per inizializzare essi.
          NOTA: il codice di setup viene fatto una sola volta, poi può essere copiato e incollato in 
                altri progetti.
 * @version 0.1
 * @date 2025-05-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <SDL2/SDL_video.h>
#include <vulkan/vulkan.hpp>
#include <deque>
#include <functional>
#include "vk_init.hpp"
#include "vk_mem_alloc.h"

struct AllocatedImage {
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
};

/*
* Struttura che ci aiuta nella distruzione delle strutture
* 
*/
struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)(); //call functors
        }

        deletors.clear();
    }
};

/*
* Struttura che contiene i dati che renderizzeremo sulla finestra.
* La useremo per creare più command buffer per lavorare in parallelo.
*
* FRAME_OVERLAP rappresenta il numero di buffer che andremo a creare.
*
* di conseguenza creeremo copie di struct che contengano ciascuna
*
* la sua pool, il suo buffer, i suoi semafori e la sua fence.
*
* il primo semaforo serve ad attendere la richiesta dell'immagine
* da parte della swapchain prima di inoltrarla ad essa
*
* il secondo semaforo verrà usato per controllare la presentazione
* dell'immagine al sistema operativo una volta che il disegno
* è terminato.
*
* La renderFence ci farà attendere che tutti i comandi di disegno
* di un frame finiscano.
*/

struct FrameData {
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkSemaphore _swapchainSemaphore, _renderSemaphore;
    VkFence _renderFence;

    DeletionQueue _deletionQueue;
};


constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {

    public:
        SDL_Window *window;
        VkInstance _instance;
        VkDebugUtilsMessengerEXT _debug_messenger;
        VkPhysicalDevice _chosenGPU;
        VkDevice _device;
        VkSurfaceKHR _surface;

        VkSwapchainKHR _swapchain;
	    VkFormat _swapchainImageFormat;

	    std::vector<VkImage> _swapchainImages;
	    std::vector<VkImageView> _swapchainImageViews;
	    VkExtent2D _swapchainExtent;
        int _frameNumber{ 0 };

        FrameData _frames[FRAME_OVERLAP];
        FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

        VkQueue _graphicsQueue;
        uint32_t _graphicsQueueFamily;

        DeletionQueue _mainDeletionQueue;

        VmaAllocator _allocator;

        //draw resources
        AllocatedImage _drawImage;
        VkExtent2D _drawExtent;

        bool bIsInitialized {false};

        void init();
        void draw();
        void cleanup();

    private:
        void init_vulkan();
        void init_swapchain();
        void init_commands();
        void init_sync_structures();

        void create_swapchain(uint32_t width, uint32_t height);
	    void destroy_swapchain();

        void draw_background(VkCommandBuffer cmd);
};