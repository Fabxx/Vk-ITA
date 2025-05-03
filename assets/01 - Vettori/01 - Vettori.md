**01.INTRODUZIONE AI VETTORI**

Un vettore possiede:

- Magnitudine (lunghezza)

- Direzione

Ciascun **componente** del vettore rappresenta una coordinata su un asse
(**x**, **y,** **z**, **w**).

Qui di seguito un immagine che mostra un esempio dei vettori **u**  e **v**

<img src="https://github.com/Fabxx/Vulkan-ITA/blob/main/assets/01 - Vettori/01.png?raw=true" alt="01" width="500" height="300"/><br>

Due vettori sono uguali solo se la dimensione, lunghezza e direzione di
entrambi coincidono.

I computer usano un sistema di coordinate per rappresentare i vettori,
poiché non possono rappresentarli geometricamente e hanno lo stesso
punto di origine.

Esempio di un vettore 2D:

v = ($v^{x},\ v^{y}$)

Considera la seguente figura che rappresenta **v**:

<img src="https://github.com/Fabxx/Vulkan-ITA/blob/main/assets/01 - Vettori/02.png?raw=true" alt="01" width="300" height="300"/><br>

Qui invece il vettore **v** è 3D.

v = ($v^{x},\ v^{y},\ v^{z}$)

<img src="https://github.com/Fabxx/Vulkan-ITA/blob/main/assets/01 - Vettori/03.png?raw=true" alt="01" width="300" height="300"/><br>

In base al numero di assi, il vettore può assumere più dimensioni.

Il sistema di coordinate prende come orientamento le tre dita
della mano sinistra dove:

- Il **pollice** è l'asse **Y**

- **L'indice** è l'asse **Z**

- Il **medio** è l'asse **X**

In **Direct3D** il sistema è left-handed, ma in altri sistemi è
possibile usare un sistema **Right-Handed** invertendo l'orientamento
delle assi.

NOTA: I singoli vettori possono avere una o più coordinate a seconda
della dimensione che li rappresenta. Ad esempio una sola freccia u in un
piano 3D può avere 3 coordinate, lunghezza, larghezza, altezza. Ogni
asse rappresenta una possibile coordinata per il vettore.

NON confondere il numero di dimensioni con il numero dei vettori! Se un
vettore è 3D non vuol dire che ci sono 3 frecce sul piano, ma una sola
freccia con 3 coordinate! Per renderelo più chiaro, le componenti
vengono scritte con il nome del vettore, e poi l'asse di riferimento.
