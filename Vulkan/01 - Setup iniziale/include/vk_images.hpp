/**
 * @file vk_images.hpp
 * @author Fabxx
 * @brief Classe che contiene funzioni per gestire le transizioni delle immagini sul command
 *        buffer per sfurttare il rendering dinamico.
 * @version 0.1
 * @date 2025-05-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include <vulkan/vulkan.hpp>

namespace vkutil {
	void transition_image(VkCommandBuffer cmd, VkImage image, 
						  VkImageLayout currentLayout, VkImageLayout newLayout);

	VkImageSubresourceRange image_subresource_range(VkImageAspectFlags aspectMask);

	void copy_image_to_image(VkCommandBuffer cmd, VkImage source,
		VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);
}