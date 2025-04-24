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

                      In questo codice verrà mostrata solo la somma, ma l'API fornisce anche le
                      altre operazioni.
 * @version 0.1
 * @date 2025-04-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <iostream>
#include <vector>
#include <DirectXPackedVector.h>

using namespace DirectX;

/*
* Somma il vettore vuoto con i vettori inizializzati. Il risultato è un nuovo vettore.
* 
* NOTA: Non fare mai release() dei puntatori all'interno delle funzioni,
* a meno che questi non siano delle copie e non dei riferimenti.
* 
* Se passati per riferimento, release() distrugge il puntatore dallo
* stack di provenienza, non nello stack della funzione stessa!
* 
* unique_ptr non è un puntatore crudo come in C con *, ma è
* un oggetto di tipo puntatore che contiene std::vector
* 
* se vogliamo accedere al riferimento di vector senza creare una copia, 
  
  dobbiamo usare & per accedere al riferimento.
*/
void sum(std::unique_ptr<std::vector<XMVECTOR>> &vectors) {
    
    XMVECTOR result {XMVectorZero()}; // 0, 0, 0, 0

    for (size_t i { 0 }; i < vectors->size(); i++) { 
        result = XMVectorAdd(result, vectors->at(i));
    }
    std::cout << "x: " << result.x << "\n" << "y: " << result.y << "\n" 
              << "z: " << result.z;
}

int main()
{
    // Controlla supporto per SSE2 (Pentium4, AMD K8, e superiore).
    if (!XMVerifyCPUSupport())
    {
        std::cout << "directx math not supported\n";
        return 0;
    }

    // set di vettori X, Y, Z, make_unique crea uno unique_ptr più sicuro.
    auto vectors{ std::make_unique<std::vector<XMVECTOR>>()};

    vectors->push_back(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
    vectors->push_back(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    vectors->push_back(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));

    XMFLOAT4 final_vector {};

    XMStoreFloat4(&final_vector, sum(vectors));

    vectors.release();

    return 0;
}
