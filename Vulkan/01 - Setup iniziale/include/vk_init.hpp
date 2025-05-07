/**
 * @file vk_init.hpp
 * @author Fabxx
 * @brief Classe che contiene funzioni helper per inizializzare le strutture.
 *        aiuta a rendere il codice principale meno verboso.
 * @version 0.1
 * @date 2025-05-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include "VkBootstrap.h"

namespace vkInit {

    // funzioni di creazione strutture info
    VkFenceCreateInfo fenceInfo(VkFenceCreateFlagBits flags);
    VkSemaphoreCreateInfo semaphoreInfo(VkSemaphoreCreateFlags flags);
    VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlagBits flags);

    // funzioni di invio strutture info
    VkSemaphoreSubmitInfo semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
    VkCommandBufferSubmitInfo command_buffer_submit_info(VkCommandBuffer cmd);
    VkSubmitInfo2 submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo,
                                VkSemaphoreSubmitInfo* waitSemaphoreInfo);

    // Funzioni di creazione di immagini separate dalla swapchain, per permettere scalatura e precisione di rendering
    VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
    VkImageViewCreateInfo imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);

    // Funzione che carica le shader compilate in SPIR-V
    bool load_shader_module(const char* filePath, VkDevice device, VkShaderModule* outShaderModule);

    VkResult VK_CHECK(VkResult x);
};