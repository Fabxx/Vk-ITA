#include "../include/vk_pipelines.hpp"
#include "../include/vk_init.hpp"
#include <fstream>


/*
* Funzione che carica il file delle shader.
* 
* Calcola la dimensione del file mettendo il cursore alla fine di esso
* 
* poi, SPIR-V si aspetta un buffer uint32_t allocato.
* 
* Infine si mette il cursore all'inizio per leggere tutti i byte della shader
* 
* per creare lo shader module.
* 
* La dimensione dello shader module deve essere in bytes, quindi moltiplichiamo
* la dimensione del buffer per il tipo di dato che vuole SPIR-V per l'allocazione.
* 
* Infine controlliamo che la shader venga compilata.
* 
*/
bool vkInit::load_shader_module(const char* filePath, VkDevice device, VkShaderModule* outShaderModule)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		return false;
	}

	size_t fileSize = (size_t)file.tellg();

	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

	file.seekg(0);
	file.read((char*)buffer.data(), fileSize);
	file.close();


	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.codeSize = buffer.size() * sizeof(uint32_t);
	createInfo.pCode = buffer.data();


	VkShaderModule shaderModule;
	vkInit::VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
	*outShaderModule = shaderModule;

	return true;
}