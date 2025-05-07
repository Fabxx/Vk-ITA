#include "../include/vk_descriptors.hpp"
#include "../include/vk_init.hpp"

/*
* Crea un bind e aggiungilo al vettore dei bindings
* Questo crea un VkDescriptorSetLayoutBinding, per ora 
* sappiamo solo il tipo di layout e il tipo del descrittore.
* 
* In questo caso essendo il primo binding, l'indice di esso è 0,
* mentre nel nostro caso, il tipo è lo stesso usato dalla shader,
* quindi VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, poiché la shader scrive
* un'immagine.
* 
* Se la shader cambia tipo di operazione, allora ne va applicato il flag corrispondente.
*/
void DescriptorLayoutBuilder::add_binding(uint32_t binding, VkDescriptorType type)
{
    VkDescriptorSetLayoutBinding newbind{};
    newbind.binding = binding;
    newbind.descriptorCount = 1;
    newbind.descriptorType = type;

    bindings.push_back(newbind);
}

/* Funzione che costruisce la disposizione del descriptor set.
*  Effettuiamo un loop in tutte le risorse inserite all'interno del descriptor,
*  per poi impostarne i flag degli stage.
* 
*  Per ogni binding in un descriptor set, i flag possono essere diversi in base al tipo di shader,
*  se fragment, vertex ecc. Nel nostro caso, non supporteremo i flag per binding specifici
*  ma li forzeremo per tutto il descriptor set.
*/
VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device, VkShaderStageFlags shaderStages,
                                                     void* pNext, VkDescriptorSetLayoutCreateFlags flags)
{
    for (auto& b : bindings) {
        b.stageFlags |= shaderStages;
    }

    VkDescriptorSetLayoutCreateInfo info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    info.pNext = pNext;

    info.pBindings = bindings.data();
    info.bindingCount = (uint32_t)bindings.size();
    info.flags = flags;

    VkDescriptorSetLayout set;
    vkInit::VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

    return set;
}


// Reset dei bindings passati ad una shader.
void DescriptorLayoutBuilder::clear()
{
    bindings.clear();
}



/*
* Funzioni di creazione e distruzione delle descriptorPools.
* 
* la funzione clear effettua un reset, distruggendo i descrittori allocati nella
* pool, ma non distrugge la pool stessa.
* 
* Per creare una pool utilizziamo il tipo PoolSizeRatio che contiene:
* 
* - Tipo del descrittore
* 
* - Rateo per moltiplicare il parametro maxSets.
* 
* Questo ci fa controllare la grandezza della pool in base a quanti tipi di bindings
* quella pool possiede.
*/
void DescriptorAllocator::init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (PoolSizeRatio ratio : poolRatios) {
        poolSizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = uint32_t(ratio.ratio * maxSets)
            });
    }

    VkDescriptorPoolCreateInfo pool_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    pool_info.flags = 0;
    pool_info.maxSets = maxSets;
    pool_info.poolSizeCount = (uint32_t)poolSizes.size();
    pool_info.pPoolSizes = poolSizes.data();

    vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
}

void DescriptorAllocator::clear_descriptors(VkDevice device)
{
    vkResetDescriptorPool(device, pool, 0);
}

void DescriptorAllocator::destroy_pool(VkDevice device)
{
    vkDestroyDescriptorPool(device, pool, nullptr);
}

/*
* Inizializza la struttura di allocazione di memoria del descriptor Set.
* 
*/
VkDescriptorSet DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout)
{
    VkDescriptorSetAllocateInfo allocInfo = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet ds;
    vkInit::VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));

    return ds;
}

