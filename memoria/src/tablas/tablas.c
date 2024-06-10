#include "tablas.h"

// ------------ INDICE DE TABLA DE PAGINAS --------- //
// Dict IndTabPag: Key = PID ; Value = t_list * Tablas de Pag . Index list = Pag ; Elem list = marco // Dejo esto porque soy lento

t_dictionary *crear_indice_de_tablas()
{
    t_dictionary *diccionario = dictionary_create();
    return diccionario;
}

void destruir_tabla_de_paginas(t_list *tp)
{
    list_destroy_and_destroy_elements(tp, free);
}

void destruir_indice_tablas()
{
    dictionary_destroy_and_destroy_elements(indice_tablas, destruir_tabla_de_paginas);
}

void agregar_proceso_al_indice(uint32_t PID)
{
    loggear_creacion_destruccion_tabla_de_paginas(PID, 0);

    t_list *tabla_de_paginas = list_create();
    dictionary_put(indice_tablas, string_itoa(PID), tabla_de_paginas);
}

void quitar_proceso_del_indice(uint32_t PID)
{
    int tp_size = list_size(obtener_tp_de_proceso(PID));
    loggear_creacion_destruccion_tabla_de_paginas(PID, tp_size);

    int *tabla_de_paginas = dictionary_remove(indice_tablas, string_itoa(PID));
    destruir_tabla_de_paginas(tabla_de_paginas);
}

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

void liberar_marcos_proceso(uint32_t PID)
{
    t_list *tabla_de_paginas = obtener_tp_de_proceso(PID);
    if (tabla_de_paginas != NULL)
    {
        for (int i = 0; i < list_size(tabla_de_paginas); i++)
        {
            int marco = obtener_marco(tabla_de_paginas, i);
            marcar_como_libre(marco);
        }
    }
}
