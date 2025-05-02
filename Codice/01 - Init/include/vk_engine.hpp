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

class VulkanEngine {

    public:
        SDL_Window *window;
        VkInstance _instance; // gestore della libreria
        VkDebugUtilsMessengerEXT _debug_messenger; // debugger per i messaggi dei layers. 
        VkPhysicalDevice _chosenGPU; // GPU scelta come dispositivo.
        VkDevice _device; // dispositivo vulkan per i comandi
        VkSurfaceKHR _surface;// superficie della finestra vulkan

        VkSwapchainKHR _swapchain; // la catena di fotogrammi da mostrare a schermo
	    VkFormat _swapchainImageFormat; // il formato delle immagini nella swapchain

	    std::vector<VkImage> _swapchainImages; // vettore che contiene le immagini da passare alla chain
	    std::vector<VkImageView> _swapchainImageViews; // vettore di visualizzazioni delle immagini.
	    VkExtent2D _swapchainExtent;

        bool bIsInitialized {false};

        void init();
        void cleanup();
        void draw();
        void run();

    private:
        void init_vulkan();
        void init_swapchain();
        void init_commands();
        void init_sync_structures();

        void create_swapchain(uint32_t width, uint32_t height);
	    void destroy_swapchain();
};