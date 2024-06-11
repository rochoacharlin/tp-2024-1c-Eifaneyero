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
