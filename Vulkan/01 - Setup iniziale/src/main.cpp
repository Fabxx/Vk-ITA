#include "../include/vk_engine.hpp"

/*
* NOTA: La macro per il VMA va definita solo in un file CPP, e insieme va incluso l'header C
*       della libreria
* 
* Poi nei file dove richiami i tipi di dati del VMA includi la libreria hpp soltanto.
*/
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

int main(void) {

    VulkanEngine vkEngine;

    vkEngine.init();
    vkEngine.run();
    vkEngine.cleanup();
}