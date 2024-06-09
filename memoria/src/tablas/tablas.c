#include "tablas.h"

int tamanio_pagina;

t_dictionary *crear_indice_de_tablas()
{
    t_dictionary *diccionario = dictionary_create();
    return diccionario;
}

void destruir_indice_de_tablas(t_dictionary *indice_de_tablas)
{
    dictionary_destroy_and_destroy_elements(indice_de_tablas, free);
}

void agregar_proceso_al_indice(t_dictionary *indice_de_tablas, uint32_t PID, int *tabla_de_paginas)
{
    dictionary_put(indice_de_tablas, string_itoa(PID), tabla_de_paginas);
}

void quitar_proceso_del_indice(t_dictionary *indice_de_tablas, uint32_t PID)
{
    int *tabla_de_paginas = dictionary_remove(indice_de_tablas, string_itoa(PID));
    if (tabla_de_paginas != NULL)
    {
        free(tabla_de_paginas);
    }
}

// TODO: Cambiar nombre
int *obtener_tabla_paginas(t_dictionary *indice_de_tablas, uint32_t PID)
{
    return dictionary_get(indice_de_tablas, string_itoa(PID));
}
