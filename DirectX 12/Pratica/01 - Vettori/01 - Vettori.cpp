/**
 * @file main.cpp
 * @author Fabxx
 * @brief
            XMVECTOR: vettorie DirectX SIMD (SSE2 streaming SIMD extension 2)
                      è un dato allineato a 16 bit, questo viene fatto in automatico per variabili
                      glocali e locali. Il vettore ha dimensione flessibile.

                      Per le classi è consigliato usare vettori di dimensione fissa, come XMFLOAT2, XMINT3 ecc.
                      
                      Per sfruttare le istruzioni SIMD peri calcoli, lavoriamo sempre sugli 
                      le funzioni Load e store possono importare/esportare i valori dagli XMVECTOR
                      ai vettori di dimensione fissa.


                      i parametri input XMVECTOR per garantire compatibilità e sfruttare SIMD sono le seguenti:

                      - I primi tre parametri devono essere di tipo FXMVECTOR
                      - il quarto parametro deve essere di tipo GXMVECTOR
                      - il quinto e sesto parametro devono essere di tipo HXMVECTOR
                      - i restanti parametri CXMVECTOR

                      questo non vale per i parametri output.

                      In questo codice verranno mostrate le operazioni sui vettori.
 * @version 0.1
 * @date 2025-04-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <iostream>
#include <DirectXPackedVector.h>

using namespace DirectX;

int main()
{
    // Controlla supporto per SSE2 (Pentium4, AMD K8, e superiore).
    if (!XMVerifyCPUSupport())
    {
        std::cout << "directx math not supported\n";
        return 0;
    }

    XMVECTOR vec_x{ XMVectorSet(5.0f, 14.0f, 11.0f, 1.0f) }, 
             vec_y{ XMVectorSet(70.0f, 20.0f, 98.0f, 0.0f) };

    XMFLOAT4 final_vector {};

    XMStoreFloat4(&final_vector, XMVectorAdd(vec_x, vec_y));

    std::cout << "x: " << final_vector.x << "\n" << "y: " << final_vector.y << "\n" << "z: " << final_vector.z << "\n";
    
    final_vector = {0.0f, 0.0f, 0.0f, 0.0f};

    XMStoreFloat4(&final_vector, XMVectorSubtract(vec_x, vec_y));

    std::cout << "x: " << final_vector.x << "\n" << "y: " << final_vector.y << "\n" << "z: " << final_vector.z << "\n";

    XMStoreFloat4(&final_vector, XMVectorMultiply(vec_x, vec_y));

    std::cout << "x: " << final_vector.x << "\n" << "y: " << final_vector.y << "\n" << "z: " << final_vector.z << "\n";

    XMStoreFloat4(&final_vector, XMVector4Dot(vec_x, vec_y));

    // il prodotto scalare ritorna un singolo valore immagazzinato su x.
    std::cout << "x: " << final_vector.x << "\n";

    return 0;
}
