#include <gestion_memoria.h>

void *espacio_usuario;
t_dictionary *indice_tablas;
t_dictionary *indice_instrucciones;
t_bitarray *marcos_libres;

pthread_mutex_t mutex_memoria;

void inicializar_memoria(void)
{
    espacio_usuario = malloc(obtener_tam_memoria());
    memset(espacio_usuario, 0, obtener_tam_memoria()); // Inicializo con 0

    indice_tablas = crear_indice_de_tablas();

    marcos_libres = crear_marcos_libres();

    indice_instrucciones = crear_indice_de_instrucciones();

    pthread_mutex_init(&mutex_memoria, NULL);
}

// No sé donde ponerles:
// Interpreto que los registros de memoria son de 32 bits
uint32_t leer_espacio_usuario(uint32_t direccion)
{
    uint32_t valor;

    pthread_mutex_lock(&mutex_memoria);
    memcpy(&valor, espacio_usuario + direccion, sizeof(uint32_t));
    pthread_mutex_unlock(&mutex_memoria);

    return valor;
}

// Escribo sin importar si me voy de la página.
void escribir_espacio_usuario(uint32_t direccion, uint32_t valor)
{
    pthread_mutex_lock(&mutex_memoria);
    memcpy(espacio_usuario + direccion, &valor, sizeof(int));
    pthread_mutex_unlock(&mutex_memoria);
}
