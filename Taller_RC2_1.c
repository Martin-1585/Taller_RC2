#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "funciones.h"

int main(){
    int option;
    caratula("Simulador de Prediccion de Contaminacion"); //Presentación del simulador
    do{
        menu();
        option = ingreo_numeroentero("Seleccione una zona de la ciudad", "Opcion no valida, ingrese nuevamente", "Supero el limite, ingrese nuevamente", 1, 6); //Llamado de funcion con sus respectivas validaciones
        switch (option){
        case 1:
            acciones_zonas("Guamani.txt", "Guamani"); //Llamado a la funcion principal del taller, esta función contiene el resto de acciones requeridas en la consigna
            break;
        case 2:
            acciones_zonas("Cotocollao.txt", "Cotocollao"); //Llamado a la funcion principal del taller, esta función contiene el resto de acciones requeridas en la consigna
            break;
        case 3:
            acciones_zonas("Centro.txt", "Centro Historico"); //Llamado a la funcion principal del taller, esta función contiene el resto de acciones requeridas en la consigna
            break;
        case 4:
            acciones_zonas("LosChillos.txt", "Valle de los Chillos"); //Llamado a la funcion principal del taller, esta función contiene el resto de acciones requeridas en la consigna
            break;
        case 5:
            acciones_zonas("Tumbaco.txt", "Tumbaco"); //Llamado a la funcion principal del taller, esta función contiene el resto de acciones requeridas en la consigna
            break;
        default:
            break;
        }
    } while (option != 6);
    printf("\x1B[33mSALIENDO DEL PROGRAMA...\x1B[0m");
    return 0;
}
