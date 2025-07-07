#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
//Limite de los dias a analizar y nombre de zona
#define MAX_DAYS 30
#define MAX_NAME 30
//Limites de contaminantes 
#define MAX_PM25 25
#define MAX_PM10 50
#define MAX_O3 100
#define MAX_NO2 200
#define MAX_SO2 20
#define MAX_CO 10
//Vaariables de humedad, temperatura y velocidad del viento
#define MAX_HUMIDITY 55
#define MAX_TEMPERATURA 15
#define MAX_VELVIENTO 6

typedef struct {
    char date[11]; //Formato: dia/mes/año
    float pm25, pm10, o3, no2, so2, co; //Contaminantes (Particulas 2.5, Particulas 10, ozono, dióxido de nitrógeno, monóxido de carbono)
} data;

typedef struct {
    char zone_name[MAX_NAME];
    data daily_data[MAX_DAYS]; //Datos diarios de contaminación
    int num_data;
} zone_data;

//Funciones de lectura
void caratula(char *message);
void menu();
int ingreo_numeroentero(char *message, char *validation, char *limit, int min, int max);
void menu_opciones();
//Funciones de analisis de datos, informacion de los .txt y fechas
void leer_datos_csv(const char *filename, zone_data *zone, int *num_data);
int validar_fecha(const char *date_enter);
time_t convertir_fecha_a_time(const char *date);
time_t obtener_fechasiguiente(const char *dater_enter);
//Funciones del taller
void acciones_zonas(const char *archive_csv, const char *zone_name);
void monitoreo_contaminacion_zonas(zone_data *zone);
void prediccion_nivelesfuturo(zone_data *zone);
void alertas_preventivas(zone_data *zone);
void calcularPromedio(zone_data *zone);
void generar_recomendaciones(zone_data *zone);
void exportar_datos(zone_data *zone);