#include "conexiones.h"
// #include "./contexto_ejecucion.h"

void servidor(void)
{
    int server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha());
    log_info(logger_propio, "Kernel listo para recibir clientes");
    int cliente_fd = esperar_cliente(logger_propio, server_fd);
    log_info(logger_propio, "Se conectó un cliente!");

    int32_t handshake_esperado = 4;
    int handshake_respuesta = handshake_servidor(logger_propio, cliente_fd, handshake_esperado);
}

void conexion_interrupt_con_CPU(void)
{
    int conexion = crear_conexion(logger_propio, obtener_ip_cpu(), obtener_puerto_cpu_interrupt());
    int32_t handshake = 5;
    int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);
}

void conexion_dispatch_con_CPU(void)
{
    int conexion = crear_conexion(logger_propio, obtener_ip_cpu(), obtener_puerto_cpu_dispatch());
    int32_t handshake = 5;
    int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);
}

void conexion_con_memoria(void)
{
    int conexion = crear_conexion(logger_propio, obtener_ip_memoria(), obtener_puerto_memoria());
    int32_t handshake = 1;
    int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);
}

/*
t_contexto *gestionar_ejecucion_proceso(t_pcb *proceso_en_ejecucion)
{
    if (contexto_ejecucion != NULL)
        destruir_contexto(); // Verifica y destruye contexto previo
    iniciar_contexto();

    buffer_contexto = malloc(sizeof(t_buffer));

    asignar_pcb_a_contexto(proceso_en_ejecucion);

    enviar_contexto_actualizado(conexion_a_cpu); //Variable que contiene socket de conexion con CPU

    if (recibir_operacion_de_cpu() < 0) // TODO: recibir_operacion(conexion_a_cpu);
    {
        error("Se desconectó el CPU.");
    }

    recibir_contexto_actualizado(conexion_a_cpu);

    actualizar_pcb(proceso_en_ejecucion);
    free(buffer_contexto);
    return contexto_ejecucion;
}
*/

/*
void asignar_pcb_a_contexto(t_pcb* proceso) {
    list_destroy_and_destroy_elements(contexto_ejecucion->instrucciones, free);
    contexto_ejecucion->pid = proceso->pid;
    dictionary_destroy_and_destroy_elements(contexto_ejecucion->registros_cpu, free);
    contexto_ejecucion->registros_cpu = registros_de_la_cpu(proceso->registros_cpu); //TODO: registros_del_cpu
}
*/

/*

void actualizarPCB(t_pcb* proceso){
    proceso->pid = contexto_ejecucion->pid;
    // proceso->program_counter = contexto_ejecucion->program_counter;
    dictionary_destroy_and_destroy_elements(proceso->registros_cpu, free);
    proceso->registros_cpu = registros_de_la_cpu(contexto_ejecucion->registros_cpu);

}

*/

/*

//Copia diccionario de registros de CPU
t_dictionary *registros_de_la_cpu(t_dictionary *a_copiar) {
    t_dictionary *copia = dictionary_create();

    char* AX = malloc(sizeof(char) * (8 + 1));
    char* BX = malloc(sizeof(char) * (8 + 1));
    char* CX = malloc(sizeof(char) * (8 + 1));
    char* DX = malloc(sizeof(char) * (8 + 1));
    char* EAX = malloc(sizeof(char) * (32 + 1));
    char* EBX = malloc(sizeof(char) * (32 + 1));
    char* ECX = malloc(sizeof(char) * (32 + 1));
    char* EDX = malloc(sizeof(char) * (32 + 1));
    char* PC = malloc(sizeof(char) * (32 + 1));
    char* SI = malloc(sizeof(char) * (32 + 1));
    char* DI = malloc(sizeof(char) * (32 + 1));

    strncpy(AX, (char *)dictionary_get(aCopiar, "AX"), 8 + 1);
    strncpy(BX, (char *)dictionary_get(aCopiar, "BX"), 8 + 1);
    strncpy(CX, (char *)dictionary_get(aCopiar, "CX"), 8 + 1);
    strncpy(DX, (char *)dictionary_get(aCopiar, "DX"), 8 + 1);
    strncpy(EAX, (char *)dictionary_get(aCopiar, "EAX"), 32 + 1);
    strncpy(EBX, (char *)dictionary_get(aCopiar, "EBX"), 32 + 1);
    strncpy(ECX, (char *)dictionary_get(aCopiar, "ECX"), 32 + 1);
    strncpy(EDX, (char *)dictionary_get(aCopiar, "EDX"), 32 + 1);
    strncpy(PC, (char *)dictionary_get(aCopiar, "PC"), 32 + 1);
    strncpy(SI, (char *)dictionary_get(aCopiar, "SI"), 32 + 1);
    strncpy(DI, (char *)dictionary_get(aCopiar, "DI"), 32 + 1);

    dictionary_put(copia, "AX", AX);
    dictionary_put(copia, "BX", BX);
    dictionary_put(copia, "CX", CX);
    dictionary_put(copia, "DX", DX);
    dictionary_put(copia, "EAX", EAX);
    dictionary_put(copia, "EBX", EBX);
    dictionary_put(copia, "ECX", ECX);
    dictionary_put(copia, "EDX", EDX);
    dictionary_put(copia, "PC", PC);
    dictionary_put(copia, "SI", SI);
    dictionary_put(copia, "DI", DI);

    return copia;
}

*/