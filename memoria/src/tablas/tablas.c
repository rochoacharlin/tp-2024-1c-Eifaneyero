#include "tablas.h"

int tamanio_pagina;

// INDICE DE TABLA DE PAGINAS ---------------------
t_dictionary *crear_indice_de_tablas()
{
    t_dictionary *diccionario = dictionary_create();
    return diccionario;
}

void destruir_indice_tablas()
{
    dictionary_destroy_and_destroy_elements(indice_tablas, free);
}

void agregar_proceso_al_indice(uint32_t PID, t_list *tabla_de_paginas)
{
    dictionary_put(indice_tablas, string_itoa(PID), tabla_de_paginas);
}

void quitar_proceso_del_indice(uint32_t PID)
{
    int *tabla_de_paginas = dictionary_remove(indice_tablas, string_itoa(PID));
    if (tabla_de_paginas != NULL)
    {
        free(tabla_de_paginas);
    }
}

//
t_list *obtener_tp_de_proceso(uint32_t PID)
{
    return (t_list *)dictionary_get(indice_tablas, string_itoa(PID));
}

// TABLA DE PÁGINAS -------------------------------

void agregar_pagina(t_list *tp)
{
    int nuevo_marco = obtener_marco_libre();
    marcar_como_ocupado(nuevo_marco);
    list_add(tp, &nuevo_marco);
}

void quitar_ultima_pagina(t_list *tp)
{
    int indice = list_size(tp) - 1;
    marcar_como_libre(obtener_marco(tp, indice));
    list_remove(tp, indice);
}

int obtener_marco(t_list *tp, int pagina)
{
    return *(int *)list_get(tp, pagina);
}

int buscar_marco(uint32_t PID, int pagina)
{
    t_list *tp_de_proceso = obtener_tp_de_proceso(PID);
    int marco = obtener_marco(tp_de_proceso, pagina);
    return marco;
}
