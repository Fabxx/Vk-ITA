/**
 * @file vk_init.hpp
 * @author Fabxx
 * @brief struttura che contiene funzioni per la creazione di Descriptor sets, per passare i dati alle shader.
 * 
 * 
 * @version 0.1
 * @date 2025-05-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <vector>
#include <VkBootstrap.h>
#include <span>

struct DescriptorLayoutBuilder {

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void add_binding(uint32_t binding, VkDescriptorType type);
    void clear();
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages, 
                                void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};


/*
* Struttura per allocare i descriptorSet.
* 
* Anziché avere un'unica pool che gestisce tutti i descriptor negli engine, 
* 
* creeremo diverse pool in base alle necessità. Poiché i descriptorSet devono essere
* pre-allocati, se usassimo una sola pool questo vorrebbe dire che avremmo bisogno
* di sapere a priori che tipo di descriptorSet andremmo ad usare.
* 
* Quando distruggi una pool, elimini TUTTI i descriptorSet ad essa associati.
* 
* Utile per come come descrittori usati per-frame, avendo descrittori usati
* per un solo fotogramma, allocati dinamicamente e prima di renderizzare il frame a schermo,
* possiamo distruggerli poiché non ci servono più.
* 
* Questa tecnica è raccomandata dai costruttori delle GPU, in quanto è il metodo
* più performance per gestire i DescriptorSet per-frame.
*/
struct DescriptorAllocator {

    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;

    void init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    void clear_descriptors(VkDevice device);
    void destroy_pool(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};
