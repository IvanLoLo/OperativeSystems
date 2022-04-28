Grupo 12:
jubernalv-ilombana-vlozanol

Para ejecutar la interfaz y la búsqueda del tiempo de viaje entre zonas de Bogotá indicadas por el usuario, basta con ejecutar el comando "make".
Al salir de la interfaz ambos procesos serán terminados. Si se desea eliminar los archivos compilados, así como la FIFO creada para la comunicación entre procesos se debe llamar "make clean".

hashCode.c -> Genera los archivos binarios "viajes.txt", "posiciones.txt" que contiene la información de los viajes organizados en la hashTable.

searchHashCode.c -> Recibe los datos enviados por la pipe que contienen la información del viaje pedido, lo busca y envia por la pipe la información del tiempo.

p1-odProgram.c -> Programa main que contiene la interfaz de usuario y envia por la tuberia la información suministrada, una vez hecha la busqueda recibe el tiempo encontrado y lo muestra al usuario.

mkMyFIFO.c, rmMyFIFO.c -> Programas encargados de crear y eliminar la pipe, respectivamente.
