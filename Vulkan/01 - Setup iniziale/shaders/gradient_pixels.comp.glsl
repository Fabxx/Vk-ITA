/*
    Specifichiamo prima la versione di GLSL da usare, poi impostiamo la dimensione del gruppo di lavoro
    che lavorerà sui pixel in 16x16 corsie su X e Y, usando 16 thread per asse.

    Main contiene il codice vero e proprio, dove si ottiene la coordinata dell'elemento della texture 

    (texel sta per texture element). in questo modo otteniamo l'indice della corsia attuale, e otteniamo la posizione
    del pixel attuale, ritornando il numero a mo di risoluzione (ad la coordinata può essere 128x512)

    Con il secondo layout specifichiamo che l'immagine 2D in questione appartiene al descriptor set 0
    e un binding 0 su quel set.

    In vulkan ogni descriptor set può avere un numero di agganci, che sono i dati agganciati a quel set.

    Quindi, questo è 1 set all'indice 0, che contiene una singola immagine nel binding 0.


    Il codice in questo caso prende le coordinate degli elementi sugli assi X e Y,

    e se i thread hanno cominciato a muoversi nei pixel, per ogni pixel genera un colore
    dividendo la coordinata attuale per la dimensione dell'asse.

    E' un metodo per generare un gradiente sui pixel.

    infine l'immagine generata viene immagazzinata, per poi essere passata alla chain.


    nota che il vettore è 4D, ma sta generando solo colori per R e G, B è a 0 e la trasparenza è a 1.0,
    ovvero il colore è opaco.

*/


#version 460
#extension GL_KHR_vulkan_glsl : enable

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba16f, set = 0, binding = 0) uniform image2D image;


void main() 
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(image);

    if (texelCoord.x < size.x && texelCoord.y < size.y)
    {
        vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

        if (gl_LocalInvocationID.x != 0 && gl_LocalInvocationID.y != 0)
        {
            color.x = float(texelCoord.x)/(size.x);
            color.y = float(texelCoord.y)/(size.y);
        }
    
        imageStore(image, texelCoord, color);
    }
}
