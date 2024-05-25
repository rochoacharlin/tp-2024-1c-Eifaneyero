#include "lectura.h"

t_instruccion_cadena *leer_instruccion(int desplazamiento)
{
    FILE *file = fopen("instrucciones_prueba.txt", "r");
    if (file == NULL)
    {
        log_info(logger_propio, "Error al abrir el archivo de instrucciones");
        return NULL;
    }

    t_instruccion_cadena *instruccion = crear_instruccion();
    char *linea = NULL;
    size_t tamanio = 0;
    ssize_t valor_leido;

    int linea_actual = 0;
    while ((valor_leido = getline(&linea, &tamanio, file)) != -1)
    {
        if (linea_actual == desplazamiento)
        {
            // Eliminar el salto de línea al final de la línea si existe
            if (linea[valor_leido - 1] == '\n')
            {
                linea[valor_leido - 1] = '\0';
            }
            instruccion->instruccion = strdup(linea);
            break;
        }
        linea_actual++;
    }

    free(linea);
    fclose(file);

    if (linea_actual != desplazamiento)
    {
        destruir_instruccion(instruccion);
        return NULL; // Desplazamiento fuera de los límites del archivo
    }

    return instruccion;
}