#include "contexto_ejecucion.h"

t_contexto *iniciar_contexto(void)
{
    t_contexto *contexto = malloc(sizeof(t_contexto));
    contexto->PID = 0;
    contexto->registros_cpu = dictionary_create();
    return contexto;
}

void destruir_contexto(t_contexto *contexto)
{
    dictionary_destroy_and_destroy_elements(contexto->registros_cpu, free);
    free(contexto);
}

// AX, EAX, BX, EBX, CX, ECX, DX, EDX, PC, SI, DI
void agregar_registros_cpu_a_paquete(t_paquete *paquete, t_dictionary *registros_cpu)
{
    char name[3] = "AX", long_name[4] = "EAX";

    for (int i = 0; i < 4; i++)
    {
        char *registroAX = (char *)dictionary_get(registros_cpu, name);
        char *registroEAX = (char *)dictionary_get(registros_cpu, long_name);

        agregar_a_paquete(paquete, (void *)registroAX, sizeof(uint8_t));
        agregar_a_paquete(paquete, (void *)registroEAX, sizeof(uint32_t));

        name[0]++;
        long_name[1]++;
    }

    agregar_a_paquete(paquete, (void *)dictionary_get(registros_cpu, "PC"), sizeof(uint32_t));
    agregar_a_paquete(paquete, (void *)dictionary_get(registros_cpu, "SI"), sizeof(uint32_t));
    agregar_a_paquete(paquete, (void *)dictionary_get(registros_cpu, "DI"), sizeof(uint32_t));
}

void agregar_contexto_a_paquete(t_contexto *contexto, t_paquete *paquete)
{
    agregar_a_paquete(paquete, (void *)&contexto->PID, sizeof(contexto->PID));
    agregar_registros_cpu_a_paquete(paquete, contexto->registros_cpu);
}

void enviar_contexto(int socket, t_contexto *contexto)
{
    t_paquete *paquete = crear_paquete(CONTEXTO_EJECUCION);

    agregar_a_paquete(paquete, (void *)&contexto->PID, sizeof(contexto->PID));
    agregar_registros_cpu_a_paquete(paquete, contexto->registros_cpu);

    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

// TODO: PROBAR!
t_contexto *recibir_contexto(int socket) // SIN opCode
{
    t_contexto *contexto = iniciar_contexto();
    t_list *registros_contexto = recibir_paquete(socket);

    memcpy(&(contexto->PID), (uint32_t *)list_get(registros_contexto, 0), sizeof(uint32_t)); //
    dictionary_put(contexto->registros_cpu, "AX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 1), 1));
    // dictionary_put(contexto->registros_cpu, "AX",
    //                memcpy(malloc(sizeof(uint8_t)), list_get(registros_contexto, 1), sizeof(uint8_t))); Correcto?
    dictionary_put(contexto->registros_cpu, "EAX",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 2), 4));
    dictionary_put(contexto->registros_cpu, "BX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 3), 1));
    dictionary_put(contexto->registros_cpu, "EBX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 4), 4));
    dictionary_put(contexto->registros_cpu, "CX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 5), 1));
    dictionary_put(contexto->registros_cpu, "ECX",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 6), 4));
    dictionary_put(contexto->registros_cpu, "DX",
                   memset(malloc(sizeof(uint8_t *)), *(uint8_t *)list_get(registros_contexto, 7), 1));
    dictionary_put(contexto->registros_cpu, "EDX",
                   memset(malloc(sizeof(uint32_t *)), *(uint8_t *)list_get(registros_contexto, 8), 4));
    dictionary_put(contexto->registros_cpu, "PC",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 9), 4));
    dictionary_put(contexto->registros_cpu, "SI",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 10), 4));
    dictionary_put(contexto->registros_cpu, "DI",
                   memset(malloc(sizeof(uint32_t *)), *(uint32_t *)list_get(registros_contexto, 11), 4));

    list_destroy_and_destroy_elements(registros_contexto, free);

    return contexto;
}