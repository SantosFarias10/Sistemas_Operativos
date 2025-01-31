# API de Memoria

![Ejercicio 1](../VirtualizacionDeLaMemoria/imagenes/ej1mem.png)

**N** -> Program code.

**a** -> Programa code (es una variable global).

**a[N]** -> Program code (variable global).

**argc** -> Stack.

**argv** -> Stack.

**b** -> Stack.

**i** -> Stack.

**ret s** -> Stack.

**argv*** -> Puntero a Heap.

**argv**** -> Puntero a Heap.

**b*** -> Puntero a Heap.

**s** -> Los registros no se encuentra en la memoria.

---

![Ejercicio 2](../VirtualizacionDeLaMemoria/imagenes/ej2mem.png)

1. En el man page de `gets()` se dice que nunca se debe usar `gets()` ya que es imposible saber sin conocer los datos de antemano cuántos caracteres leerá, y porque `gets()` continuara almacenando caracteres más allá del final del buffer, es extramadamente peligroso usarlo. Se utiliza para romper la segurdad xd. Se debe usar `fgets()` en su lugar.

2. Está mal allocated la memoria de `*d` ya que, como esta pidiendo memoria para un `string`, le falta un byte más para almacenar la terminación del `string` '\0'.

3. `strdup()` reserva memoria, no es necesario ya que ya se reservo antes.

4. `sizeof(a) = 16 bytes`, sabes que un `sizeof(int) = 4 bytes` por lo que al intentar almacenar 42 enteros en el ultimo byte del arreglo, le faltan 3 bytes para poder guardarlo.

---

![Ejercicio 3](../VirtualizacionDeLaMemoria/imagenes/ej3mem.png)

* **(a)** **Falso**. `malloc()` es una función del user space que usa system calls en su implementación.

* **(b)** **Falso**. Contraejemplo: Si realizamos un `malloc(a)`, hacerle free y hacer otro `malloc(a)` pidiendo la misma cantidad de bytes.

* **(c)** **Verdadero**. Al pedir memoria usa la syscall `brk()`/`sbrk()`.

* **(d)** **Verdadero**. `free()` al igual que `malloc()` usa las syscall `brk()`/`sbrk()`.

* **(e)** **Falso**. No siempre lo es, como el mismo contraejemplo que **(b)**.

---

## Traducción de direcciones

![Ejercicio 4](../VirtualizacionDeLaMemoria/imagenes/ej4mem.png)

Registros base = 4096 y bounds (limite) = 256 ---> bounds = 4096 + 256 = 4352.

4096,

4096 + 5, 128

1096 + 8,

4096 + 10, 256

4096 + 13

---

![Ejercicio 5](../VirtualizacionDeLaMemoria/imagenes/ej5mem.png)

**Fracmentación Externa**: Si tenemmos una free list [P1 | FREE | P2 | FREE] donde el primer bloque de memoria libre es de 5 bytes y el segundo de 10 bytes, sumando un total de 15 bytes disponibles, si un proceso solicita 15 bytes no se le podra asignar ya que hay una fragmentación externa.

**Fracmentación Interna**: Si se le asigna 20 bytes a un proceso que solamente usa 15 bytes, los 5 bytes restantes son fragmentación interna.

---

![Ejercicio 6](../VirtualizacionDeLaMemoria/imagenes/ej6mem.png)

La **relocalización dinámica** usa los registros base/limite, para cada proceso se escribe y compila pensando que esta cargando en la dirección cero. Basicamente usa el hardware (registros) para obtener la dirección física.

La **relocalización estática** se usa el software para obtener la dirección física del proceso, perdiendo protección.

---

![Ejercicio 7](../VirtualizacionDeLaMemoria/imagenes/ej7mem.png)

* **(a)** **Verdadero**. Ya que si no es cierto podrían modificar su espacio.

* **(b)** **Falso**. Hay un juego de registro (base/limite) por cada CPU.

---

## Manejo del Espacio Libre

![Ejercicio 9](../VirtualizacionDeLaMemoria/imagenes/ej9mem.png)

* **(a)**

**First fit** Encuentra el primer bloque de memoria que sea suficientemente grande retorna la cantidad pedida al usuario. Por lo que le daria el bloque de 20 KB -> 13 KB, el de 10 KB -> 10 KB y 18 KB ->9 KB.

* **(b)**

**Best fit** Primero, busca a traves de la free list y encuentra los chunks de memoria libre que sean igual o más grande que el tamaño pedido. Entonces, retorna el más chico de ese grupo de candidatos. Por lo que le daria el bloque de 12 KB -> 12 KB, 10 KB -> 10 KB y el de 9 KB -> 9 KB.

* **(c)**

**Worst fit** Es el opuesto a best fit; encuentra el chunk mas grande y devuelve la cantidad solicitada. Le da el de 20 KB -> 12 KB, 18 KB -> 10 KB y el de 15 KB -> 9 KB.

* **(d)**

**Next fit** Mantiene un puntero adicional a la ubicación dentro del lista donde se miró por última vez. La idea es repartir las búsquedas de espacio libre por toda la lista de forma mas uniforme, evitando así fragmentación. Le da el de 20 KB -> 12 KB, 18 KB -> 10 KB y el de 9 KB -> 9 KB.

---

## Paginación

![Ejercicio 10](../VirtualizacionDeLaMemoria/imagenes/ej10mem.png)

