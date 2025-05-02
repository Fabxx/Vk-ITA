/**
 * @file vk_engine.cpp
 * @author Fabxx
 * @brief Classe che si occupa delle funzionalità di inizializzazione dell'engine.
 * @version 0.1
 * @date 2025-05-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>
#include <cstddef>

#include "../include/vk_engine.hpp"
#include "VkBootstrap.h"

// La prima cosa da fare è creare una finestra con SDL e la sua superficie.
void VulkanEngine::init() {

	constexpr int width {1280};
	constexpr int heigh {720};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
        width,
        heigh,
		window_flags
	);

    init_vulkan();
	init_swapchain();
	init_commands();
	init_sync_structures();

    bIsInitialized = true;
}


// Creiamo l'istanza per la GPU con vk-bootstrap con debug di base.
void VulkanEngine::init_vulkan() {

    vkb::InstanceBuilder builder;
    
    auto returned_instance = builder.set_app_name("Nome applicazione")
                    .request_validation_layers(true)
                    .use_default_debug_messenger()
                    .require_api_version(1, 3, 0)
                    .build();
    
    vkb::Instance vkb_instance = returned_instance.value();

    // Esporta nella classe l'istanza raccolta dal bootstrap e il debugger.
    _instance = vkb_instance.instance;
    _debug_messenger = vkb_instance.debug_messenger;

	// Crea la superficie da passare alla finestra.
	SDL_Vulkan_CreateSurface(window, _instance, &_surface);


    /* In questa sezione otteniamo le feature di vulkan 1.3 per il dynamic rendering
       questo ci evita l'uso dei frame buffer e dei render pass dalle versioni 
       precedenti di vulkan, inoltre possiamo usare sync2 per sincronizzare meglio
       le risorse.
    */
    VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features.dynamicRendering = true;
	features.synchronization2 = true;

    /* Qui prendiamo le feature di vulkan 1.2
       nello specifico, per usare i puntatori alla GPU senza agganciare dei buffer,
       e per avere textures non legate a nulla.
    */ 
    VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing = true;

    
    /* Seleziona una GPU con vk-bootstrap 
       Vogliamo una GPU che possa scrivere nella superficie di SDL e che supporti 
       vulkan 1.3 con le caratteristiche selezionate.
	*/
	
    vkb::PhysicalDeviceSelector selector{vkb_instance};
	
    vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 3)
		.set_required_features_13(features)
		.set_required_features_12(features12)
		.set_surface(_surface)
		.select()
		.value();

    // Creiamo il dispositivo finale.
	vkb::DeviceBuilder deviceBuilder{ physicalDevice };

	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a vulkan application
	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;
	
}

/*
    Funzione che crea la swapchain, selezioniamo il formato di colore standard RGBA8, con profondità
    di 8 bit per canale, rappresentando 256 combinazioni di colori possibili.

    NOTA: La modalità FIFO (first in first out) di presentazione dei fotogrammi forza il VSync,
          in modo che l'engine generi lo stesso numero dei fotogrammi supportati dal refresh rate del monitor.

    Inoltre, stiamo inoltrando la risoluzione delle immagini alla swapchain, perché le immagini devono 
    avere la stessa risoluzione della finestra. Attualmente è fissa, ma più in là vedremo come ricostruire
    la chain man mano che ridimensioniamo la finestra.
*/
void VulkanEngine::create_swapchain(uint32_t width, uint32_t height)
{
	vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU,_device,_surface };

	_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		//.use_default_format_selection() il formato delle immagini l'abbiamo selezionato, quindi questa funzioe è commentata
		.set_desired_format(VkSurfaceFormatKHR{ .format = _swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(width, height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value();

	_swapchainExtent = vkbSwapchain.extent;
	//store swapchain and its related images
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();
}

/*
  Funzione che distrugge la catena di immagini.
*/
void VulkanEngine::destroy_swapchain()
{
	vkDestroySwapchainKHR(_device, _swapchain, nullptr);

	// distruggi le risorse della chain
	for (size_t i = 0; i < _swapchainImageViews.size(); i++) {
		vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
	}
}


/*
  Funzione che elimina tutte le risorse usate.

  Per testare che i layers funzionino correttamente, prova a distruggere l'istanza prima di tutto.
*/
void VulkanEngine::cleanup()
{
	if (bIsInitialized) {
		destroy_swapchain();
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkDestroyDevice(_device, nullptr);
		
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		vkDestroyInstance(_instance, nullptr);
		SDL_DestroyWindow(window);
	}
}



void VulkanEngine::init_swapchain() {
    create_swapchain(_swapchainExtent.width, _swapchainExtent.height);
}

void VulkanEngine::init_commands() {
    
}
void VulkanEngine::init_sync_structures()
{
    //nothing yet
}

