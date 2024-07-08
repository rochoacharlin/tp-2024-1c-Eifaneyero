#include "sistema_archivos.h"

FILE *bloques;
int bloque_utilizados;
t_bitarray *bitmap;

void iniciar_bitmap()
{
    char *path = string_new();
    string_append(&path, obtener_path_base_dialfs());
    string_append(&path, "bitmap.dat");
    FILE *fbitmap = fopen(path, "wb");
    free(path);
    if (fbitmap == NULL)
    {
        log_error(logger_propio, "No se pudo crear o abrir el archivo bitmap.dat: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    size_t tamanio_bitmap = (obtener_block_count() + 8 - 1) / 8;
    ftruncate(fileno(fbitmap), tamanio_bitmap);

    void *espacio_bitmap = mmap(NULL, tamanio_bitmap, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fbitmap), 0);
    fclose(fbitmap);
    if (espacio_bitmap == MAP_FAILED)
    {
        log_error(logger_propio, "No se pudo mapear a memoria el archivo bitmap.dat: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    bitmap = bitarray_create_with_mode(espacio_bitmap, tamanio_bitmap, LSB_FIRST);
}

void leer_bloques()
{
    char *path = string_new();
    string_append(&path, obtener_path_base_dialfs());
    string_append(&path, "bloques.dat");
    FILE *fbloques = fopen(path, "wb");
    free(path);
    if (fbloques == NULL)
    {
        log_error(logger_propio, "No se pudo crear o abrir el archivo bloques.dat: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    size_t tamanio = obtener_block_count() * obtener_block_size();
    ftruncate(fileno(fbloques), tamanio);

    void *bloques = mmap(NULL, tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fbloques), 0);
    fclose(fbloques);
    if (bloques == MAP_FAILED)
    {
        log_error(logger_propio, "No se pudo mapear a memoria el archivo bloques.dat: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void leer_fcbs()
{
    // creo directorio para metadata si no existe
    char *path = string_new();
    string_append(&path, obtener_path_base_dialfs());
    string_append(&path, "metadata");
    if (mkdir(path, 0777) == -1)
    {
        free(path);
        log_error(logger_propio, "No se pudo crear el directorio de metadata: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // abro el directorio
    DIR *drmetadata = opendir(path);
    if (drmetadata == NULL)
    {
        free(path);
        log_error(logger_propio, "No se pudo abrir el directorio de metadata: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // leo la metadata
    struct dirent *de;
    while ((de = readdir(drmetadata)) != NULL)
    {
        // crear estructura de fcb y guardarla en una lista. Por qué una lista? Porque se puede filtrar
    }
    closedir(drmetadata);
}

void crear_archivo(uint32_t *PID, char *nombre)
{
    char *path_dialfs = obtener_path_base_dialfs();
    size_t tam = strlen(nombre) + strlen(path_dialfs) + 1;
    char *path_absoluto = malloc_or_die(tam, "No se pudo crear espacio para el path_absoluto del archivo.");
    strcpy(path_absoluto, path_dialfs);
    strcat(path_absoluto, nombre);

    t_config *metadata = config_create(path_absoluto);
    config_set_value(metadata, "BLOQUE_INICIAL", string_itoa(bloque_utilizados++));
    config_set_value(metadata, "TAMANIO_ARCHIVO", "0");

    // REVISAR: Falta algo mas?

    // log minimo y obligatorio
    loggear_dialfs_crear_archivo(*PID, nombre);
}

void eliminar_archivo(uint32_t *PID, char *nombre)
{
    // TODO
}

void truncar_archivo(uint32_t *PID, char *nombre, int tam)
{
    // TODO
}

void leer_archivo(uint32_t *PID, char *nombre, int tam, int *puntero)
{
    // TODO
}

void escribir_archivo(uint32_t *PID, char *nombre, int tam, int *puntero)
{
    // TODO
}