# *Laboratorio 1 : Creación de Baash*
##### Integrantes: Fracchia Joaquin, Santiago Marini, Colazo Tomas.

# **Proceso de desarrollo**
Dividimos el proceso de desarrollo en varios módulos o partes que fuimos realizando según las dependencias del baash.



# MODULO PIPELINE:
En este modulo se consideran principalmente los TADS pipeline y scommand.

Tanto para la secuencia de strings (caso command) o secuencia de comandos simples (pipeline) decidimos implementarlas con una Cola, utilizando la liberia glib recomendada.
No tuvimos muchos problemas ya que las funciones auxiliares de dicha libreria nos facilitó mucho la implementación,para cualquiera de los dos TADS. 

El problema mas grande de éste modulo fueron los memory leaks, al utilizar bucles tuvimos que tener cuidado de no dejar bstrings sin liberar. Las funciones para convertir a strings tanto comandos como pipes fueron las mas complicadas para lograr pasar los tests.

# MODULO BUILTIN:
Para éste modulo los profesores nos dejaron en claro que podíamos cambiar la implementación, pero decidimos dejarla como estaba para no arriesgarnos.

Las funciones ```builtin_index``` y ```builtin_is_exit``` fueron las fáciles de implementar porque simplemente usabamos las funciones declaradas en ```command.h```
La función ```builtin_run``` hubo que pensarla más. Definimos el caso más sencillo, donde el comando es un exit, si no, al ser cd teníamos que obtener la ruta para llamar a ```chdir```. Utilizando la función de la cola Pop pudimos solucionarlo.
Luego manejamos posibles problemas al pasar argumentos y el módulo quedó finalizado.

# MÓDULO MAIN (BAASH):
En el ```main.c``` es donde corre nuestro Baash. Lo que hicimos en principio para testear los comandos internos ```cd``` y ```exit``` fue crear un pipeline, y luego llamar a la funciónes ```parser_new, parse_pipeline``` con stdin como argumento para que el parser (ya implementado por la cátedra) nos convierta el input en un pipeline listo para ejecutar. 

Para finalizar, llamábamos a builtin_run con el pipe de parámetro, y testeabamos cd y exit. Esa linea fué cambiada for ```execute_pipeline``` (quien usa como dependencia a builtin) una vez que ya empezamos con ese módulo.

# MÓDULO EXECUTE :
Éste módulo fué el más costoso y el que nos llevo más tiempo. Antes de empezar estudiamos bien las syscalls ```execvp,fork,pipe,etc```. 
La idea principal que usamos para desarrollarlo fue la siguiente:
Empezar con el caso más simple, que el pipeline tuviera un solo comando. Si el primer comando era cd o exit, ejecutabamos ```builtin_run```, en el caso contrario, llamamos a ```fork() ```  para manejar las redirecciones, preguntamos cual de las dos existe (o las 2) y abrimos los respectivos path, y usamos dup para terminar de redirigir. 
Necesitamos parsear el comando para pasárselo a ```execvp``` para que lo ejecutara.

Ésto lo logramos con una funcion auxiliar, que conviertiera a string el comando ignorando los redirectores > y <, y luego, rellenando el arreglo ```args[]``` que necesitaba ```execvp``` con los argumentos del comando.

Para el caso donde tenemos 2 comandos, llamamos a ```fork()```, inicializando un pipe, para utilizar los file descriptors y poder pasar inputs. El primer proceso, se encarga del primer comando, fijándose si necesitar usar redirecciones o no. Luego de lograr eso, parsea y utiliza execvp para el primer comando. Para el segundo comando necesitamos utilizar Pop para obtener el segundo comando, recibimos la informacion del file descriptor, y nuevamente parseamos y mandamos a execvp a ejecutar.


# **Resultado Final**
El resultado final fue una bash con la funcionalidad de correr 1 comando o como máximo dos.
Logramos cubrir los casos bordes, adquiriendo flexibilidad y robustez al programa. Tuvimos problemas con los procesos zombies, y luego de testear continuamente pudimos manejarlos.
