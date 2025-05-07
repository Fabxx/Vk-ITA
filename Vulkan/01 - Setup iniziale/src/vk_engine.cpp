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
#include <SDL2/SDL_vulkan.h>
#include <cstddef>
#include <iostream>
#include <chrono>
#include <thread>
#include <fmt/core.h>
#include <fmt/format.h>
#include <filesystem>

#include "../include/vk_engine.hpp"
#include "../include/vk_images.hpp"
#include "VkBootstrap.h"
#include "vk_mem_alloc.hpp"

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
	init_descriptors();
	init_pipelines();

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

	// Prendi il dispositivo usato dall'intera applicazione.
	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;

	// Ottieni le queue e i loro tipi
	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	// inizializza il memory allocator
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = _chosenGPU;
	allocatorInfo.device = _device;
	allocatorInfo.instance = _instance;
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&allocatorInfo, &_allocator);

	_mainDeletionQueue.push_function([&]() {
		vmaDestroyAllocator(_allocator); 
		});
}

/*
    Funzione che crea la swapchain, selezioniamo il formato di colore standard RGBA8, con profondità
    di 8 bit per canale, rappresentando 256 combinazioni di colori possibili.

    NOTA: La modalità FIFO (first in first out) di presentazione dei fotogrammi forza il VSync,
          in modo che l'engine generi lo stesso numero dei fotogrammi supportati dal refresh rate del monitor.

	NOTA: RGBA8 è un formato dei colori Little-Endian, se vuoi big-endian, usa BGRA8

    Inoltre, stiamo inoltrando la risoluzione delle immagini alla swapchain, perché le immagini devono 
    avere la stessa risoluzione della finestra. Attualmente è fissa, ma più in là vedremo come ricostruire
    la chain man mano che ridimensioniamo la finestra.
*/
void VulkanEngine::create_swapchain(uint32_t width, uint32_t height)
{
	vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU,_device,_surface };

	_swapchainImageFormat = VK_FORMAT_R8G8B8A8_UNORM;

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		//.use_default_format_selection() il formato delle immagini l'abbiamo selezionato, quindi questa funzione è commentata
		.set_desired_format(VkSurfaceFormatKHR{ .format = _swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		// Usa modalità di presentazione con VSync (FIFO_KHR)
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
		/*
		* Abbiamo creato la command pool con i buffer, aspettiamo che la GPU finisca e poi distruggiamo anch'essa.
		* Siccome è l'oggetto più recente, distruggiamo prima esso, poi la swapchain e la superficie.
		*/ 
		vkDeviceWaitIdle(_device);

		for (int i = 0; i < FRAME_OVERLAP; i++) {
			vkDestroyCommandPool(_device, _frames[i].commandPool, nullptr);
			
			//destroy sync objects
			vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
			vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
			vkDestroySemaphore(_device, _frames[i]._swapchainSemaphore, nullptr);
			_frames[i]._deletionQueue.flush();
		}

		//flush the global deletion queue
		_mainDeletionQueue.flush();

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
	
	//La dimensione del disegno dell'immagine combacia con la finestra
	VkExtent3D drawImageExtent = {
		_swapchainExtent.width,
		_swapchainExtent.height,
		1
	};

	//hardcode il formato di disegno a 16 bit float
	_drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	_drawImage.imageExtent = drawImageExtent;

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkImageCreateInfo rimg_info = vkInit::image_create_info(_drawImage.imageFormat, drawImageUsages, drawImageExtent);

	//Allochiamo l'immagine da disegnare dalla memoria locale della GPU.
	VmaAllocationCreateInfo rimg_allocinfo = {};
	rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//Alloca e crea l'immagine
	vmaCreateImage(_allocator, &rimg_info, &rimg_allocinfo, &_drawImage.image, &_drawImage.allocation, nullptr);

	//Costruisci un'anteprima per l'immagine da usare nel rendering.
	VkImageViewCreateInfo rview_info = vkInit::imageview_create_info(_drawImage.imageFormat, _drawImage.image, 
																	 VK_IMAGE_ASPECT_COLOR_BIT);

	vkInit::VK_CHECK(vkCreateImageView(_device, &rview_info, nullptr, &_drawImage.imageView));

	// Aggiungi alle queue da cancellare.
	_mainDeletionQueue.push_function([=]() {
		vkDestroyImageView(_device, _drawImage.imageView, nullptr);
		vmaDestroyImage(_allocator, _drawImage.image, _drawImage.allocation);
		});
}

/*
* Crea una command pool per i comandi da inviare alla queue
* vogliamo che la pool ci permetta il reset dei command buffer individuali
* 
* il for loop crea le command pool definite
* 
* Le due strutture info contengono le informazioni riguardo
* la command pool e i command buffer.
*/
void VulkanEngine::init_commands() {

	for (int i = 0; i < FRAME_OVERLAP; i++) {	
		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.pNext = nullptr;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = _graphicsQueueFamily;

		vkInit::VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i].commandPool));
		
		VkCommandBufferAllocateInfo commandBufferInfo = {};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.pNext = nullptr;
		commandBufferInfo.commandPool = _frames[i].commandPool;
		commandBufferInfo.commandBufferCount = 1;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		vkInit::VK_CHECK(vkAllocateCommandBuffers(_device, &commandBufferInfo, &_frames[i].commandBuffer));
	}
}


/*
* Una volta creati i command pool e buffer per inviare i comandi alla
* GPU, dobbiamo sincronizzare queste strutture tra GPU e CPU per tracciarne
* lo stato di esecuzione.
*/
void VulkanEngine::init_sync_structures()
{
	VkFenceCreateInfo fenceInfo = vkInit::fenceInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreInfo = vkInit::semaphoreInfo(NULL);

	for (int i = 0; i < FRAME_OVERLAP; i++) {
		vkInit::VK_CHECK(vkCreateFence(_device, &fenceInfo, nullptr, &_frames[i]._renderFence));
		vkInit::VK_CHECK(vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_frames[i]._swapchainSemaphore));
		vkInit::VK_CHECK(vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_frames[i]._renderSemaphore));
	}
}

/*
* Funzione per disegnare sullo schermo.
* 
* NOTA: L'ideale sarebbe fare tutto tramite shaders, e non fare 
* computazioni tramite CPU. questo codice verrà rimosso più in la.
* 
* Attendi che la GPU abbia terminato il suo lavoro
* per poi resettare la Fence per sincronizzare il nuovo stato
* del command buffer tra GPU e CPU.
* 
* La fence va resettata ad ogni comando usato dal buffer.
* 
* L'ultimo parametro della funzione wait rappresenta 1 secondo in nanosecondi.
* 
* Se impostato a 0, puoi sapere se la GPU sta ancora eseguendo il comando o no.
* 
* Poi, richiediamo l'indice dell'immagine nella swapchain, se nessuna immagine è
* disponibile, il thread verrà bloccato per 1 secondo per ottenere un'immagine disponibile.
* 
* 
*/


/*
* Funzione di init dei descriptors.
* 
* Creiamo 10 descrittori nel set di tipo STORE_IMAGE per la compute
* shader usata.
* 
* Il vettore crea una pool che contiene 10 set (puntatori) con ciascuna 1 immagine.
* 
* Poi, si imposta la disposizione (layout) in scrittura, con il flag STAGE_COMPUTE.
* 
* Nota che la pool non ha allocazione dinamica, se si usano più di 10 descrittori
* il programma crasha. La pool verrà resa dinamica più avanti.
* 
* 
* Poi allochiamo il set di descrittori per la nostra immagine da disegnare.
* 
* In questo caso la struttura di allocazione indica che il set di descrittori
* 
* è di tipo STORAGE_IMAGE con layout generico.
* 
* Infine puliamo l'allocatore dei descrittori e i layout usati per l'immagine.
* 
* Il set 0 punta al Binding 0 dove l'immagine 2D viene presa dalla shader.
* 
* 
*/
void VulkanEngine::init_descriptors()
{
	std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }};

	globalDescriptorAllocator.init_pool(_device, 10, sizes);

	{
		DescriptorLayoutBuilder builder;
		builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		_drawImageDescriptorLayout = builder.build(_device, VK_SHADER_STAGE_COMPUTE_BIT);
	}



	_drawImageDescriptors = globalDescriptorAllocator.allocate(_device, _drawImageDescriptorLayout);

	VkDescriptorImageInfo imgInfo{};
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgInfo.imageView = _drawImage.imageView;

	VkWriteDescriptorSet drawImageWrite = {};
	drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	drawImageWrite.pNext = nullptr;

	drawImageWrite.dstBinding = 0;
	drawImageWrite.dstSet = _drawImageDescriptors;
	drawImageWrite.descriptorCount = 1;
	drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	drawImageWrite.pImageInfo = &imgInfo;

	vkUpdateDescriptorSets(_device, 1, &drawImageWrite, 0, nullptr);

	_mainDeletionQueue.push_function([&]() {
		globalDescriptorAllocator.destroy_pool(_device);

		vkDestroyDescriptorSetLayout(_device, _drawImageDescriptorLayout, nullptr);
	});
}

/*
* Funzione che inizializza le pipeline in background.
* 
* Per creare una pipeline è necessario avere un array di layout 
  di DescriptorSets e altre configurazioni come Push-Constants.

  In questo caso non ne abbiamo bisogno per questa shader.

  Ora creiamo l'oggetto pipeline vero e proprio aggiungendo lo 
  shader module e altre informazioni tramite VkComputePipelineCreateInfo,

  Dipende  dalle strutture stageInfo che richiede che la shader sia compilata

  e dalla computePipelineCreateInfo, che richiede i dati compilati della struttura
  precedente.

  Se la compilazione della shader fallisce, allora il programma crasha.

  Nota che l'entry point della shader in questo caso è main, ma se vogliamo
  più shader module con diversi entry point, possiamo cambiare il nome
  della funzione nella shader per poi farne riferimento qui.

  Infine come ogni oggetto, distruggiamo lo shader module creato,
  il layout della pipeline e la pipeline stessa.
* 
*/

void VulkanEngine::init_pipelines()
{
	init_background_pipelines();
}

/*
* Funzione che inizializza una pipeline, in questo caso una compute
* pipeline.
* 
* La funzione scansiona la directory delle shaders e cerca i file compilati in 
* 
* SPRI-V da GLSL, se trova una shader valida, la carica all'interno dello shaderModule.
* 
* Infine, i dati della shader vengono passati ai descriptor sets.
*/
void VulkanEngine::init_background_pipelines()
{
	VkPipelineLayoutCreateInfo computeLayout{};
	computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computeLayout.pNext = nullptr;
	computeLayout.pSetLayouts = &_drawImageDescriptorLayout;
	computeLayout.setLayoutCount = 1;

	vkInit::VK_CHECK(vkCreatePipelineLayout(_device, &computeLayout, nullptr, &_gradientPipelineLayout));


	VkShaderModule computeDrawShader = VK_NULL_HANDLE;

	namespace fs = std::filesystem;

	const std::string shaderDir = "shaders";

	for (const auto& entry : fs::directory_iterator(shaderDir)) {
		if (entry.is_regular_file() && entry.path().extension() == ".spv") {
			const std::string path = entry.path().string();

			VkShaderModule shaderModule;

			if (!vkInit::load_shader_module(path.c_str(), _device, &shaderModule)) {
				fmt::print("Failed to load shader: {}\n", path);
			}
			else {
				fmt::print("Loaded shader: {}\n", path);
				computeDrawShader = shaderModule;
			}
		}
	}

	VkPipelineShaderStageCreateInfo stageinfo{};
	stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageinfo.pNext = nullptr;
	stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stageinfo.module = computeDrawShader;
	stageinfo.pName = "main";

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.pNext = nullptr;
	computePipelineCreateInfo.layout = _gradientPipelineLayout;
	computePipelineCreateInfo.stage = stageinfo;

	vkInit::VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, 
											  nullptr, &_gradientPipeline));

	vkDestroyShaderModule(_device, computeDrawShader, nullptr);

	_mainDeletionQueue.push_function([&]() {
		vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
		vkDestroyPipeline(_device, _gradientPipeline, nullptr);
		});
}


void VulkanEngine::draw() {

	vkInit::VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));
	
	get_current_frame()._deletionQueue.flush();

	vkInit::VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));

	uint32_t swapchainImageIndex;
	vkInit::VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, get_current_frame()._swapchainSemaphore,
		nullptr, &swapchainImageIndex));

	// reset del command buffer dopo l'esecuzione.

	vkInit::VK_CHECK(vkResetCommandBuffer(get_current_frame().commandBuffer, 0));

	//inizia la registrazione del command buffer, lo useremo una sola volta, il flag indica questo a Vulkan.
	VkCommandBufferBeginInfo commandBufferBeginInfo = vkInit::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	_drawExtent.width = _drawImage.imageExtent.width;
	_drawExtent.height = _drawImage.imageExtent.height;

	vkInit::VK_CHECK(vkBeginCommandBuffer(get_current_frame().commandBuffer, &commandBufferBeginInfo));

	/*
	* Transita l'immagine da disegnare nel layout generale cosi da scriverci dentro
	* Lo sovrascriviamo completamente cosi non ci importa di cosa c'era nel vecchio layout.
	*/

	vkutil::transition_image(get_current_frame().commandBuffer, _drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);


	// La funzione principale che disegna sullo schermo. Qui possiamo inserire altre funzioni di disegno in sequenza.
	draw_background(get_current_frame().commandBuffer);

	//Transita l'immagine e la swapchain nei loro corretti layout.
	vkutil::transition_image(get_current_frame().commandBuffer, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::transition_image(get_current_frame().commandBuffer, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// esegui una copia dell'immagine disegnata nella swapchain
	vkutil::copy_image_to_image(get_current_frame().commandBuffer, _drawImage.image, _swapchainImages[swapchainImageIndex], _drawExtent, _swapchainExtent);

	// imposta il layout della swapchain in "presentazione" cosi da mostrare l'immagine.
	vkutil::transition_image(get_current_frame().commandBuffer, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//Finalizza il command buffer (non possiamo aggiungere comandi, ma possiamo eseguirlo)
	vkInit::VK_CHECK(vkEndCommandBuffer(get_current_frame().commandBuffer));


	/*
	* Prepara l'invio alla queue
	* vogliamo aspettare il segnale del _presentSemaphore in quanto indica quando la swapchain è pronta.
	* invieremo il segnale al _renderSemaphore per indicare che il rendering è finito.
	*/

	VkCommandBufferSubmitInfo cmdinfo = vkInit::command_buffer_submit_info(get_current_frame().commandBuffer);

	VkSemaphoreSubmitInfo waitInfo = vkInit::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
		get_current_frame()._swapchainSemaphore);
	VkSemaphoreSubmitInfo signalInfo = vkInit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
		get_current_frame()._renderSemaphore);

	VkSubmitInfo2 submit = vkInit::submit_info(&cmdinfo, &signalInfo, &waitInfo);

	// Invia il command buffer alla queue e eseguilo.
	// _renderFence ora bloccherà fin quando i comandi grafici non hanno terminato l'esecuzione.
	vkInit::VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, get_current_frame()._renderFence));


	/*
	* Prepara la presentazione
	* 
	* questo mette l'immagine che abbiamo renderizzato in una finestra visibile.
	* vogliamo aspettare il _renderSemaphore per questo.
	* è necessario che i comandi di disegno abbiano finito di renderizzare l'immagine
	* prima di mostrarla all'utente.
	*/
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.pSwapchains = &_swapchain;
	presentInfo.swapchainCount = 1;
	presentInfo.pWaitSemaphores = &get_current_frame()._renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pImageIndices = &swapchainImageIndex;

	vkInit::VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

	//Incrementa il numero dei fotogrammi disegnati.
	_frameNumber++;
}

/*
* Funzione di disegno in background.
* 
* Richiama la compute shader impostata, 
* dandogli un punto di aggancio alla nostra pipeline
* 
* poi dandogli i descriptor sets che abbiamo creato per quella shader
* e immagine.
* 
* Infine, eseguiamo la pipeline con il comando dispatch. Il disegno viene
* diviso di 16 su larghezza e 16 su altezza, poiché il gruppo di lavoro
* è di 16x16 nella shader.
* 
*/
void VulkanEngine::draw_background(VkCommandBuffer cmd)
{
	
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipeline);

	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipelineLayout, 0, 1, 
							&_drawImageDescriptors, 0, nullptr);

	vkCmdDispatch(cmd, std::ceil(_drawExtent.width / 16.0), std::ceil(_drawExtent.height / 16.0), 1);

}

void VulkanEngine::run()
{
	SDL_Event e;
	bool bQuit = false;

	// main loop
	while (!bQuit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT)
				bQuit = true;

			if (e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
					stop_rendering = true;
				}
				if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
					stop_rendering = false;
				}
			}
		}

		// Non renderizzare se la finestra è minimizzata.
		if (stop_rendering) {
			// Rallenta la velocità del thread.
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		draw();
	}
}
