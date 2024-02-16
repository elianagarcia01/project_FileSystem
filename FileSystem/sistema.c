// Estructuras de Datos y Algoritmos - Curso 2023
// Tecnologo en Informatica FIng - DGETP - UTEC
//
// Trabajo Obligatorio
// sistema.c
// Modulo de Implementacion del File System.
//Integrantes: Eliana Garcia, Alejandra Pompilio

#include "definiciones.h"
#include "sistema.h"
#include <iostream>
#include <cstring>
#include <string>

using namespace std;

struct lista_archivos{
    bool permisoW=true;
    char nombreArch[20];
    char contenido[TEXTO_MAX];
    listaArc sig;
    listaArc ant;
};

struct lista_directorios{
    char nombreD[16];
    listaDir sig;
    listaDir ant;
};

struct _sistema{ //Directorio/s
	// aquí deben figurar los campos que usted considere necesarios para manipular el sistema de directorios
	// Se deberan crear nuevos modulos
    arbol raiz;
    arbol actual;
};

struct arbolNario{
    arbol ph;//primer hijo
	arbol sh;//siguiente hermano
	arbol padre;
	char nombreDir[16];//nombre de directorio
	listaArc listaArchivos;
};

TipoRet CREARSISTEMA(Sistema &s){
// Inicializa el sistema para que contenga únicamente al directorio RAIZ, sin subdirectorios ni archivos.
// Para mas detalles ver letra.

    s = new(_sistema);
    arbol diraiz= new(arbolNario);

    s->raiz= diraiz;
    s->actual= diraiz;
    s->actual->listaArchivos = NULL;

    strcpy(s->raiz->nombreDir,"RAIZ");
    s->raiz->padre = NULL;
    s->raiz->ph = NULL; //DIRAIZ= directorio raiz
    s->raiz->sh = NULL;

	return OK;
}

// Función que libera la memoria de una lista de archivos
//La lista de archivos no está vacía
void liberarListaArchivos(listaArc archivos) {
    while (archivos != NULL) {
        listaArc temp = archivos;
        archivos = archivos->sig;
        delete temp;
    }
}

// Función para destruir un árbol n-ario
void destruirArbol(arbol nodo) {
    if (nodo != NULL) {
        // Liberar lista de archivos en el nodo actual
        liberarListaArchivos(nodo->listaArchivos);

        // Llamar recursivamente para los hijos y hermanos del nodo actual
        destruirArbol(nodo->ph);
        destruirArbol(nodo->sh);

        // Liberar el nodo actual
        delete nodo;
    }
}

TipoRet DESTRUIRSISTEMA(Sistema &s){
// Destruye el sistema, liberando la memoria asignada a las estructuras que datos que constituyen el file system.

    if (s != NULL) {
        // Llamar a la función de destruir el árbol n-ario desde la raíz
        destruirArbol(s->raiz);
        // Liberar la memoria asignada para el sistema
        delete s;
    }

	return OK;
}

TipoRet CD(Sistema &s, Cadena nombreDirectorio) {
    arbol actual = s->actual;

    if (strcmp(nombreDirectorio, "..") == 0) {
        if (s->actual == s->raiz) {
            printf("Estoy en la raiz, no se puede retroceder.\n");
            return ERROR;
        } else {
            s->actual = s->actual->padre;

            return OK;
        }
    } else {
        if (s->actual->ph != NULL) { //Si hay 1 hijo o más
            if (strcmp(s->actual->ph->nombreDir, nombreDirectorio) == 0) { //Si está en el ph
                s->actual = s->actual->ph;

                return OK;
            } else {
                arbol aux = s->actual->ph; //Sino, buesco en los sh del ph

                while (aux != NULL) { //MIentras hay hermanos
                    if (strcmp(aux->nombreDir, nombreDirectorio) == 0) { //Comparo si lo encuentro
                        s->actual = aux; //Actualizo

                        return OK;
                    }
                    aux = aux->sh;
                }
            }
            if (strcmp(s->actual->nombreDir, nombreDirectorio) != 0) { //Si tiene hermanos y no lo encontró
                printf("No existe: %s\n", nombreDirectorio);
                return ERROR;
            }
        }
        printf ("No existe: %s\n", nombreDirectorio);
        return ERROR; //Si no tiene ph ni sh
    }

}
TipoRet MKDIR(Sistema &s, Cadena nombreDirectorio) {
    arbol aux = s->actual;
    arbol comienzo = s->actual->ph;

    if (strcmp(nombreDirectorio, "RAIZ") == 0) {
        printf("Nombre de directorio invalido, intente con otro\n");
        return ERROR;
    } else {
        if (aux->ph == NULL) {
            arbol newDir = new(arbolNario);
            strcpy(newDir->nombreDir, nombreDirectorio);
            newDir->sh = NULL;
            newDir->ph = NULL;
            newDir->listaArchivos = NULL;
            newDir->padre = aux;
            aux->ph = newDir;
            return OK;
        } else {
            arbol aux2 = aux->ph;
            arbol anterior = NULL;

            while (aux2 != NULL) {
                if (strcmp(aux2->nombreDir, nombreDirectorio) == 0) {
                    printf("El directorio actual ya contiene un subdirectorio con ese nombre\n");
                    return ERROR;
                }
                anterior = aux2;
                aux2 = aux2->sh;
            }

            arbol newDir = new(arbolNario);
            strcpy(newDir->nombreDir, nombreDirectorio);
            newDir->sh = NULL;
            newDir->ph = NULL;
            newDir->listaArchivos = NULL;
            newDir->padre = aux;

            // Agregar al final de la lista de hermanos
            anterior->sh = newDir;

            return OK;
        }
    }
}

TipoRet RMDIR (Sistema &s, Cadena nombreDirectorio){
//elimina un subdirectorio hijo del directorio actual.
//Al eliminar un directorio se eliminarán todos sus subdirectorios junto con los archivos correspondientes
//El directorio RAIZ no podrá ser eliminado

    arbol aux = s->actual;
    arbol comienzo = s->actual->ph;

    if (strcmp(nombreDirectorio, "RAIZ") == 0) {
        printf("Nombre de directorio invalido, intente con otro\n");
        return ERROR;
    } else {
        if (aux->ph == NULL) {
            printf("No existe el subdirectorio a eliminar\n");
            return ERROR;
        } else {
            bool encontroDir=false;
            arbol aux2 = aux->ph;
            arbol anterior = NULL;
            //arbol antAux;

            while (aux2 != NULL && encontroDir==false) {
                if (strcmp(aux2->nombreDir, nombreDirectorio) == 0) {
                    encontroDir=true;
                }
                if(encontroDir==false){//si no se encontro avanzo
                    anterior = aux2;
                    aux2 = aux2->sh;
                }
            }
            if(encontroDir==true){
                //despues de haber eliminado los subdirectorios y archivos tendre que eliminar este directorio actual
                //y dejar bien apuntados los punteros dependiendo en que situacion me encuentre (primero,medio,ultimo)

                if(anterior==NULL && aux2->sh==NULL){//si solo hay uno solo

                    s->actual->ph=NULL;
                }
                else{
                    if(anterior ==NULL){//si elimino el primero

                       if (aux2->sh == NULL) {
                            // No hay más hijos
                            s->actual->ph=NULL;

                        } else {//anterior es null pero sh existe

                           s->actual->ph=aux2->sh; // El ph ahora será el siguiente hermano
                        }
                    }
                    else{
                        if(aux2->sh==NULL){//si elimino el ultimo directorio
                            anterior->sh=NULL;
                        }
                        else{//elimino en el medio
                            anterior->sh= aux2->sh;
                        }
                    }
                }
                aux2->padre=NULL;

                //elimino ese arbol, incluido archivos y subdirectorios y el directorio padre (aux2)
                destruirArbol(aux2);

                return OK;
            }
            else{
                printf("No existe el subdirectorio a eliminar\n");
                return ERROR;
            }
        }
    }
}

TipoRet MOVE (Sistema &s, Cadena nombre, Cadena directorioDestino){
// mueve un directorio o archivo desde su directorio origen hacia un nuevo directorio destino.

    //buscamos si es un directorio, sino vamos a recorrer la lista de archivos (parametro nombre)
    arbol aux = s->actual; //Var para recorrer
    arbol aux2 = aux->ph; //Var para manejar directorios
    arbol anteriorDir = NULL;


    bool encontro= false;

    while (aux2 != NULL && encontro==false) { //Recorro ph y sh si tiene
        if (strcmp(aux2->nombreDir, nombre) == 0) {
            encontro=true;
        }
        if(encontro==false){
            anteriorDir = aux2;
            aux2 = aux2->sh;
        }
    }

    if(encontro==false){//no encontro dir, vamos a buscar si es un arch
       bool encontroArch = false;

        listaArc auxS =s->actual->listaArchivos; //var para recorrer los archivos

        listaArc anterior=NULL; //Archivo anterior
        //recorro la lista general de archivos hasta el final o hasta encontrar archivo
        while(auxS !=NULL && encontroArch==false){

            int result = strcmp(auxS->nombreArch,nombre);
            if(result==0){//son iguales
                encontroArch=true;//encontre el archivo
            }
            else{//son distintos
                anterior = auxS;
                auxS= auxS->sig;//avanzo de arch para ver si lo encuentro
            }
        }

        if(encontroArch==false){ //Si no lo encontré
            printf("No existe un archivo o subdirectorio llamado %s, en el directorio actual \n", nombre);
            return ERROR;

        }
        else{ //Si encontré el archivo:
            //primero vamos a actualizar los punteros
            //Para acorarme: anterior= auxS->ant

            if (auxS->sig == NULL && anterior == NULL) {
                // Si solo hay un archivo, la lista la dejo en NULL
                s->actual->listaArchivos = NULL;
            } else {
                // Si estoy en el medio de la lista
                if (auxS->sig != NULL && anterior != NULL) {
                    // Actualizo los punteros para desenlazar el archivo en el medio de la lista
                    anterior->sig = auxS->sig;
                    auxS->sig->ant = anterior;
                } else {
                    // Si es el último archivo de la lista
                    if (auxS->sig == NULL) {
                        // Actualizo los punteros para desenlazar el último archivo
                        anterior->sig = NULL;
                    } else {
                        // Si el archivo es el primero de la lista
                        if (anterior == NULL) {
                            // Actualizo los punteros para desenlazar el primer archivo
                            s->actual->listaArchivos = auxS->sig;
                            auxS->sig->ant = NULL;
                        }
                    }
                }
            }
            //ahora vamos a moverlo
            //separamos en tokens la ruta, para ver que este correcta
            arbol regreso=s->actual; //me guardo para regresar desde donde me quede (el move mueve, pero el actual se queda posicionado donde ya estaba)

            s->actual=s->raiz; //ESTO ES PARA QUE VUELVA A LA RAIZ (porque el cd avanza desde la raiz)

            Sistema sist=s;
            char *pch = strtok(directorioDestino, "/");
            TipoRet ret=NO_IMPLEMENTADA;

            if ((strcmp (pch,s->raiz->nombreDir)!=0)){ //Si el primer directorio en el camino NO es RAIZ
                printf ("La ruta debe comenzar desde la raiz.\n");
                return ERROR;
            }

            pch = strtok(NULL, "/"); //avanzo 1 para que apunte al siguiente
            //entonces como mi actual ya lo inicialice en raiz, al hacer cd avanza hasta el 2do parametro

            while ((pch != NULL) && ( ret!=ERROR) ) {
                ret=CD (sist, pch); //Busco el directorio destino con CD una y otra vez
                if(ret==OK){
                    pch = strtok(NULL, "/"); //avanzamos a la siguiente dirección si CD==OK
                }
            }

            if (ret==OK || (strcmp (directorioDestino,"RAIZ")==0)){ //Si CD==OK o si la ruta es hacia RAIZ

                ret=ATTRIB(sist,nombre,(Cadena)"+W");//cambiamos el permiso del archivo a LyE
                if(ret==OK){ //Si attrib==OK, significa que si hay un archivo con ese nombre

                    listaArc l=sist->actual->listaArchivos; //Me defino una lista aux

                    while (l->sig!=NULL){ //Recorro al l aux
                        l=l->sig;
                    }
                    l->sig=auxS;  //Aclaracion: auxS es el archivo que movemos
                    auxS->sig=NULL;
                    auxS->ant=l;
                    //Inserté el archivo al final de mi lista de archivos
                    ret=DELETE(sist,nombre); //Sobreescribimos

                    sist->actual=regreso;
                }
                else{ //Si el ret de ATTRIB==ERROR
                    //Significa que no hay que reemplazar, solo inserto

                    listaArc l=sist->actual->listaArchivos; // lista aux

                    if(l==NULL){//si la lista de archivos esta vacia
                        sist->actual->listaArchivos=auxS;
                        auxS->ant=NULL;
                        auxS->sig=NULL;
                    }
                    else{
                        while (l->sig!=NULL){ //Recorro al l aux
                            l=l->sig;
                        }
                        l->sig=auxS;  //Aclaracion: auxS es el archivo que movemos
                        auxS->sig=NULL;
                        auxS->ant=l;
                        //Inserto al final.
                    }
                    sist->actual=regreso;//regreso al origen (lo muestra la funcion move en la letra)
                }
            }
            else{ //Si CD==ERROR
                printf ("No existe el directorio destino.\n");
                return ERROR;
            }

        }

    }
    else{ //Si encontró el directorio a mover
        //Verificar que el destino no sea un subdirectorio del directorio origen
        arbol origen=aux2->padre; //El directorio origen del directorio a mover
        arbol regreso=s->actual;
        s->actual=s->raiz;

        Sistema sist=s; //Sistema auxiliar
        char *pch = strtok(directorioDestino, "/"); //Separo la ruta por '/'

        TipoRet ret=NO_IMPLEMENTADA; //retorno auxiliar

        if ((strcmp (pch,s->raiz->nombreDir)!=0)){ //Si el primer directorio en el camino NO es RAIZ
            printf ("La ruta debe comenzar desde la raiz.\n");
            return ERROR;
        }
        //que avance al 2do token si es que lo hay
        if(pch!=NULL){
            pch = strtok(NULL, "/");//lo dejo en el 2do token
        }
        else{
            ret=CD (sist, directorioDestino);//para el caso en el que el path es RAIZ
        }

        while ((pch != NULL) && ( ret!=ERROR) ) {
            ret=CD (sist, pch); //Busco el directorio destino con CD una y otra vez
            if(ret==OK){
                pch = strtok(NULL, "/"); //avanzamos a la siguiente dirección si CD==OK
            }
        }

        if (ret==ERROR){
            return ERROR;
        }else{ //ret==OK
            if (strcmp(sist->actual->nombreDir,origen->nombreDir)==0){
                printf ("El directorio destino es un subdirectorio del directorio origen.\n");
                return ERROR;
            }else{ //Si el directorio a mover tiene el mismo nombre que el padre y el destino es el abuelo
                if ((strcmp(aux2->nombreDir,origen->nombreDir)==0) && (strcmp(sist->actual->nombreDir,origen->padre->nombreDir)==0)){
                    printf ("Se provoca la perdida del directorio actual\n");
                    return ERROR;
                }else{ //Todo OK, podemos mover
                    //actualizo los punteros, desenlazo del origen a aux2 (el dir que quiero mover)

                    if (anteriorDir == NULL && aux2->sh == NULL) { // si solo hay uno solo
                        origen->ph = NULL; // que el ph sea NULL
                    } else {
                        if (anteriorDir == NULL) { // si muevo el primero
                            origen->ph = aux2->sh; // El ph ahora será el siguiente hermano
                        } else {
                            //si anterior  tiene, pero sh no
                            if(aux2->sh==NULL){//si elimino el ultimo directorio
                                anteriorDir->sh=NULL;
                            }
                            else{//anterior tiene y sh tiene
                                anteriorDir->sh = aux2->sh; // si elimino en el medio
                            }
                        }
                    }
                    //Ver si hay que reemplazar
                    //Recorrer el destino
                    arbol destino=sist->actual->ph; //Voy a recorrer los subdirectorios del destino
                    arbol ant = NULL;

                    if(destino !=NULL){//si el destino TIENE hijo

                        while ((destino!= NULL) && (strcmp(aux2->nombreDir,destino->nombreDir) != 0) ) {
                            //aux2 es el dir a mover
                            //Si tienen el mismo nombre, voy a reemplazarlo
                            ant=destino;
                            destino = destino->sh;
                        }

                        if (destino!=NULL && (strcmp(aux2->nombreDir,destino->nombreDir) == 0) ){ //Si salió del while pq son iguales (destino no llego a null)

                            //PIMERO VOY A ELIMINAR
                            TipoRet deldir;
                            deldir=RMDIR(sist, nombre);//Eliminamos el primero que tenga el mismo nombre (el primero repetido)

                            //AHORA QUE YA ELIMINA, AGREGO AL COMIENZO
                            aux2->sh = sist->actual->ph;
                            sist->actual->ph = aux2;
                            aux2->padre = sist->actual;
                        }
                        else{//salio del while porque llego al final no hay que reemplazar
                            if(destino==NULL){//recorrio todos los hermanos y no encontro cincidencia
                                //voy a agregarlo al final de los hermanos
                                ant->sh = aux2;
                                aux2->sh = NULL;
                                aux2->padre= sist->actual;
                            }
                        }
                    }//SI EL DESTINO ESTA VACIO
                    else{ //No hay que reemplazar, simplemente movemos
                        sist->actual->ph=aux2;
                        aux2->sh=NULL;
                        aux2->padre= sist->actual;
                    }
                }
            }
        }
        sist->actual=regreso;
    }

    return OK;
}

void impRuta(Sistema s) {
    // Antes de usarla se debe guardar una variable con la pos actual para recuperarla,
    // ya que al finalizar esta funcion te deja parado en raiz y poner un printf ("\n");
    // apenas se llama a la funcion.

    if (s->actual!= NULL) {
        // Caso base: si el directorio actual es NULL, detener la recursión

        if (strcasecmp(s->actual->nombreDir, s->raiz->nombreDir) != 0) {
            arbol aux = s->actual;
            s->actual = s->actual->padre;  // Retroceder al directorio padre antes de la llamada recursiva
            impRuta(s);
            printf("/%s", aux->nombreDir);
        }else{
            printf ("RAIZ");
        }


    }
}

listaArc last(Sistema s) {
    Sistema auxS = s;
    listaArc primElem = auxS->actual->listaArchivos;
    listaArc retorno = NULL;

    while (primElem->sig != NULL) {
        primElem = primElem->sig;
    }

    retorno = primElem;
    return retorno;
}

listaDir lastDir(listaDir l) {

    listaDir primElem = l;

    while (primElem->sig != NULL) {
        primElem = primElem->sig;
    }
    return primElem;
}

void ordenarArchs (Sistema &s){ //Ordenar lista de ARCHIVOS para DIR

    Sistema sys=s;

    arbol direc=sys->actual;
    listaArc primElem=s->actual->listaArchivos;
    if((sys->actual->listaArchivos != NULL) && (sys->actual->listaArchivos->sig != NULL)){

        //Ordeno con burbuja, si no es vacía o si tiene + de elemento

        listaArc pivote=sys->actual->listaArchivos, actual=NULL,ultElem=NULL; //pivote=primerElemento inicialmente.
        char nombreTemporal [20];
        bool permisoTemporal;
        ultElem= last(sys);



        while (pivote!=NULL){
            actual=pivote->sig; //Actual es el siguiente elemento
            while (actual!=NULL){
                if (strcmp(pivote->nombreArch,actual->nombreArch)>0){

                    strcpy (nombreTemporal,pivote->nombreArch);
                    permisoTemporal=pivote->permisoW;
                    //Copié pivote a la auxiliar

                    strcpy (pivote->nombreArch,actual->nombreArch);  //pivote->nombreArch=actual->nombreArch;
                    pivote->permisoW=actual->permisoW;
                    //Copié actual a pivote

                    strcpy(actual->nombreArch,nombreTemporal); //actual->nombreArch=aux->nombreArch;
                    actual->permisoW=permisoTemporal;
                    //Copié aux(pivot anterior) a Act

                }
                actual=actual->sig;
            }
            ultElem->sig=NULL;
            pivote=pivote->sig;
            if (pivote!=NULL){

                strcpy(nombreTemporal,pivote->nombreArch); //nombreTemporal=pivote->nombreArch;
                permisoTemporal=pivote->permisoW;
            }


        }

        while (primElem != NULL){
            printf("%s     ", primElem->nombreArch);
            if (primElem->permisoW == true){
                printf("Lectura/Escritura\n");
            } else {
            printf("Lectura\n");
            }
            primElem = primElem->sig;
        }
        //printf ("He salido de la imp de archs\n");
        sys->actual=direc;
    }else{
        if (sys->actual->listaArchivos->sig==NULL){
            printf ("%s     ",sys->actual->listaArchivos->nombreArch);
            if (primElem->permisoW == true){
                printf("Lectura/Escritura\n");
            } else {
                printf("Lectura\n");
            }
        }
    }
    //delete sys;

}

void ordenarDirs (Sistema &s){ //Ordena la lista de DIRECTORIOS para DIR

    Sistema Sys= s;

    arbol actual=Sys->actual;
    arbol origen=s->actual;

    if (actual->ph!=NULL){ //Si tiene un subdirectorio
        if (actual->ph->sh==NULL ){ //Si solo tiene un subdirectorios
            printf ("%s",Sys->actual->ph->nombreDir);

        }else{
            if (actual->ph->sh!=NULL){ //Si tiene mas de 1 directorio

                arbol aGuardar=NULL;
                aGuardar=Sys->actual->ph; //Empiezo a guardar desde el ph.
                listaDir l; //Me creo una lista auxiliar
                l= new (lista_directorios);//lista_directorios==un NODO de la lista
                strcpy (l->nombreD,aGuardar->nombreDir); //Me guardo el primer elemento de la lista de directorios
                listaDir primElem=l;
                //Voy a copiar los directorios a la lista

                //Me guardo el nombre del primer directorio
                aGuardar=aGuardar->sh; //Avanzo

                while (aGuardar!= NULL) {

                    l->sig = new (lista_directorios); // Asignar memoria para el segundo nodo donde estoy paradad
                    l = l->sig; // Avanzar en la lista
                    strcpy(l->nombreD, aGuardar->nombreDir); // Copiar el nombre de dir a la lista de directorios
                    aGuardar = aGuardar->sh; // Avanzar por sh, en el árbol

                }
                //Luego de que copié los nombres de los directorios a la lista "l", voy a ordenar la lista:
                l=primElem;
                if((l != NULL)){
                    //printf ("Primer elem:%s\n",l->nombreD);
                    //Ordeno con burbuja, si no es vacía o si tiene + de 1 elemento
                    listaDir ultElem= lastDir(l);
                    //printf ("Ult elem:%s\n",ultElem->nombreD);
                    listaDir pivote=l, actual=NULL;//ultElem=primElem; //pivote=primerElemento inicialmente.
                    char nombreTemporal [20];
                    //While para ir hasta el ultimo elemento de la lista de directorios
                    //listaDir aux=primElem; //aux para recorrer hasta el ult elemento de la l de dirs

                    while (pivote != ultElem && pivote != NULL) {
                    actual = pivote->sig; // Actual es el siguiente elemento a pivote

                        while (actual != NULL) {
                            if (strcmp(pivote->nombreD, actual->nombreD) > 0) {
                                strcpy(nombreTemporal, pivote->nombreD);
                                strcpy(pivote->nombreD, actual->nombreD);
                                strcpy(actual->nombreD, nombreTemporal);
                            }
                            actual = actual->sig;
                        }
                        if (pivote != NULL && pivote->sig != NULL) {
                            pivote = pivote->sig;
                            strcpy(nombreTemporal, pivote->nombreD);
                        }
                    }
                    //printf ("Luego del while\n");
                    l=primElem;
                    while (l != NULL){
                        printf("%s\n", l->nombreD);
                        l = l->sig;
                    }

                }
            }

        }
    }
    //s->actual=origen;
    //delete Sys;
}


TipoRet DIR (Sistema &s, Cadena parametro){
    char parS[]= "/S";
    arbol actual=s->actual;

    if (parametro == NULL || strcasecmp(parametro, "/S") != 0){ //
        if ((s->actual->ph!=NULL) && (s->actual->listaArchivos==NULL) ){ //Si solo tiene directorios
            impRuta(s); //Imprime la ruta
            s->actual=actual;
            printf ("\n");

            ordenarDirs(s);
            printf ("\n");

            s->actual=actual;
        }else{
            if ((s->actual->listaArchivos!=NULL) && (s->actual->ph==NULL)){//Si solo tiene archivos
                impRuta(s); //Imprime la ruta
                s->actual=actual;
                printf ("\n");

                ordenarArchs(s);
                s->actual=actual;

            }else{
                if ((s->actual->ph!=NULL) && (s->actual->listaArchivos!=NULL)){ //Si tiene archs y dirs
                    impRuta(s); //Imprime la ruta
                    printf ("\n");
                    s->actual=actual;
                    ordenarArchs(s);
                    //s->actual=actual;
                    ordenarDirs(s);
                    s->actual=actual;
                    //s->actual=actual;
                }else{
                    impRuta(s); //Imprime la ruta
                    s->actual=actual;
                    printf ("\n");
                    printf ("No hay archivos, ni directorios.\n");
                }
            }
        }
    }else{ //Cuando parametro == /S
        impRuta(s);
        if (s->actual->listaArchivos!=NULL){ //Si tiene archivos
            impRuta(s);
            printf ("/%s\n",s->actual->listaArchivos->nombreArch);
        }
        if (s->actual->ph!=NULL){//Si tiene directorios
            //Idea: Hago una lista de directorios ordenados, la recorro y usando el nombre, busco el nodo con ese nombre
            //Y ahí si, me fijo si tiene archs, luego si tiene directorios, y hago lo mismo una y otra vez, de seguro
            //es mejor hacerlo recursivo, está bien hacer una lista de directorios en cada llamada recursiva CREO
            s->actual=s->actual->ph;
            impRuta(s);


        }

    }

    return OK;
}


TipoRet CREATEFILE (Sistema &s, Cadena nombreArchivo){
// Crea un nuevo archivo en el directorio actual.

    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    listaArc anterior;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){

        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            anterior = auxS;
            auxS= auxS->sig;//avanzo de arch
        }
    }

    if(encontroArch==false){//si no existe
        listaArc nuevoFile= new(lista_archivos); //creamos nuevo nodo

        strcpy(nuevoFile->nombreArch, nombreArchivo);

        if(s->actual->listaArchivos== NULL){//solo creamos un archivo
            s->actual->listaArchivos= nuevoFile;
            nuevoFile->sig=NULL;
            nuevoFile->ant=NULL;
            strcpy(nuevoFile->contenido, "");

        }
        else{//hay 1 o mas archivos
            anterior->sig= nuevoFile;//apunto como sig del ultimo archivo al "nuevoFile"
            nuevoFile->ant=anterior;
            strcpy(nuevoFile->contenido, "");
            nuevoFile->sig=NULL;
        }
        return OK;
    }
    else{
        printf("Ya existe el archivo parametro\n");
        return ERROR;
    }
}

TipoRet DELETE (Sistema &s, Cadena nombreArchivo){
// Elimina un archivo del directorio actual, siempre y cuando no sea de sólo lectura.
    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    listaArc anterior=NULL;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){

        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            anterior = auxS;
            auxS= auxS->sig;//avanzo de arch
        }
    }

    if(encontroArch==true){//si existe el archivo
        if(auxS->permisoW){//si es de LyE entonces lo borro

            if(auxS->sig==NULL && anterior==NULL){//si solo hay uno

                s->actual->listaArchivos = NULL;
            }
            else{
                if(auxS->sig!=NULL && anterior!=NULL ){//si estoy en el medio
                    anterior->sig = auxS->sig;
                    auxS->sig->ant=anterior;
                }
                else{
                    if(auxS->sig==NULL){//si elimino el ultimo
                        anterior->sig = NULL;
                    }
                    else{
                        if(anterior==NULL){//si elimino el primero
                            auxS->sig->ant= NULL;
                            s->actual->listaArchivos = auxS->sig;
                        }
                    }
                }
            }
            delete auxS;

            return OK;
        }
        else{
            printf("El archivo parametro es de tipo lectura\n");
            return ERROR;//porque es de permiso lectura
        }
    }
    else{
        printf("No se encontro el archivo parametro\n");
        return ERROR;//porque no encontro el archivo
    }
}

TipoRet ATTRIB (Sistema &s, Cadena nombreArchivo, Cadena parametro){
// Cambia los atributos de un archivo perteneciente al directorio actual.

    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){

        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            auxS= auxS->sig;//avanzo de arch
        }
    }

    if(encontroArch==true){

        if( strcmp ( parametro, "-W")== 0 ) {
            auxS->permisoW=false;
        }else{
            auxS->permisoW=true;
        }
        return OK;
    }
    else{
       // printf("No se encontro el archivo parametro\n");
        return ERROR;
    }

}

int longitudT(Cadena texto){
//funcion para contar la longitud de textos
//Pre: el texto no está vacío
    int longitud = 0;
    while (texto[longitud] != '\0') {
        longitud++;
    }
    return longitud;
}

TipoRet IC (Sistema &s, Cadena nombreArchivo, Cadena texto){
// Agrega un texto al comienzo del archivo NombreArchivo.
    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){
        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            auxS= auxS->sig;//avanzo de arch
        }
    }

    if(encontroArch==true){
        if(auxS->permisoW){//si es de LyE entonces escribo el contenido

            int longitud = longitudT(auxS->contenido);
            int longTxt= longitudT(texto);
            int espacioDisponible = TEXTO_MAX - longTxt-1;//espacio disponible para el contenido del archivo que ya estaba

            if(longTxt > TEXTO_MAX){
                longTxt= TEXTO_MAX-1;
            }
            // Verificar si hay espacio suficiente en el archivo para agregar el texto
            if ((longitud + longTxt) > TEXTO_MAX) {
                longitud = espacioDisponible;  // Reducimos la longitud del archivo original, truncando
                auxS->contenido[longitud] = '\0';//en la ultima posicion coloco el \0
            }
            // Desplaza el contenido del archivo hacia la derecha para hacer espacio para texto
            for (int i = longitud; i >= 0; --i) {
                auxS->contenido[i + longTxt] = auxS->contenido[i];
            }
            // Agrega el contenido de texto al comienzo del archivo
            for (int i = 0; i < longTxt; ++i) {
            auxS->contenido[i] = texto[i];
            }
            return OK;
        }
        else{
            printf("El archivo parametro es de tipo lectura\n");
            return ERROR;
        }
    }
    else{
        printf("No se encontro el archivo parametro\n");
        return ERROR;
    }
}

TipoRet IF (Sistema &s, Cadena nombreArchivo, Cadena texto){
// Agrega un texto al final del archivo NombreArchivo.
    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){
        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            auxS= auxS->sig;//avanzo de arch
        }
    }

    if(encontroArch==true){
        if(auxS->permisoW){//si es de LyE entonces escribo el contenido
            int longitud = longitudT(auxS->contenido);
            int longTxt= longitudT(texto);
            int espacioDisponible = TEXTO_MAX - longitud-1; //-1 por el \0

            if(longTxt >= TEXTO_MAX){
                longTxt= TEXTO_MAX-1;//trunco el texto ingresado, ya que supera el maximo permitido
            }
            // Si el contenido actual ya ha alcanzado TEXTO_MAX, sobrescribe los caracteres
            // en lugar de agregar al final del contenido.
            if (longitud >= TEXTO_MAX - 1) {
                longitud = TEXTO_MAX - 1 - longTxt;
            }
            if ((longitud + longTxt) > TEXTO_MAX-1) {//si la suma de ambos supera el texto maximo
                longTxt = espacioDisponible;  // Reducimos la longitud del texto ingresado, truncando
            }
            for(int i=0; i<longTxt; i++){
                auxS->contenido[longitud]=texto[i];
                longitud++;
            }
            auxS->contenido[longitud] = '\0';//en la ultima posicion coloco el \0

            return OK;
        }
        else{
            printf("El archivo parametro es de tipo lectura\n");
            return ERROR;
        }
    }
    else{
        printf("No se encontro el archivo parametro\n");
        return ERROR;
    }
}

TipoRet DC (Sistema &s, Cadena nombreArchivo, int k){
// Elimina a lo sumo los K primeros caracteres del archivo parámetro.
    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){

        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            auxS= auxS->sig;//avanzo de arch
        }
    }

    if(encontroArch==true){
        if(auxS->permisoW){//si es de LyE
            int longitud = strlen(auxS->contenido);//no cuenta el \0

            // Si K es mayor o igual a la longitud del string, establece el string como vacío
            if (k >= longitud) {
                auxS->contenido[0] = '\0';
            } else {
            // Mueve los caracteres (longitud - k) posiciones hacia adelante para eliminar los primeros K elementos
                for (int i = 0; i <= longitud - k; ++i) {
                    auxS->contenido[i] = auxS->contenido[i + k];
                }
            }
            return OK;
        }
        else{
            printf("El archivo parametro es de tipo lectura\n");
            return ERROR;
        }
    }
    else{
        printf("No se encontro el archivo parametro\n");
        return ERROR;
    }

}

TipoRet DF (Sistema &s, Cadena nombreArchivo, int k){
// Elimina los, a lo sumo K últimos caracteres del archivo parámetro.
    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){

        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            auxS= auxS->sig;//avanzo de arch
        }
    }

    if(encontroArch==true){
        if(auxS->permisoW){//si es de LyE
            int longitud = strlen(auxS->contenido);//no cuenta el \0

            if (k >= longitud) {
                auxS->contenido[0] = '\0';
            } else {

                auxS->contenido[longitud-k] = '\0';
            }
            return OK;
        }
        else{
            printf("El archivo parametro es de tipo lectura\n");
            return ERROR;
        }
    }
    else{
        printf("No se encontro el archivo parametro\n");
        return ERROR;
    }
}

TipoRet TYPE (Sistema &s, Cadena nombreArchivo){
// Imprime el contenido del archivo parámetro.
    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){

        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            auxS= auxS->sig;//avanzo de arch
        }
    }

    if(encontroArch==true){
        int longitud = longitudT(auxS->contenido);
        if(longitud==0){
            printf("El archivo parametro no posee contenido\n");
        }
        else{
            printf("%s\n",auxS->contenido);
        }
    }
    else{
        printf("No se encontro el archivo parametro\n");
        return ERROR;
    }
    return OK;
}

TipoRet SEARCH (Sistema &s, Cadena nombreArchivo, Cadena texto){
// Busca dentro del archivo la existencia del texto.
    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){
        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            auxS= auxS->sig;//avanzo de arch
        }
    }
    if(encontroArch==true){//encontro el archivo
        int longitud = strlen(auxS->contenido);
        int lonTxt = strlen(texto);

        if(longitud < lonTxt){
            printf("No se encontro el texto parametro\n");
            return ERROR;
        }
        else{
            for(int i=0; i<longitud; i++){

                if(auxS->contenido[i]== texto[0]){
                    int result = strncmp(auxS->contenido +i ,texto, lonTxt);
                    if(result==0){
                        return OK;
                    }
                }
            }
            printf("No se encontro el texto parametro\n");
            return ERROR;//no encontro el texto
        }
	}
	else{
        printf("No se encontro el archivo parametro\n");
        return ERROR;//no encontro el archivo
	}
}

TipoRet REPLACE (Sistema &s, Cadena nombreArchivo, Cadena texto1, Cadena texto2){
// Busca y reemplaza dentro del archivo la existencia del texto1 por el texto2.
//Pre: solo se reemplazara la primera coincidencia
    bool encontroArch = false;

    listaArc auxS =s->actual->listaArchivos;
    //recorro la lista general de archivos hasta el final o hasta encontrar archivo
    while(auxS !=NULL && encontroArch==false){
        int result = strcmp(auxS->nombreArch,nombreArchivo);
        //printf("entro en el while1\n");
        if(result==0){//son iguales
            encontroArch=true;//encontre el archivo
        }
        else{//son distintos
            auxS= auxS->sig;//avanzo de arch
        }
    }
    if(encontroArch==true){//encontro el archivo
        int longitud = strlen(auxS->contenido);//longitud del archivo encontrado
        int lonTxt1 = strlen(texto1);//longitud del texto1
        int lonTxt2=strlen(texto2);//longitud del texto2

        if(lonTxt1 > longitud){
            printf("No se encontro el texto parametro\n");
            return ERROR;
        }
        else{
            int i=0;
            bool encontroTxt=false;
            while(i<longitud && encontroTxt==false){
                if(auxS->contenido[i]== texto1[0]){
                    int result = strncmp(auxS->contenido +i ,texto1, lonTxt1);
                    if(result==0){//encontro el texto1
                        encontroTxt=true;
                    }
                }
                if(encontroTxt==false){
                    i++;
                }
            }
            if(encontroTxt==true){//aca hacer el intercambio
                if((longitud-lonTxt1)+ lonTxt2 > TEXTO_MAX){//Retorna error si llego a hacer el intercambio y supera la longitud permitida
                    printf("No se puede reemplazar\n");
                    return ERROR;
                }
                else{
                    //si tienen la misma longitud simplemente se reemplazan
                    if(lonTxt1==lonTxt2){
                        for(int o=0; o<lonTxt2; o++){
                            auxS->contenido[i]= texto2[o];
                            i++;
                        }
                    }
                    else{
                        if(lonTxt2<lonTxt1){//si es menor hay que /home/tecnoinf/Descargas/Primera_entrega_update.zipr el contenido hacia la izquierda
                            // Reemplazar el texto1 con el texto2
                            for (int j = 0; j < lonTxt2; j++) {
                                auxS->contenido[i + j] = texto2[j];
                            }

                            // Mover el contenido restante hacia la izquierda
                            while (i < longitud) {
                                auxS->contenido[i + lonTxt2] = auxS->contenido[i + lonTxt1];
                                i++;
                            }

                            // Asegurar que el nuevo contenido termine con '\0'
                            auxS->contenido[i + lonTxt2] = '\0';
                        }

                        else{//si es mayor, el texto despues de esa palaba se mueve hacia la derecha
                            if(lonTxt2>lonTxt1){

                                // Hacer espacio para el nuevo texto moviendo el contenido hacia la derecha
                                for (int j = longitud; j >= i + lonTxt1; j--) {
                                    auxS->contenido[j + lonTxt2 - lonTxt1] = auxS->contenido[j];
                                }
                                // Insertar el nuevo texto en la posición correcta
                                for (int j = 0; j < lonTxt2; j++) {
                                    auxS->contenido[i + j] = texto2[j];
                                }
                            }
                        }
                    }
                    return OK;
                }
            }
            else{
                printf("No se encontro el texto parametro\n");
                return ERROR;//no encontro el texto
            }
        }
	}
	else{
        printf("No se encontro el archivo parametro\n");
        return ERROR;//no encontro el archivo
	}
}
