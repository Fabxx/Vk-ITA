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

#pragma once

#include "../include/vk_init.hpp"
#include <iostream>
#include <fstream>

VkFenceCreateInfo vkInit::fenceInfo(VkFenceCreateFlagBits flags) {
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = nullptr;
	fenceInfo.flags = flags;
	return fenceInfo;
}

VkSemaphoreCreateInfo vkInit::semaphoreInfo(VkSemaphoreCreateFlags flags) {
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = nullptr;
	semaphoreInfo.flags = flags;
	return semaphoreInfo;
}

VkCommandBufferBeginInfo vkInit::commandBufferBeginInfo(VkCommandBufferUsageFlagBits flags) {
	VkCommandBufferBeginInfo cmdBeginInfo = {};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.pNext = nullptr;
	cmdBeginInfo.pInheritanceInfo = nullptr;
	cmdBeginInfo.flags = flags;
	return cmdBeginInfo;
}

VkSemaphoreSubmitInfo vkInit::semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
{
	VkSemaphoreSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.semaphore = semaphore;
	submitInfo.stageMask = stageMask;
	submitInfo.deviceIndex = 0;
	submitInfo.value = 1;

	return submitInfo;
}

VkCommandBufferSubmitInfo vkInit::command_buffer_submit_info(VkCommandBuffer cmd)
{
	VkCommandBufferSubmitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	info.pNext = nullptr;
	info.commandBuffer = cmd;
	info.deviceMask = 0;

	return info;
}

VkSubmitInfo2 vkInit::submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo,
								  VkSemaphoreSubmitInfo* waitSemaphoreInfo)
{
	VkSubmitInfo2 info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	info.pNext = nullptr;

	info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
	info.pWaitSemaphoreInfos = waitSemaphoreInfo;

	info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
	info.pSignalSemaphoreInfos = signalSemaphoreInfo;

	info.commandBufferInfoCount = 1;
	info.pCommandBufferInfos = cmd;

	return info;
}

VkImageCreateInfo vkInit::image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
{
	VkImageCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.pNext = nullptr;

	info.imageType = VK_IMAGE_TYPE_2D;

	info.format = format;
	info.extent = extent;

	info.mipLevels = 1;
	info.arrayLayers = 1;

	/*
	* Per Multi sampling anti aliasing MSAA, noi non lo useremo, quindi lo mettiamo il campionamento a 
	* 1 campione per pixel.
	*/
	info.samples = VK_SAMPLE_COUNT_1_BIT;

	/*
	* optimal tiling, l'immagine è immagazzinata nel miglior formato per la GPU
	* Se vogliamo leggere i dati dell'immagine dalla CPU, dovremmo usare il tiling LINEAR, 
	* che trasforma i dati della GPU in un semplice array 2D.
	*/
	
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.usage = usageFlags;

	return info;
}

VkImageViewCreateInfo vkInit::imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
{
	// build a image-view for the depth image to use for rendering
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;

	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.image = image;
	info.format = format;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;
	info.subresourceRange.aspectMask = aspectFlags;

	return info;
}


VkResult vkInit::VK_CHECK(VkResult x) {
	if (x != VK_SUCCESS) {
		std::cerr << "Error occurred: " << x << "\n";
		abort();
	}
	return x;
}