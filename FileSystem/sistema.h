#ifndef SISTEMA_H
#define SISTEMA_H

// Estructuras de Datos y Algoritmos - Curso 2023
// Tecnologo en Informatica FIng - DGETP - UTEC
//
// Trabajo Obligatorio
// sistema.h
// Modulo de Definición del File System.
//Integrantes: Eliana Garcia, Alejandra Pompilio


#include "definiciones.h"

typedef struct _sistema * Sistema;

typedef struct arbolNario * arbol;

typedef struct lista_directorios * listaDir;

typedef struct lista_archivos * listaArc;

void liberarListaArchivos(listaArc archivos);

void impRuta(Sistema s);

listaArc last(Sistema s);

int longitudT(Cadena texto);

void destruirArbol(arbol nodo);

void ordenarArchs(Sistema &s);

void ordenarDirs (Sistema &s);

listaDir lastDir(Sistema s);

TipoRet CREARSISTEMA(Sistema &s);
// Inicializa el sistema para que contenga únicamente al directorio RAIZ, sin subdirectorios ni archivos.

TipoRet DESTRUIRSISTEMA(Sistema &s);
// Destruye el sistema, liberando la memoria asignada a las estructuras que datos que constituyen el file system.

TipoRet CD (Sistema &s, Cadena nombreDirectorio);
// Cambia directorio.

TipoRet MKDIR (Sistema &s, Cadena nombreDirectorio);
// Crea un nuevo directorio.

TipoRet RMDIR (Sistema &s, Cadena nombreDirectorio);
// Elimina un directorio.

TipoRet MOVE (Sistema &s, Cadena nombre, Cadena directorioDestino);
// mueve un directorio o archivo desde su directorio origen hacia un nuevo directorio destino.

TipoRet DIR (Sistema &s, Cadena parametro);
// Muestra el contenido del directorio actual.

TipoRet CREATEFILE (Sistema &s, Cadena nombreArchivo);
// Crea un nuevo archivo en el directorio actual.

TipoRet DELETE (Sistema &s, Cadena nombreArchivo);
// Elimina un archivo del directorio actual, siempre y cuando no sea de sólo lectura.

TipoRet ATTRIB (Sistema &s, Cadena nombreArchivo, Cadena parametro);
//  cambia los atributos de un archivo perteneciente al directorio actual.

TipoRet IC (Sistema &s, Cadena nombreArchivo, Cadena texto);
// Agrega un texto al comienzo del archivo NombreArchivo.

TipoRet IF (Sistema &s, Cadena nombreArchivo, Cadena texto);
// Agrega un texto al final del archivo NombreArchivo.

TipoRet DC (Sistema &s, Cadena nombreArchivo, int k);
// Elimina los a lo sumo K primeros caracteres del archivo parámetro.

TipoRet DF (Sistema &s, Cadena nombreArchivo, int k);
// Elimina los a lo sumo K últimos caracteres del archivo parámetro.

TipoRet TYPE (Sistema &s, Cadena nombreArchivo);
// Imprime el contenido del archivo parámetro.

TipoRet SEARCH (Sistema &s, Cadena nombreArchivo, Cadena texto);
// Busca dentro del archivo la existencia del texto.

TipoRet REPLACE (Sistema &s, Cadena nombreArchivo, Cadena texto1, Cadena texto2);
// Busca y reemplaza dentro del archivo la existencia del texto1 por el texto2.

int longitudT(Cadena texto);
//cuenta la longitud de textos



#endif


