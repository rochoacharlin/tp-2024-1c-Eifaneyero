#include "sistema_archivos.h"

t_bitarray *bitmap;
t_list *fcbs;
void *bloques;
void *espacio_bitmap;
size_t tamanio_bitmap;

void iniciar_bitmap(void)
{
    char *path = string_new();
    string_append_with_format(&path, "%s/%s", obtener_path_base_dialfs(), "bitmap.dat");
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

void leer_bloques(void)
{
    char *path = string_new();
    string_append_with_format(&path, "%s/%s", obtener_path_base_dialfs(), "bloques.dat");
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
    string_append_with_format(&path, "%s/metadata", obtener_path_base_dialfs());
    if (mkdir(path, 0777) == -1 && errno != EEXIST)
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
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
        {
            // abro config
            config_ruta = string_new();
            string_append_with_format(&config_ruta, "%s/%s", path, de->d_name);
            config = iniciar_config(logger_propio, config_ruta);

            if (config == NULL)
            {
                log_error(logger_propio, "No se encontró el archivo en %s", config_ruta);
                free(config_ruta);
                exit(EXIT_FAILURE);
            }
            else
                free(config_ruta);

            // cargo fcb
            fcb = malloc(sizeof(fcb));
            fcb->nombre = string_duplicate(de->d_name);
            fcb->bloque_inicial = config_get_int_value(config, "BLOQUE_INICIAL");
            fcb->tamanio_en_bytes = config_get_int_value(config, "TAMANIO_ARCHIVO");

            cargar_fcb(fcb);
        }
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
            bloque = i;
    }

    return bloque;
}

void crear_archivo(uint32_t *PID, char *nombre)
{
    char *path_absoluto = string_new();
    string_append_with_format(&path_absoluto, "%s/metadata/%s", obtener_path_base_dialfs(), nombre);

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
        bitarray_set_bit(bitmap, i);
    msync(espacio_bitmap, tamanio_bitmap, MS_SYNC);
}

void destruir_fcb(void *data)
{
    t_fcb *fcb = (t_fcb *)data;
    free(fcb->nombre);
    free(fcb);
}

void eliminar_metadata(char *archivo)
{
    bool buscar_por_nombre(void *fcb) { return strcmp(((t_fcb *)fcb)->nombre, archivo) == 0; }

    list_remove_and_destroy_by_condition(fcbs, buscar_por_nombre, destruir_fcb);

    char *path = string_new();
    string_append_with_format(&path, "%s/metadata/%s", obtener_path_base_dialfs(), archivo);
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
    char *path = string_new();
    string_append_with_format(&path, "%s/metadata/%s", obtener_path_base_dialfs(), nombre);

    FILE *archivo = fopen(path, "w");
    t_fcb *fcb = metadata_de_archivo(archivo);

    // Obtengo el bloque inicial del archivo y su tamaño actual
    int bloque_inicial = fcb->bloque_inicial;
    int tamanio_anterior = fcb->tamanio_en_bytes;

    // Calcular el nuevo tamaño en bloques,  REVISAR
    int nuevos_bloques = bytes_a_bloques(tam);
    int bloques_anteriores = bytes_a_bloques(tamanio_anterior);

    // Si el nuevo tamaño es menor, se liberan los bloques adicionales
    if (nuevos_bloques < bloques_anteriores)
    {
        for (int i = nuevos_bloques; i < bloques_anteriores; i++)
        {
            int bloque_a_liberar = bloque_inicial + i;
            bitarray_clean_bit(bitmap, bloque_a_liberar);
        }
    }

    // Si es necesario compactar, llamar a la función compactar
    if (validar_compactacion(nuevos_bloques, fcb))
    {
        compactar(PID, archivo, tam);
    }

    // Actualizar metadata

    config_set_value(metadata, "TAMANIO_ARCHIVO", string_itoa(tam));
    config_save(metadata);

    free(path);
    close(archivo);

    // log minimo y obligatorio
    loggear_dialfs_truncar_archivo(*PID, nombre, tam);
}

bool validar_compactacion(int bloque_agregados, t_fcb *fcb)
{
    t_list_iterator *fcb_iterando = list_iterator_create(fcbs);

    while (list_iterator_has_next(fcb_iterando))
    {

        t_fcb *fcb_actual = list_iterator_next(fcb_iterando);
        int bloques_de_siguiente = ((t_fcb *)list_iterator_next(fcb_iterando))->bloque_inicial;

        if (strcmp(fcb->nombre, fcb_actual->nombre) && (bloques_de_siguiente - bloque_agregados < 0))
        {
            return true;
        }
    }

    return false;
}

void compactar(uint32_t *PID, t_fcb *archivo_a_truncar, int tamanio_execedente_en_bloques)
{
    // log minimo y obligatorio
    loggear_dialfs_inicio_compactacion(*PID);

    usleep(obtener_retraso_compactacion() * 1000);

    // Ordenar la lista de FCBs por bloque inicial
    // ordenar_fcb_por_bloque_inicial(fcbs); // rocio dice que no hace falta

    // Copiar el contenido del archivo a truncar en un auxiliar y eliminarlo de la lista
    void *archivo_auxiliar = malloc(bytes_a_bloques(archivo_a_truncar->tamanio_en_bytes) * obtener_block_size());
    void *src = bloques + archivo_a_truncar->bloque_inicial * obtener_block_size();
    memcpy(archivo_auxiliar, src, bytes_a_bloques(archivo_a_truncar->tamanio_en_bytes) * obtener_block_size());
    list_remove_element(fcbs, (void *)archivo_a_truncar);

    t_list_iterator *fcbs_iterando = list_iterator_create(fcbs);
    int bloque_final = 0;

    if (((t_fcb *)list_get(fcbs, 0))->bloque_inicial != 0)
    {
        t_fcb *primer_fcb = list_get(fcbs, 0);
        mover_fcb(primer_fcb, 0);
    }

    while (list_iterator_has_next(fcbs_iterando))
    {
        t_fcb *fcb_actual = list_iterator_next(fcbs_iterando);
        int bloque_inicial_actual = fcb_actual->bloque_inicial;
        int ultimo_bloque_actual = bytes_a_bloques(fcb_actual->tamanio_en_bytes) + bloque_inicial_actual;

        if (list_iterator_has_next(fcbs_iterando))
        {
            t_fcb *fcb_siguiente = list_iterator_next(fcbs_iterando);
            int bloque_inicial_siguiente = fcb_siguiente->bloque_inicial;
            // int ultimo_bloque_siguiente = bytes_a_bloques(fcb_siguiente->tamanio_en_bytes) + bloque_inicial_siguiente;

            if (ultimo_bloque_actual + 1 != bloque_inicial_siguiente)
            {
                mover_contenido_fcb(fcb_siguiente, ultimo_bloque_actual + 1, bloques + bloque_inicial_siguiente * obtener_block_size());
                fcb_siguiente->bloque_inicial = ultimo_bloque_actual + 1;
                bloque_final = ultimo_bloque_actual + 1 + bytes_a_bloques(fcb_siguiente->tamanio_en_bytes);
                actualizar_metadata(fcb_siguiente);
            }
        }
        else
            bloque_final = ultimo_bloque_actual;
    }

    list_iterator_destroy(fcbs_iterando);

    // Pegar el contenido de archivo_auxiliar desde bloque_final
    void *dst = bloques + (bloque_final + 1) * obtener_block_size();
    memcpy(dst, archivo_auxiliar, bytes_a_bloques(archivo_a_truncar->tamanio_en_bytes) * obtener_block_size());

    // Actualizar el FCB del archivo truncado con su nuevo tamaño y bloque inicial
    archivo_a_truncar->bloque_inicial = bloque_final + 1;
    archivo_a_truncar->tamanio_en_bytes += tamanio_execedente_en_bloques;
    actualizar_metadata(archivo_a_truncar);

    // Actualizar el bitmap
    for (int i = 0; i < bytes_a_bloques(archivo_a_truncar->tamanio_en_bytes); i++)
        bitarray_set_bit(bitmap, archivo_a_truncar->bloque_inicial + i);

    free(archivo_auxiliar);

    msync(espacio_bitmap, tamanio_bitmap, MS_SYNC);
    msync(bloques, obtener_block_count() * obtener_block_size(), MS_SYNC);

    // log minimo y obligatorio
    loggear_dialfs_fin_compactacion(*PID);
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
    bool buscar_por_nombre(void *fcb) { return strcmp(((t_fcb *)fcb)->nombre, archivo) == 0; }
    t_list *filtrados = list_filter(fcbs, buscar_por_nombre);
    t_fcb *fcb = list_remove(filtrados, 0);
    list_destroy(filtrados);
    return fcb;
}

void escribir_archivo(uint32_t *PID, char *nombre, int tam, int puntero, void *dato_a_escribir)
{
    int pos_inicial = bloque_inicial(nombre) * obtener_block_size() + puntero;
    memcpy(bloques + pos_inicial, dato_a_escribir, tam);
    msync(bloques, obtener_block_count() * obtener_block_size(), MS_SYNC);
    // log minimo y obligatorio
    loggear_dialfs_escribir_archivo(*PID, nombre, tam, puntero);
}

void actualizar_metadata(t_fcb *fcb)
{
    // abro config
    char *config_ruta = string_new();
    string_append_with_format(&config_ruta, "%s/metadata/%s", obtener_path_base_dialfs(), fcb->nombre);
    t_config *config = iniciar_config(logger_propio, config_ruta);

    // seteo valores
    char *bloque_inicial = string_itoa(fcb->bloque_inicial);
    char *tamanio_en_bytes = string_itoa(fcb->tamanio_en_bytes);
    config_set_value(config, "BLOQUE_INICIAL", bloque_inicial);
    config_set_value(config, "TAMANIO_ARCHIVO", tamanio_en_bytes);

    // grabo config
    config_save(config);

    // cierro config y libero valores
    free(bloque_inicial);
    free(tamanio_en_bytes);
    config_destroy(config);
}

void mover_fcb(t_fcb *fcb, int nuevo_inicio)
{
    int viejo_inicio = fcb->bloque_inicial;
    void *src = bloques + viejo_inicio * obtener_block_size();
    mover_contenido_fcb(fcb, nuevo_inicio, src);
}

void mover_contenido_fcb(t_fcb *fcb, int nuevo_inicio, void *src_contenido)
{
    int viejo_inicio = fcb->bloque_inicial;
    int tam_en_bloques = bytes_a_bloques(fcb->tamanio_en_bytes);
    void *dst = bloques + nuevo_inicio * obtener_block_size();
    memcpy(dst, src_contenido, tam_en_bloques * obtener_block_size());

    // Actualizar el bitmap
    for (int i = 0; i < tam_en_bloques; i++)
    {
        bitarray_clean_bit(bitmap, viejo_inicio + i);
        bitarray_set_bit(bitmap, nuevo_inicio + i);
    }

    // Actualizar el FCB del siguiente con su nuevo bloque inicial
    fcb->bloque_inicial = nuevo_inicio;
}

int bytes_a_bloques(int bytes)
{
    return (bytes + obtener_block_size() - 1) / obtener_block_size();
}