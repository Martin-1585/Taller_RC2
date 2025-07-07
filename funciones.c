#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "funciones.h"

void caratula(char *message){
    printf("\t\x1B[34m**************************\x1B[0m\n");
    printf("\t\x1B[35m%s\x1B[0m\n", message);
    printf("\t\x1B[34m**************************\x1B[0m\n");
}

int ingreo_numeroentero(char *message, char *validation, char *limit, int min, int max){
    int valido, value;
    do{
        printf("%s (%d - %d): ",message, min, max); //Paso de parámetros para desplegar mensajes de bienvenida 
        valido = scanf("%d", &value);
        if (valido != 1 || value < min){
            printf("\x1B[31m%s\x1B[0m\n", validation); //Mensaje de validación si el usuario ingresa un dato que no es un entero o es menor al limite impuesto
            while(getchar() != '\n');
        }else if (value > max){
            printf("\x1B[33m%s\x1B[0m\n", limit); //Mensaje de validación si el usuario ingresa un entero que sea mayor al limite impuesto
            while(getchar() != '\n');
        }else{}
    } while (valido != 1 || value < min || value > max);
    return value;
}

void menu(){
    printf("\tCalidad de Aire en zonas de Quito\n");
    printf("--------------------------------------------\n");
    printf("|1. Guamani                                |\n");
    printf("|2. Cotocollao                             |\n");
    printf("|3. Centro Historico                       |\n");
    printf("|4. Valle de los Chillos                   |\n");
    printf("|5. Tumbaco                                |\n");
    printf("|6. SALIR                                  |\n");
    printf("--------------------------------------------\n");
}

void menu_opciones() {
    printf("\tAnalisis de informacion\n");
    printf("--------------------------------------------\n");
    printf("|1. Monitoreo de contaminacion actual      |\n");
    printf("|2. Prediccion de niveles futuros          |\n");
    printf("|3. Alertas preventivas                    |\n");
    printf("|4. Calculo de promedios historicos        |\n");
    printf("|5. Generacion de recomendaciones          |\n");
    printf("|6. Exportacion de datos                   |\n");
    printf("|7. REGRESAR AL MENU PRINCIPAL             |\n");
    printf("--------------------------------------------\n");
}

time_t convertir_fecha_a_time(const char *date) {
    struct tm tm_fecha = {0}; // Declaracion de estructura tm inicilizada en 0
    int dia, mes, anio;

    if (sscanf(date, "%d/%d/%d", &dia, &mes, &anio) != 3) {
        return -1; // Devuelve un valor de -1 si el formato de la fecha es incorrecto
    }

    tm_fecha.tm_mday = dia; //Transformación de carácter a un día válido mediante el acceso a a estructra y posterior asignacioón
    tm_fecha.tm_mon = mes - 1; //Transformación de carácter a una mes válido, los meses en C comienzan desde 0 hasta 11
    tm_fecha.tm_year = anio - 1900; //Transformación de carácter a un año válido, los años en C comienzan desde 1900 
    tm_fecha.tm_isdst = -1; //Análisis si el año bisiesto o no, la instrucción -1 da la instrucción al sistema de analizarlo automáticamente      
    
    return mktime(&tm_fecha); //Se regresa el valor de la fecha en un formato válido para el análisis
}

int validar_fecha(const char *date_enter){
    struct tm tm_user = {0};
    int day, month, year;
    if(sscanf(date_enter, "%d/%d/%d", &day, &month, &year) != 3){ //Lectura del formate de la fecha, si la fecha no posee el formato solicitado devolverá una valor de 0 y un posterios mensaje de validación
        return 0;
    }
    tm_user.tm_mday = day; //Transformación de carácter a un día válido mediante el acceso a a estructra y posterior asignacioón
    tm_user.tm_mon = month - 1; //Transformación de carácter a una mes válido, los meses en C comienzan desde 0 hasta 11
    tm_user.tm_year = year - 1900; //Transformación de carácter a un año válido, los años en C comienzan desde 1900
    tm_user.tm_isdst = -1; //Análisis si el año bisiesto o no, la instrucción -1 da la instrucción al sistema de analizarlo automáticamente
    time_t time_valid = mktime(&tm_user); //Se convierte la fecha ingresada por el usuario a un formato de tiempo válido para su posterior análisis

    if (time_valid == -1 || tm_user.tm_mday != day || tm_user.tm_mon != month - 1){
        return 0; //Si el formato del dia o mes son incorecctos se devuelve un valor de 0 y un mensaje de validación
    }
    time_t now;
    time(&now); //Se utiliza el tiempo registrado en el dispositvo para generar una predicción más precisa
    struct tm *tm_now = localtime(&now); // Se obtiene la hora actual del sistema
    tm_now -> tm_hour = 0;
    tm_now -> tm_min = 0;
    tm_now -> tm_sec = 1;
    time_t midnight = mktime(tm_now); //Se comsidera una fecha nueva cada vez que sea medianoche

    if(time_valid < midnight){
        return -1; // Fecha pasada no permitida
    }
    time_t future = midnight + (365 * 24 * 3600); // Un año en segundos
    if (time_valid > future) {
        return -2; // Fecha muy adelantada no permitida
    }
    return 1;
}

time_t obtener_fechasiguiente(const char *dater_enter){
    time_t future_date = convertir_fecha_a_time(dater_enter); //Conveierte la fecha ingresada por el uusuario en la fecha posterior a la ingresada
    return future_date + 24 * 3600; //El valor de 24 * 3600 es el número de segundos que hay en un día, por lo que se obtiene la fecha del día siguiente
}

void leer_datos_csv(const char *filename, zone_data *zone, int *num_data){
    FILE *file;
    file = fopen(filename, "r"); //Se abre el archivo .txt en modo de lectura
    if (file == NULL) {
        printf("\x1B[31mError al abrir el archivo %s\x1B[0m\n", filename); //Si el archivo esta vacío o ocurre alguna falla al abrirlo se despliega un mensaje de atencion 
        return;
    }
    char line[256];
    fgets(line, sizeof(line), file); // Leer la cabecera
    
    zone->num_data = 0; //Se declara que la cantidad de infromación sobre la zona es 0
    while(fgets(line, sizeof(line), file) && zone->num_data < MAX_DAYS) { //Minentras se lea una línea del archivo y la cantidad de datos sea menor a 30 se repite este proceso
        data *current_d = &zone->daily_data[zone->num_data];
        if (sscanf(line, "%10[^,],%f,%f,%f,%f,%f,%f", current_d->date, &current_d->pm25, &current_d->pm10, &current_d->o3, &current_d->no2, &current_d->so2, &current_d->co) == 7){
            zone->num_data++; //Si el archivo se lee correctamente se incrementa la cantidad de datos de la zona
        }
    }
    fclose(file); //Finalmente se debe cerrar el archivo para evitar la pérdida de información o errores en el programa
}

void acciones_zonas(const char *archive_csv, const char *zone_name){
    zone_data zone;
    int num_data = 0, option;
    strcpy(zone.zone_name, zone_name); //Se copia el nombre de la zona a la estrucutra y permite que se puedea utilizar en múltiples funciones
    leer_datos_csv(archive_csv, &zone, &num_data); //Se llama a la función de leer_datos_csv para que el resto de funciones trabajen con 30 datos
    do{
        printf("\t\x1B[33m%s\x1B[0m\n", zone_name);
        menu_opciones();
        option = ingreo_numeroentero("Seleccione una accion", "Opción no válida, ingrese nuevamente", "Superó el límite, ingrese nuevamente", 1, 7);
        switch (option){
        case 1:
            monitoreo_contaminacion_zonas(&zone); //Llamado a la función de monitoreo, este despliega la información de los 30 dias y determina si los agentes conteminantes esta en un rango aceptable o no
            break;
        case 2:
            prediccion_nivelesfuturo(&zone); //Llamado a la función de predicción, esta permite ingresar una fecha y predecir los niveles de contaminación 
            break;
        case 3:
            alertas_preventivas(&zone); //Llamado a la función de alertas preventivas, esta despliega un mensaje de alerta si los niveles de contaminación son altos
            break;
        case 4:
            calcularPromedio(&zone); //Llamado a la función de calcular promedio, esta calcula el promedio de los 30 días de los agentes contaminantes y determina si estos están en un rango aceptable o no
            break;
        case 5:
            generar_recomendaciones(&zone); //Llamado a la función de generar recomendaciones, esta despliega mensajes de recomendación si los niveles de contaminación son altos
            break;
        case 6:
            exportar_datos(&zone); //Llamado a la función de exportar datos, esta permite exportar los datos de los 30 días a un archivo .txt
            break;
        default:
            break;
        }
    } while (option != 7);
    printf("\x1B[33mSALIENDO DE LAS ACCIONES DE %s...\x1B[0m\n", zone_name);
    printf("--------------------------------------\n");
    
}

void monitoreo_contaminacion_zonas(zone_data *zone){
    printf("\t\t\x1B[33mMonitoreo de Contaminacion de los ultimos %d dias en %s\x1B[0m\n", zone->num_data, zone->zone_name);
    printf("|Fecha\t\t|PM2.5\t\t|PM10\t\t|O3\t\t|NO2\t\t|SO2\t\t|CO\t |\n");
    printf("----------------------------------------------------------------------------------------------------------\n");
    if (zone->num_data == 0) {
        printf("\x1B[31mNo se encontraron datos para mostrar.\x1B[0m\n");
        return;
    }
    for (int i = 0; i < zone->num_data; i++){
        const data *info = &zone->daily_data[i];
        const char *state_pm25;
        if (info->pm25 < MAX_PM25) { //Comparación de los niveles de PM25 con el límite impuesto por la OMS, si es bajo de 25 se considera un aire de calidad
            state_pm25 = "\x1B[32mBajo\x1B[0m"; 
        } else if (info->pm25 < MAX_PM10) { //Comparación de los niveles de PM25 con el límite impuesto por la OMS, si es mayor a 25 pero menor a 50 se considera un aire con precaución
            state_pm25 = "\x1B[33mMedio\x1B[0m"; 
        } else { //Comparación de los niveles de PM25 con el límite impuesto por la OMS, si es mayor a 50 se considera un aire de mala calidad
            state_pm25 = "\x1B[31mAlto\x1B[0m"; 
        }
        const char *state_pm10;
        if (info->pm10 < MAX_PM10) { //Comparación de los niveles de PM10 con el límite impuesto por la OMS, si es bajo de 50 se considera un aire de calidad
            state_pm10 = "\x1B[32mBajo\x1B[0m"; 
        } else if (info->pm10 < MAX_O3) { //Comparación de los niveles de PM10 con el límite impuesto por la OMS, si es mayor de 50 pero menor a 100 se considera un aire de media calidad
            state_pm10 = "\x1B[33mMedio\x1B[0m"; 
        } else { //Comparación de los niveles de PM10 con el límite impuesto por la OMS, si es mayor a 100 se considera un aire de mala calidad
            state_pm10 = "\x1B[31mAlto\x1B[0m"; 
        }
        const char *state_o3;
        if (info->o3 < MAX_O3) { //Comparación de los niveles de O3 con el límite impuesto por la OMS, si es bajo de 100 se considera un aire de calidad
            state_o3 = "\x1B[32mBajo\x1B[0m"; 
        } else if (info->o3 < MAX_NO2) { //Comparación de los niveles de O3 con el límite impuesto por la OMS, si es mayor a 100 pero menor a 200 se considera un aire con precaución
            state_o3 = "\x1B[33mMedio\x1B[0m"; 
        } else { //Comparación de los niveles de O3 con el límite impuesto por la OMS, si es mayor a 200 se considera un aire de mala calidad
            state_o3 = "\x1B[31mAlto\x1B[0m"; 
        }
        const char *state_no2;
        if (info->no2 < MAX_NO2) { //Comparación de los niveles de NO2 con el límite impuesto por la OMS, si es bajo de 200 se considera un aire de calidad
            state_no2 = "\x1B[32mBajo\x1B[0m"; 
        } else if (info->no2 < MAX_SO2) { //Comparación de los niveles de NO2 con el límite impuesto por la OMS, si es mayor a 200 pero menor a 300 se considera un aire con precaución
            state_no2 = "\x1B[33mMedio\x1B[0m"; 
        } else { //Comparación de los niveles de NO2 con el límite impuesto por la OMS, si es mayor a 300
            state_no2 = "\x1B[31mAlto\x1B[0m"; 
        }
        const char *state_so2;
        if (info->so2 < MAX_SO2) { //Comparación de los niveles de SO2 con los límites impuestos por la OMS, si esta por debajo de 20 se considera un aire de calidad
            state_so2 = "\x1B[32mBajo\x1B[0m"; 
        } else if (info->so2 < MAX_CO) { //Comparación de los niveles de SO2 con los límites impuestos de la OMS, si esta por encima encima de 20 se considera un aire de media calidad
            state_so2 = "\x1B[33mMedio\x1B[0m";
        } else { //Comparación de los niveles de SO2 con los límites impuestos de la OMS, si esta por encima de 25 se considera un aire de mala calidad 
            state_so2 = "\x1B[31mAlto\x1B[0m"; 
        }
        const char *state_co;
        if(info->co < MAX_CO) {
            state_co = "\x1B[32mBajo\x1B[0m"; 
        } else if (info->co < MAX_PM25) {
            state_co = "\x1B[33mMedio\x1B[0m"; 
        } else {
            state_co = "\x1B[31mAlto\x1B[0m"; 
        }
        printf("|%s\t|%.2f\t\t|%.2f\t\t|%.2f\t\t|%.2f\t\t|%.2f\t\t|%.2f\t |\n", zone->daily_data[i].date, zone->daily_data[i].pm25, zone->daily_data[i].pm10, zone->daily_data[i].o3, zone->daily_data[i].no2, zone->daily_data[i].so2, zone->daily_data[i].co);
        printf("|\t\t|PM2.5: %s\t|PM10: %s\t|O3: %s\t|NO2: %s\t|SO2: %s\t|CO: %s|\n", state_pm25, state_pm10, state_o3, state_no2, state_so2, state_co);
    }
    printf("----------------------------------------------------------------------------------------------------------\n");
}

void prediccion_nivelesfuturo(zone_data *zone) {
    char date_enter[11];
    int result_valid;
    float sum_pm25 = 0, sum_pm10 = 0, sum_03 = 0, sum_no2 = 0, sum_so2 = 0, sum_co = 0;
    float prom_pm25, prom_pm10, prom_03, prom_no2, prom_so2, prom_co;
    float predic_pm25, predic_pm10, predic_o3, predic_no2, predic_so2, predic_co;
    int days_counted = 0;
    
    printf("\x1b[33m\n\tPrediccion de niveles de contaminacion en 24 hora para %s\x1B[0m\n", zone->zone_name);
    do{
        printf("Ingrese la fecha a predecir (formato dia/mes/anio): ");
        scanf("%10s", date_enter);
        while(getchar() != '\n'); 
        result_valid = validar_fecha(date_enter);
        if (result_valid == 0){
            printf("\x1B[31mFecha invalida, debe ser en formato (dia/mes/anio) o revise el dia y mes\x1B[0m\n");
        } else if (result_valid == -1){
            printf("\x1B[33mFecha invalida, no se puede ingresar una fecha pasada\x1B[0m\n");
        } else if (result_valid == -2) {
            printf("\x1B[33mFecha invalida, no se puede ingresar una fecha tan distante\x1B[0m\n");
        } else {
            break;
        }
    } while (result_valid != 1);

    time_t prediction = obtener_fechasiguiente(date_enter);
    time_t beginning = prediction - (30 * 24 *3600);
    
    for (int i = 0; i < zone->num_data; i++){
        time_t info_date = convertir_fecha_a_time(zone->daily_data[i].date);
        if (info_date >= beginning && info_date < prediction) {
            sum_pm25 += zone->daily_data[i].pm25;
            sum_pm10 += zone->daily_data[i].pm10;
            sum_03 += zone->daily_data[i].o3;
            sum_no2 += zone->daily_data[i].no2;
            sum_so2 += zone->daily_data[i].so2;
            sum_co += zone->daily_data[i].co;
            days_counted++;
        }
    }
    
    if(days_counted == 0){
        printf("\x1B[33mNo se posee la cantidad de datos requeridos para realizar la prediccion\x1B[0m\n");
        return;
    }

    prom_pm25 = sum_pm25 / days_counted;
    prom_pm10 = sum_pm10 / days_counted;
    prom_03 = sum_03 / days_counted;
    prom_no2 = sum_no2 / days_counted;
    prom_so2 = sum_so2 / days_counted;
    prom_co = sum_co / days_counted;

    predic_pm25 = prom_pm25 * (1 + (MAX_HUMIDITY/ 100.0 - MAX_VELVIENTO / 100.0));
    predic_pm10 = prom_pm10 * (1 + (MAX_HUMIDITY / 120.0 - MAX_VELVIENTO / 90.0));
    predic_o3 = prom_03 * (1 + (MAX_TEMPERATURA / 120.0 - MAX_VELVIENTO / 120.0));
    predic_no2 = prom_no2 * (1 + (MAX_HUMIDITY/110.0));
    predic_so2 = prom_so2 * (1 +(MAX_HUMIDITY / 130.0 - MAX_VELVIENTO / 80.0));
    predic_co = prom_co * (1 + (MAX_HUMIDITY/150.0));

    printf("\n\x1B[36mPredicción de niveles de contaminación para %s el %s:\x1B[0m\n", zone->zone_name, date_enter);
    printf("------------------------------------------------------------------\n");
    printf("|Contaminante\t| Nivel Actual\t\t|Prediccion en 24 horas\t|\n");
    printf("------------------------------------------------------------------\n");
    printf("|PM 25\t\t| %.2f ug/m^3\t\t| %.2f ug/m^3\t\t|\n", prom_pm25, predic_pm25);
    printf("|PM 10\t\t| %.2f ug/m^3\t\t| %.2f ug/m^3\t\t|\n", prom_pm10, predic_pm10);
    printf("|O3\t\t| %.2f ug/m^3\t\t| %.2f ug/m^3\t\t|\n", prom_03, predic_o3);
    printf("|NO2\t\t| %.2f ug/m^3\t\t| %.2f ug/m^3\t\t|\n", prom_no2, predic_no2);
    printf("|SO2\t\t| %.2f ug/m^3\t\t| %.2f ug/m^3\t\t|\n", prom_so2, predic_so2);
    printf("|CO\t\t| %.2f mg/m^3\t\t| %.2f mg/m^3\t\t|\n", prom_co, predic_co);
    printf("------------------------------------------------------------------\n");
}

void alertas_preventivas(zone_data *zone) {
    printf("\n\x1B[31m--- ADVERTENCIAS PREVENTIVAS PARA %s ---\x1B[0m\n", zone->zone_name);
    int found_alert = 0;
    for (int i = 0; i < zone->num_data; i++) {
        data *d = &zone->daily_data[i];
        int alerta = 0;
 
        if (d->pm25 > 25 || d->pm10 > 50 || d->o3 > 100 || d->no2 > 200 || d->so2 > 20 || d->co > 10) {
            printf(" \x1B[33mFecha: %s\x1B[0m\n", d->date);
            if (d->pm25 > 25) { 
                printf("  PM2.5: %.2f ug/m^3 (Limite: 25)\n", d->pm25); 
                alerta = 1; 
            }
            if (d->pm10 > 50) { 
                printf("  PM10: %.2f ug/m^3 (Limite: 50)\n", d->pm10); 
                alerta = 1; 
            }
            if (d->o3 > 100)  { 
                printf("  O3: %.2f ug/m^3 (Limite: 100)\n", d->o3); 
                alerta = 1; 
            }
            if (d->no2 > 200) { 
                printf("  NO2: %.2f ug/m^3 (Limite: 200)\n", d->no2); 
                alerta = 1; 
            }
            if (d->so2 > 20)  { 
                printf("  SO2: %.2f ug/m^3 (Limite: 20)\n", d->so2); 
                alerta = 1; 
            }
            if (d->co > 10)   { 
                printf("  CO: %.2f mg/m^3 (Limite: 10)\n", d->co); 
                alerta = 1; 
            }
            if (alerta) {
                printf(" Recomendacion: Evite actividades al aire libre. Use mascarilla si es necesario.\n\n");
                found_alert = 1;
            }
        }
    }
    if (!found_alert) {
        printf("\x1B[32mNo se detectaron niveles de contaminacion peligrosos en los últimos %d días.\x1B[0m\n", zone->num_data);
    }
}

void impresionEstado(float estado, int limite){
    if (estado > limite)
        printf("\n\x1B[31mEstado: Alto\x1B[0m\n");
     else 
        printf("\n\x1B[33mEstado: Aceptable\x1B[0m\n");
}

void calcularPromedio(zone_data *zone){
    float suma_pm25 = 0, suma_pm10 = 0, suma_o3 = 0, suma_no2 = 0, suma_so2 = 0, suma_co = 0;
    float prom_suma_pm25 = 0, prom_suma_pm10 = 0, prom_suma_o3 = 0, prom_suma_no2 = 0, prom_suma_so2 = 0, prom_suma_co = 0;
    for(int i = 0; i < zone->num_data;i++)
    {
        suma_pm25 += zone->daily_data[i].pm25;
        suma_pm10 += zone->daily_data[i].pm10;
        suma_o3 += zone->daily_data[i].o3;
        suma_no2 += zone->daily_data[i].no2;
        suma_so2 += zone->daily_data[i].so2;
        suma_co += zone->daily_data[i].co;
    }

    prom_suma_pm25 = suma_pm25 / zone->num_data;
    prom_suma_pm10 = suma_pm10 / zone->num_data;
    prom_suma_o3 = suma_o3 / zone->num_data;
    prom_suma_no2 = suma_no2 / zone->num_data;
    prom_suma_so2 = suma_so2 / zone->num_data;
    prom_suma_co = suma_co / zone->num_data;

    printf("\n\x1B[36m--- PROMEDIOS HISTORICOS EN %s ---\x1B[0m\n", zone->zone_name);
    printf("Periodo de dias\n");
    printf("=============================================\n");
    printf("Contaminante\t\tPromedio\t\tLimite OMS\n");
    printf("------------------------------------------------------------\n");

    printf("PM25\t\t\t%0.2f\t\t\t25", prom_suma_pm25);
    impresionEstado(prom_suma_pm25, 25);
    printf("PM10\t\t\t%0.2f\t\t\t50", prom_suma_pm10);
    impresionEstado(prom_suma_pm10, 50);
    printf("O3\t\t\t%0.2f\t\t\t100", prom_suma_o3);
    impresionEstado(prom_suma_o3, 100);
    printf("No2\t\t\t%0.2f\t\t\t200", prom_suma_no2);
    impresionEstado(prom_suma_no2, 200);
    printf("SO2\t\t\t%0.2f\t\t\t20", prom_suma_so2);
    impresionEstado(prom_suma_so2, 20);
    printf("Co\t\t\t%0.2f\t\t\t10", prom_suma_co);
    impresionEstado(prom_suma_co, 10);
    printf("------------------------------------------------------------\n");
}

void generar_recomendaciones(zone_data *zone) {
    printf("\n\x1B[36m==============================\n");
    printf("  Generacion de recomendaciones\n");
    printf("==============================\x1B[0m\n");
    printf("Zona: %s\n", zone->zone_name);
    printf("==============================\n");

    for (int i = 0; i < zone->num_data; i++) {
        data d = zone->daily_data[i];
        int score = 0;
        printf("\n\x1B[34mFecha: %s\x1B[0m\n", d.date);
        printf("------------------------------\n");
        if (d.pm25 > MAX_PM25) {
            printf("  * Alta concentracion de PM2.5. Se recomienda usar mascarilla.\n");
            score += 2;
        }
        if (d.pm10 > MAX_PM10) {
            printf("  * PM10 elevado. Evitar actividades al aire libre.\n");
            score += 2;
        }
        if (d.o3 > MAX_O3) {
            printf("  * Ozono alto. Use mascarilla para evitar enfermedades respiratorias.\n");
            score += 1;
        }
        if (d.no2 > MAX_NO2) {
            printf("  * NO2 elevado. Riesgo respiratorio.\n");
            score += 1;
        }
        if (d.so2 > MAX_SO2) {
            printf("  * SO2 peligroso. Permanecer en lugares internos.\n");
            score += 2;
        }
        if (d.co > MAX_CO) {
            printf("  * CO alto. Mucha combustion cercana.\n");
            score += 2;
        }

        if (score >= 6) {
            printf("\x1B[41m>> ALERTA CRITICA: Calidad del aire muy peligrosa.\x1B[0m\n");
            printf("  - Suspender todas las actividades al aire libre.\n");
            printf("  - Cierre temporal de industrias contaminantes.\n");
            printf("  - Implementar restricciones vehiculares estrictas.\n");
            printf("  - Promover confinamiento voluntario en zonas urbanas.\n");
        } else if (score >= 3) {
            printf("\x1B[43m>> ALERTA ALTA: Riesgo para la salud.\x1B[0m\n");
            printf("  - Reducir tráfico vehicular (pico y placa ampliado).\n");
            printf("  - Limitar actividades al aire libre (escuelas y parques).\n");
            printf("  - Incentivar el teletrabajo y el transporte público.\n");
        } else if (score > 0) {
            printf("\x1B[103m>> ALERTA MODERADA: Precaucion necesaria.\x1B[0m\n");
            printf("  - Usar mascarilla si va a estar al aire libre mucho tiempo.\n");
            printf("  - Preferir espacios cerrados ventilados.\n");
        } else {
            printf("  * Calidad de aire aceptable. Disfrute al aire libre con precaución.\n");
        }

        if (strstr(d.date, "31/12") || strstr(d.date, "01/01")) {
            printf("  * Fin de año: Evite la quema de monigotes. Genera mucho material particulado.\n");
        }
        if (strstr(d.date, "12/02") || strstr(d.date, "13/02")) { 
            printf("  * Carnaval: Evite el uso de espumas en espacios públicos. Use transporte público para movilizarse.\n");
        }
        if (strstr(d.date, "28/03") || strstr(d.date, "29/03")) { 
            printf("  * Semana Santa: Cuidado con el aumento de residuos en procesiones. Respete el entorno natural.\n");
        }
        if (strstr(d.date, "01/05")) {
            printf("  * Día del Trabajo: Menor tráfico. Buena oportunidad para actividades al aire libre si el aire es limpio.\n");
        }
        if (strstr(d.date, "06/12")) {
            printf("  * Fiestas de Quito: Evite congestión y uso de vehiculos privados. Use el trolebús o la Ecovía.\n");
        }
        printf("  * Quito tiene alta radiación UV. Use protector solar.\n");
        printf("  * No queme basura ni hojas secas. Contribuye a mantener limpio el aire.\n");
        printf("  * Recicle siempre que sea posible. Reduzca el uso de papel y plásticos.\n");
        printf("------------------------------\n");
    }

    printf("==============================\n");
    printf("\x1B[32mRecomendaciones generadas exitosamente.\x1B[0m\n");
}

void exportar_datos(zone_data *zone) {
    char filename_export[50];
    sprintf(filename_export, "reporte_%s", zone->zone_name);
 
    FILE *file = fopen(filename_export, "w");
    if (file == NULL) {
        printf("\x1B[31mError: No se pudo crear el archivo de exportacion\x1B[0m\n");
        return;
    }
 
    fprintf(file, "=============================================\n");
    fprintf(file, "REPORTE DE CALIDAD DEL AIRE - ZONA: %s\n", zone->zone_name);
    fprintf(file, "Total de dias registrados: %d\n", zone->num_data);
    fprintf(file, "=============================================\n\n");
 
    fprintf(file, "%-10s %-8s %-8s %-8s %-8s %-8s %-8s\n",
            "Fecha", "PM2.5", "PM10", "O3", "NO2", "SO2", "CO");
    fprintf(file, "%-10s %-8s %-8s %-8s %-8s %-8s %-8s\n",
            "==========", "========", "========", "========", "========", "========", "========");
 
    for (int i = 0; i < zone->num_data; i++) {
        data d = zone->daily_data[i];
        fprintf(file, "%-10s %-8.2f %-8.2f %-8.2f %-8.2f %-8.2f %-8.2f\n",
                d.date, d.pm25, d.pm10, d.o3, d.no2, d.so2, d.co);
    }
    fprintf(file, "=================================================================\n");
    fclose(file);
    printf("\x1B[32mReporte generado exitosamente: '%s'\x1B[0m\n", filename_export);
}