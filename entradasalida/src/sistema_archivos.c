#include "sistema_archivos.h"

t_bitarray *bitmap;
t_list *fcbs;
void *bloques;
void *espacio_bitmap;
size_t tamanio_bitmap;

void iniciar_bitmap()
{
    char *path = string_new();
    string_append(&path, obtener_path_base_dialfs());
    string_append(&path, "/bitmap.dat");
    FILE *fbitmap = fopen(path, "wb+");
    free(path);
    if (fbitmap == NULL)
    {
        log_error(logger_propio, "No se pudo crear o abrir el archivo bitmap.dat: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    tamanio_bitmap = (obtener_block_count() + 8 - 1) / 8;
    ftruncate(fileno(fbitmap), tamanio_bitmap);

    espacio_bitmap = mmap(NULL, tamanio_bitmap, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fbitmap), 0);
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
    string_append(&path, "/bloques.dat");
    FILE *fbloques = fopen(path, "wb+");
    free(path);
    if (fbloques == NULL)
    {
        log_error(logger_propio, "No se pudo crear o abrir el archivo bloques.dat: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    size_t tamanio = obtener_block_count() * obtener_block_size();
    ftruncate(fileno(fbloques), tamanio);

    bloques = mmap(NULL, tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fbloques), 0);
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

    t_config *config;
    char *config_ruta;
    t_fcb *fcb;
    while ((de = readdir(drmetadata)) != NULL)
    {
        // abro config
        config_ruta = string_new();
        string_append(&config_ruta, path);
        string_append(&config_ruta, de->d_name);
        config = iniciar_config(logger_propio, config_ruta);

        if (config == NULL)
        {
            log_error(logger_propio, "No se encontró el archivo en %s", config_ruta);
            free(config_ruta);
            exit(EXIT_FAILURE);
        }
        else
        {
            free(config_ruta);
        }

        // cargo fcb
        // crear estructura de fcb y guardarla en una lista. Por qué una lista? Porque se puede filtrar (eliminar comentario)
        fcb = malloc(sizeof(fcb));
        fcb->nombre = string_duplicate(de->d_name);
        fcb->bloque_inicial = config_get_int_value(config, "BLOQUE_INICIAL");
        fcb->tamanio_en_bytes = config_get_int_value(config, "TAMANIO_ARCHIVO");

        cargar_fcb(fcb);

        config_destroy(config);
    }
    closedir(drmetadata);
}

void cargar_fcb(t_fcb *fcb)
{
    list_add_sorted(fcbs, fcb, ordenar_fcb_por_bloque_inicial);
}

bool ordenar_fcb_por_bloque_inicial(void *fcb1, void *fcb2)
{
    return ((t_fcb *)fcb1)->bloque_inicial < ((t_fcb *)fcb2)->bloque_inicial;
}

int obtener_bloque_libre(void)
{
    int bloque = -1;

    for (int i = 0; i < obtener_block_count() && bloque < 0; i++)
    {
        if (!bitarray_test_bit(bitmap, i))
        {
            bloque = i;
        }
    }

    return bloque;
}

void crear_archivo(uint32_t *PID, char *nombre)
{
    char *path_dialfs = obtener_path_base_dialfs();
    size_t tam = strlen(nombre) + strlen(path_dialfs) + 1;
    char *path_absoluto = malloc_or_die(tam, "No se pudo crear espacio para el path_absoluto del archivo.");
    strcpy(path_absoluto, path_dialfs);
    strcat(path_absoluto, nombre);

    FILE *metadataArchivo = fopen(path_absoluto, "w");
    fclose(metadataArchivo);
    t_config *metadata = iniciar_config(logger_propio, path_absoluto);

    int bloque_libre = obtener_bloque_libre();
    config_set_value(metadata, "BLOQUE_INICIAL", string_itoa(bloque_libre));
    bitarray_set_bit(bitmap, bloque_libre);
    config_set_value(metadata, "TAMANIO_ARCHIVO", "0");
    config_save(metadata);

    // log minimo y obligatorio
    loggear_dialfs_crear_archivo(*PID, nombre);
}

void eliminar_archivo(uint32_t *PID, char *nombre)
{
    // log minimo y obligatorio
    loggear_dialfs_eliminar_archivo(*PID, nombre);

    liberar_archivo(nombre);
    eliminar_metadata(nombre);
}

void liberar_archivo(char *archivo)
{
    int pos_inicial = bloque_inicial(archivo) * obtener_block_size();
    int tam_en_bloques = tamanio_en_bloques(archivo);
    for (int i = pos_inicial; i < tam_en_bloques; i++)
    {
        bitarray_set_bit(bitmap, i);
    }
    msync(espacio_bitmap, tamanio_bitmap, MS_SYNC);
}

void eliminar_metadata(char *archivo)
{
    bool buscar_por_nombre(void *fcb)
    {
        return strcmp(((t_fcb *)fcb)->nombre, archivo) == 0;
    }
    void eliminar_fcb(void *data)
    {
        t_fcb *fcb = (t_fcb *)data;
        free(fcb->nombre);
        free(fcb);
    }
    list_remove_and_destroy_by_condition(fcbs, buscar_por_nombre, eliminar_fcb);

    char *path = string_new();
    string_append(&path, obtener_path_base_dialfs());
    string_append(&path, "metadata/");
    string_append(&path, archivo);
    remove(path);
    free(path);
}

int tamanio_en_bloques(char *archivo)
{
    t_fcb *fcb = metadata_de_archivo(archivo);
    return (fcb->tamanio_en_bytes + obtener_block_size() - 1) / obtener_block_size();
}

void truncar_archivo(uint32_t *PID, char *nombre, int tam)
{

    // log minimo y obligatorio
    loggear_dialfs_truncar_archivo(*PID, nombre, tam);
}

void *leer_archivo(uint32_t *PID, char *nombre, int tam, int puntero)
{
    int pos_inicial = bloque_inicial(nombre) * obtener_block_size() + puntero;
    void *lectura = malloc(tam);
    memcpy(lectura, bloques + pos_inicial, tam);

    // log minimo y obligatorio
    loggear_dialfs_leer_archivo(*PID, nombre, tam, puntero);

    return lectura;
}

int bloque_inicial(char *archivo)
{
    t_fcb *fcb = metadata_de_archivo(archivo);
    return fcb->bloque_inicial;
}

t_fcb *metadata_de_archivo(char *archivo)
{
    bool buscar_por_nombre(void *fcb)
    {
        return strcmp(((t_fcb *)fcb)->nombre, archivo) == 0;
    }
    t_list *filtrados = list_filter(fcbs, buscar_por_nombre);
    t_fcb *fcb = list_remove(filtrados, 0);
    list_destroy(filtrados);
    return fcb;
}

void escribir_archivo(uint32_t *PID, char *nombre, int tam, int puntero)
{

    // log minimo y obligatorio
    loggear_dialfs_escribir_archivo(*PID, nombre, tam, puntero);
}