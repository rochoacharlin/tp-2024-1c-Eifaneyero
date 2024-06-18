#include "lectura.h"

t_list *subir_instrucciones(char *path)
{
    char *path_absoluto = obtener_path_instrucciones();
    string_append(&path_absoluto, path);
    FILE *archivo = fopen(path_absoluto, "r");
    if (archivo == NULL)
    {
        // loggear algo
        return NULL;
    }

    t_list *instrucciones = list_create();
    char *instruccion;

    while ((instruccion = leer_instruccion(archivo)) != NULL)
    {
        list_add(instrucciones, instruccion);
    }

    fclose(archivo);
    return instrucciones;
}

char *leer_instruccion(FILE *archivo)
{
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), archivo) != NULL)
    {
        size_t tamanio = strlen(buffer);
        if (tamanio > 0 && buffer[tamanio - 1] == '\n')
        {
            buffer[tamanio - 1] = '\0'; // Elimina el carácter de nueva línea
        }
        return strdup(buffer); // Duplica la cadena y la devuelve
    }
    return NULL;
}