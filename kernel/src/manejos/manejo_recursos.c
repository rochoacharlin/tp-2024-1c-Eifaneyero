#include "manejo_recursos.h"

int *instancias_recursos;
t_list *recursos;
char **nombres_recursos;

void crear_colas_de_bloqueo(void)
{
    instancias_recursos = NULL;
    recursos = list_create();
    nombres_recursos = obtener_recursos();
    char **instancias_aux = obtener_instancias_recursos();

    for (int i = 0; i < string_array_size(instancias_aux); i++)
    {
        int instancia_en_entero = atoi(instancias_aux[i]);
        instancias_recursos = realloc(instancias_recursos, (i + 1) * sizeof(int));
        instancias_recursos[i] = instancia_en_entero;

        t_list *cola_bloqueo = list_create();
        list_add(recursos, (void *)cola_bloqueo);
    }

    destruir_lista_string(instancias_aux);
}

void destruir_lista_string(char **lista_string)
{
    for (int i = 0; i < string_array_size(lista_string); i++)
        free(lista_string[i]);
    free(lista_string);
}