#include "lectura.h"

t_dictionary *crear_indice_de_instrucciones()
{
    t_dictionary *diccionario = dictionary_create();
    return diccionario;
}

void destruir_lista_instrucciones(void *tp)
{
    list_destroy_and_destroy_elements(tp, free);
}

void destruir_indice_de_instrucciones(t_dictionary *indice_de_instrucciones)
{
    dictionary_destroy_and_destroy_elements(indice_de_instrucciones, destruir_lista_instrucciones);
}

void agregar_instrucciones_al_indice(t_dictionary *indice_de_instrucciones, uint32_t PID, char *path)
{
    t_list *instrucciones_de_proceso = subir_instrucciones(path); // sube las instrucciones a la estructura de memoria
    dictionary_put(indice_de_instrucciones, string_itoa(PID), instrucciones_de_proceso);
}

void quitar_instrucciones_al_indice(t_dictionary *indice_de_instrucciones, uint32_t PID)
{
    t_list *instrucciones_de_proceso = dictionary_remove(indice_de_instrucciones, string_itoa(PID));
    if (instrucciones_de_proceso != NULL)
    {
        list_destroy_and_destroy_elements(instrucciones_de_proceso, free);
    }
}
