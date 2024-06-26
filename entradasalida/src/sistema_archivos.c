#include "sistema_archivos.h"

FILE *bloques;
FILE *bitmap;
int bloque_utilizados;
t_bitarray *bitarray;

void crear_archivo(char *nombre)
{
    char *path_dialfs = obtener_path_base_dialfs();
    size_t tam = strlen(nombre) + strlen(path_dialfs) + 1;
    char *path_absoluto = malloc_or_die(tam, "No se pudo crear espacio para el path_absoluto del archivo.");
    strcpy(path_absoluto, path_dialfs);
    strcat(path_absoluto, nombre);

    t_config *metadata = config_create(path_absoluto);
    config_set_value(metadata, "BLOQUE_INICIAL", string_itoa(bloque_utilizados++));
    config_set_value(metadata, "TAMANIO_ARCHIVO", "0");
}

void eliminar_archivo(char *nombre)
{
    // TODO
}

void truncar_archivo(char *nombre)
{
    // TODO
}

void leer_archivo(char *nombre)
{
    // TODO
}

void escribir_archivo(char *nombre)
{
    // TODO
}