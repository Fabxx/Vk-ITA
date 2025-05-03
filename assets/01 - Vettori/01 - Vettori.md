**01.INTRODUZIONE AI VETTORI**

Un vettore possiede:

- Magnitudine (lunghezza)

- Direzione

Ciascun **componente** del vettore rappresenta una coordinata su un asse
(**x**, **y,** **z**, **w**).

![](media/image1.png){width="3.0282797462817146in"
height="1.4913254593175853in"}

Due vettori sono uguali solo se la dimensione, lunghezza e direzione di
entrambi coincidono.

I computer usano un sistema di coordinate per rappresentare i vettori,
poiché non possono rappresentarli geometricamente e hanno lo stesso
punto di origine.

Esempio di un vettore 3D:

v = ($v^{x},\ v^{y},\ v^{z}$)

Considera la seguente figura che rappresenta un vettore 2D **v**:

![](media/image2.png){width="1.7490671478565178in"
height="1.4539391951006124in"}

Qui il vettore **v** è 3D. Le coordinate in questa immagine sono "**Left
handed**".

![](media/image3.png){width="2.185566491688539in"
height="1.3741535433070866in"}

Ovvero un sistema di coordinate che prende come orientamento le tre dita
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
