#include <gestion_memoria.h>

void *espacio_usuario;
t_dictionary *indice_tablas;
t_dictionary *indice_instrucciones;
t_bitarray *marcos_disponibles;

void inicializar_memoria(void)
{
    espacio_usuario = malloc(obtener_tam_memoria());
    memset(espacio_usuario, 0, obtener_tam_memoria()); // Inicializo con 0

    indice_tablas = crear_indice_de_tablas();
    int cant_marcos = (obtener_tam_memoria() + obtener_tam_pagina() - 1) / obtener_tam_pagina();
    crear_marcos_memoria(cant_marcos);

    indice_instrucciones = crear_indice_de_instrucciones();
}

void crear_marcos_memoria(int cantidad)
{
    // marcos_disponibles = bitarray_create_with_mode(espacio_usuario, cant_marcos / 8, LSB_FIRST); // TODO: https://github.com/sisoputnfrba/foro/issues/3790
}
