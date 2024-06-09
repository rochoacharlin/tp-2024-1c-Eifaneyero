#include "planificacion.h"

int rafaga_cpu_ejecutada;

void planificar_a_corto_plazo_segun_algoritmo(void)
{
    if (strcmp(algoritmo, "FIFO") == 0 || strcmp(algoritmo, "RR") == 0)
    {
        planificar_a_corto_plazo(proximo_a_ejecutar_segun_FIFO_o_RR);
    }
    else if (strcmp(algoritmo, "VRR") == 0)
    {
        planificar_a_corto_plazo(proximo_a_ejecutar_segun_VRR);
    }
    else
    {
        log_error(logger_propio, "Algoritmo invalido. Debe ingresar FIFO, RR o VRR");
        abort();
    }
}

void planificar_a_corto_plazo(t_pcb *(*proximo_a_ejecutar)())
{
    crear_colas_de_bloqueo();
    while (1)
    {
        sem_wait(&hay_pcbs_READY);
        pcb_en_EXEC = proximo_a_ejecutar();

        estado anterior = pcb_en_EXEC->estado;
        pcb_en_EXEC->estado = EXEC;

        // log minimo y obligatorio
        loggear_cambio_de_estado(pcb_en_EXEC->PID, anterior, pcb_en_EXEC->estado);

        procesar_pcb_segun_algoritmo(pcb_en_EXEC);
        esperar_contexto_y_actualizar_pcb(pcb_en_EXEC);
    }
}

t_pcb *proximo_a_ejecutar_segun_FIFO_o_RR(void)
{
    return desencolar_pcb(pcbs_en_READY);
}

t_pcb *proximo_a_ejecutar_segun_VRR(void)
{
    t_pcb *pcb;
    if (!list_is_empty(pcbs_en_aux_READY))
        pcb = desencolar_pcb(pcbs_en_aux_READY);
    else
        pcb = desencolar_pcb(pcbs_en_READY);

    return pcb;
}

void encolar_pcb_segun_algoritmo(t_pcb *pcb, int rafaga_cpu)
{
    int rafaga_restante = pcb->quantum - rafaga_cpu;
    if (strcmp(algoritmo, "FIFO") == 0 || strcmp(algoritmo, "FIFO") == 0 || (strcmp(algoritmo, "VRR") == 0 && rafaga_restante == 0))
    {
        ingresar_pcb_a_READY(pcb);
    }
    else if (strcmp(algoritmo, "VRR") == 0 && rafaga_restante > 0)
    {
        encolar_pcb(pcbs_en_aux_READY, pcb);

        sem_post(&hay_pcbs_READY);

        // log minimo y obligatorio
        lista_PIDS = string_new();
        mostrar_PIDS(pcbs_en_READY);
        mostrar_PIDS(pcbs_en_aux_READY);
        loggear_ingreso_a_READY(lista_PIDS);
        free(lista_PIDS);

        // COMPLETAR: Garantizar que cuando este proceso vaya a EXEC ejecute solo la rafaga restante
    }
    else
    {
        log_error(logger_propio, "Algoritmo invalido. Debe ingresar FIFO, RR o VRR");
        abort();
    }
}

void esperar_contexto_y_actualizar_pcb(t_pcb *pcb)
{
    int motivo_desalojo = recibir_operacion(conexion_kernel_cpu_dispatch);
    t_list *paquete = recibir_paquete(conexion_kernel_cpu_dispatch);
    t_contexto *contexto = obtener_contexto_de_paquete_desalojo(paquete);
    actualizar_pcb(pcb, contexto);
    // printf("%d, %d\n", motivo_desalojo, obtener_valor_registro(pcb->registros_cpu, "AX"));
    // pcb_en_EXEC = NULL;

    switch (motivo_desalojo) // ACTUALIZAR EL ESTADO DEL PCB Y TENER EN CUENTA QUE NO LO SACA DE EXEC HASTA QUE TENGA OTRO PARA PASAR A EXEC.
    {
    case DESALOJO_IO:
        t_list *parametros = obtener_parametros_de_paquete_desalojo(paquete);
        // enviar_a_entrada_salida(parametros);

        break;

    case DESALOJO_EXIT_SUCCESS:
        enviar_pcb_a_EXIT(pcb, SUCCESS);
        break;

    case DESALOJO_EXIT_INTERRUPTED:
        enviar_pcb_a_EXIT(pcb, INTERRUPTED_BY_USER);
        break;

    case DESALOJO_FIN_QUANTUM:
        encolar_pcb_segun_algoritmo(pcb, rafaga_cpu_ejecutada);
        break;

    case DESALOJO_WAIT:
        // COMPLETAR: Todavia no esa hecha la funcion en la CPU y por lo tanto no se como me mandan los datos
        wait_recurso("", pcb);
        break;

    case DESALOJO_SIGNAL:
        // COMPLETAR: Todavia no esa hecha la funcion en la CPU y por lo tanto no se como me mandan los datos
        signal_recurso("", pcb, rafaga_cpu_ejecutada);
        break;

    default:
        log_error(logger_propio, "Motivo de desalojo incorrecto.");
        break;
    }
}

void procesar_pcb_segun_algoritmo(t_pcb *pcb)
{
    t_contexto *contexto = crear_contexto(pcb);

    if (strcmp(algoritmo, "FIFO") == 0)
    {
        ejecutar_segun_FIFO(contexto);
    }
    else if (strcmp(algoritmo, "RR") == 0 || strcmp(algoritmo, "VRR") == 0)
    {
        ejecutar_segun_RR_o_VRR(contexto);
    }
    else
    {
        log_error(logger_propio, "Algoritmo invalido. Debe ingresar FIFO, RR o VRR");
        abort();
    }
}

void ejecutar_segun_FIFO(t_contexto *contexto)
{
    enviar_contexto(conexion_kernel_cpu_dispatch, contexto);
}

void ejecutar_segun_RR_o_VRR(t_contexto *contexto)
{
    enviar_contexto(conexion_kernel_cpu_dispatch, contexto);

    usleep(obtener_quantum());
    enviar_interrupcion("FIN_QUANTUM");
    loggear_fin_de_quantum(contexto->PID);
}

void enviar_interrupcion(char *motivo) // considerar meterlo en otro archivo si hay mas funciones sobre las interrupciones
{
    // ACLARACION: El motivo puede ser: "FIN_QUANTUM" o "EXIT"
    t_paquete *paquete = crear_paquete(INTERRUPCION);
    agregar_a_paquete_string(paquete, motivo);
    enviar_paquete(paquete, conexion_kernel_cpu_interrupt);
    eliminar_paquete(paquete);
}

t_contexto *obtener_contexto_de_paquete_desalojo(t_list *paquete)
{
    t_contexto *contexto = iniciar_contexto();

    memcpy(&(contexto->PID), (uint32_t *)list_get(paquete, 0), sizeof(uint32_t));
    dictionary_put(contexto->registros_cpu, "AX", memcpy(malloc(sizeof(uint8_t)), list_get(paquete, 1), sizeof(uint8_t)));
    dictionary_put(contexto->registros_cpu, "EAX", memcpy(malloc(sizeof(uint32_t)), list_get(paquete, 2), sizeof(uint32_t)));
    dictionary_put(contexto->registros_cpu, "BX", memcpy(malloc(sizeof(uint8_t)), list_get(paquete, 3), sizeof(uint8_t)));
    dictionary_put(contexto->registros_cpu, "EBX", memcpy(malloc(sizeof(uint32_t)), list_get(paquete, 4), sizeof(uint32_t)));
    dictionary_put(contexto->registros_cpu, "CX", memcpy(malloc(sizeof(uint8_t)), list_get(paquete, 5), sizeof(uint8_t)));
    dictionary_put(contexto->registros_cpu, "ECX", memcpy(malloc(sizeof(uint32_t)), list_get(paquete, 6), sizeof(uint32_t)));
    dictionary_put(contexto->registros_cpu, "DX", memcpy(malloc(sizeof(uint8_t)), list_get(paquete, 7), sizeof(uint8_t)));
    dictionary_put(contexto->registros_cpu, "EDX", memcpy(malloc(sizeof(uint32_t)), list_get(paquete, 8), sizeof(uint32_t)));
    dictionary_put(contexto->registros_cpu, "PC", memcpy(malloc(sizeof(uint32_t)), list_get(paquete, 9), sizeof(uint32_t)));
    dictionary_put(contexto->registros_cpu, "SI", memcpy(malloc(sizeof(uint32_t)), list_get(paquete, 10), sizeof(uint32_t)));
    dictionary_put(contexto->registros_cpu, "DI", memcpy(malloc(sizeof(uint32_t)), list_get(paquete, 11), sizeof(uint32_t)));

    return contexto;
}

t_list *obtener_parametros_de_paquete_desalojo(t_list *paquete)
{
    t_list *parametros = list_create();

    for (int i = 12; i > list_size(paquete); i++)
    {
        list_add(parametros, list_get(paquete, i));
    }

    list_destroy_and_destroy_elements(paquete, free);

    return parametros;
}