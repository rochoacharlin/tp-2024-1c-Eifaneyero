#include "contexto_ejecucion.h"

t_contexto_ejecucion *contexto_ejecucion = NULL;

void iniciar_contexto()
{
    contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    contexto_ejecucion->PID = 0;
    contexto_ejecucion->registros_cpu = dictionary_create();
    contexto_ejecucion->rafaga_cpu_ejecutada = 0;
}

void destruir_contexto()
{
    dictionary_destroy_and_destroy_elements(contexto_ejecucion->registros_cpu, free);
    free(contexto_ejecucion);
    contexto_ejecucion = NULL;
}

void enviar_contexto_actualizado(int socket)
{
    t_paquete *paquete = crear_paquete(CONTEXTO_EJECUCION);

    agregar_a_paquete(paquete, (void *)&contexto_ejecucion->PID, sizeof(contexto_ejecucion->PID));
    agregar_registros_cpu_a_paquete(paquete, contexto_ejecucion->registros_cpu);
    agregar_a_paquete(paquete, (void *)&contexto_ejecucion->rafaga_cpu_ejecutada, sizeof(contexto_ejecucion->rafaga_cpu_ejecutada));

    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void agregar_registros_cpu_a_paquete(t_paquete *paquete, t_dictionary *registros_cpu)
{
    char name[3] = "AX", long_name[4] = "EAX";

    for (int i = 0; i < 4; i++)
    {
        char *registroAX = (char *)dictionary_get(registros_cpu, name);
        char *registroEAX = (char *)dictionary_get(registros_cpu, long_name);

        agregar_a_paquete(paquete, (void *)registroAX, sizeof(uint8_t));   // "A/B/C/D X"
        agregar_a_paquete(paquete, (void *)registroEAX, sizeof(uint32_t)); //  "E A/B/C/D X"

        name[0]++;
        long_name[1]++;
    }

    agregar_a_paquete(paquete, (void *)dictionary_get(registros_cpu, "PC"), sizeof(uint32_t));
    agregar_a_paquete(paquete, (void *)dictionary_get(registros_cpu, "SI"), sizeof(uint32_t));
    agregar_a_paquete(paquete, (void *)dictionary_get(registros_cpu, "DI"), sizeof(uint32_t));
}

// HORRIBLE? Si. Probar
void recibir_contexto_y_actualizar_global(int socket) // SIN opCode
{
    if (contexto_ejecucion != NULL)
        destruir_contexto();
    iniciar_contexto();
    t_list *registros_contexto = recibir_paquete(socket);

    memcpy(&(contexto_ejecucion->PID), (uint32_t *)list_get(registros_contexto, 0), sizeof(uint32_t));
    dictionary_put(contexto_ejecucion->registros_cpu, "AX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 1), 1));
    // dictionary_put(contexto_ejecucion->registros_cpu, "AX",
    //                memcpy(malloc(sizeof(uint8_t)), list_get(registros_contexto, 1), sizeof(uint8_t))); Correcto?
    dictionary_put(contexto_ejecucion->registros_cpu, "EAX",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 2), 4));
    dictionary_put(contexto_ejecucion->registros_cpu, "BX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 3), 1));
    dictionary_put(contexto_ejecucion->registros_cpu, "EBX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 4), 4));
    dictionary_put(contexto_ejecucion->registros_cpu, "CX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 5), 1));
    dictionary_put(contexto_ejecucion->registros_cpu, "ECX",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 6), 4));
    dictionary_put(contexto_ejecucion->registros_cpu, "DX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 7), 1));
    dictionary_put(contexto_ejecucion->registros_cpu, "EDX",
                   memset(malloc(sizeof(uint32_t *)), *(uint8_t *)list_get(registros_contexto, 8), 4));
    dictionary_put(contexto_ejecucion->registros_cpu, "PC",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 9), 4));
    dictionary_put(contexto_ejecucion->registros_cpu, "SI",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 10), 4));
    dictionary_put(contexto_ejecucion->registros_cpu, "DI",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 11), 4));
    memcpy(&(contexto_ejecucion->rafaga_cpu_ejecutada),
           (uint64_t *)list_get(registros_contexto, 12), sizeof(uint64_t));

    list_destroy_and_destroy_elements(registros_contexto, free);
}
