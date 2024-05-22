#include "utils/contexto_ejecucion.h"

t_contexto_ejecucion *contexto_ejecucion = NULL;

void iniciar_contexto()
{
    contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    contexto_ejecucion->PID = 0;
    // contexto_ejecucion->program_counter = 0; Definimos que queda en los registros de CPU nada mas?
    contexto_ejecucion->registros_cpu = dictionary_create(); // ó crear_diccionario_registros_de_cpu()
    contexto_ejecucion->rafaga_cpu_ejecutada = 0;
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

        agregar_a_paquete(paquete, (void *)registroAX, sizeof(char) * 8);   // "A/B/C/D X"
        agregar_a_paquete(paquete, (void *)registroEAX, sizeof(char) * 32); //  "E A/B/C/D X"

        name[0]++;
        long_name[1]++;
    }

    agregar_a_paquete(paquete, (void *)dictionary_get(registros_cpu, "PC"), sizeof(char) * 32);
    agregar_a_paquete(paquete, (void *)dictionary_get(registros_cpu, "SI"), sizeof(char) * 32);
    agregar_a_paquete(paquete, (void *)dictionary_get(registros_cpu, "DI"), sizeof(char) * 32);
}

// Hay otra función para recibir OpCode
/* TODO:
void recibir_contexto_actualizado(int socket)
{
    if (contexto_ejecucion != NULL)
        destruir_contexto();
    iniciar_contexto();
    int size, desplazamiento = 0;
    void *buffer;

    buffer = recibir_buffer(socket, &size);

    // Desplazamiento: Tamaño de PID, PID, y tamaño de program_counter.
    desplazamiento += sizeof(int); // tamaño del opCode
    memcpy(&(contexto_ejecucion->pid), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(contexto_ejecucion->pid) + sizeof(int);

    deserializar_registros(buffer, &desplazamiento);

    // Desplazamiento: Tamaño de la rafaga de CPU ejecutada.
    desplazamiento += sizeof(int);
    memcpy(&(contexto_ejecucion->rafaga_cpu_ejecutada), buffer + desplazamiento, sizeof(uint64_t));

    free(buffer);
}
*/
/*
TODO:

void deserializar_registros(void * buffer, int * desplazamiento) {
    dictionary_clean_and_destroy_elements(contexto_ejecucion->registros_cpu, free);

    char *temp, nombre[3] = "AX", nombre_largo[4] = "EAX";

    for (int i = 0; i < 4; i++) {
        ssize_t tamanio_actual = sizeof(char) * (4 * pow(2, i) + 1);
        for (int j = 0; j < 4; j++) {
            temp = malloc(tamanio_actual);

            // Desplazamiento: Registro actual y tamaño del proximo registro.
            // (Para el ultimo registro pasa a ser el tamaño del comando de desalojo)
            memcpy(temp, buffer + (*desplazamiento), tamanio_actual);
            (*desplazamiento) += tamanio_actual + sizeof(int);
            //debug("%s", temp);

            //char * auxiliar =
                //string_substring(temp, 0, tamanio_actual - 1);
            dictionary_put(contexto_ejecucion->registros_cpu, (i) ? nombre_largo : nombre, temp);
            //free(temp);
            nombre[0]++, nombre_largo[1]++;
        }
        nombre_largo[1] = 'A', nombre_largo[0] = (i == 1) ? 'R' : 'E';
    }
}
*/

/*

void destruir_contexto() {
    dictionary_destroy_and_destroy_elements(contexto_ejecucion->registros_cpu, free);
    free(contexto_ejecucion);
    contexto_ejecucion = NULL;
}
*/
