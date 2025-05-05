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

//TODO: Cambia vkinit da classe a namespace, avendo solo funzioni non è necessario mantenere lo stato di variaibli in memoria.

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

    VkResult VK_CHECK(VkResult x);
};