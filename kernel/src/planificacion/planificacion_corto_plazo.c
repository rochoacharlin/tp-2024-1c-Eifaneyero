#include "planificacion.h"

void planificar_a_corto_plazo_segun_algoritmo(void)
{
    char *algoritmo = obtener_algoritmo_planificacion();

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

// A LA ESPERA DE QUE ROCIO ME DIGA COMO OBTENER EL CONTEXTO QUE ELLA ME DEVUELVE
t_contexto *esperar_contexto_y_actualizar_pcb(t_pcb *pcb)
{
    int motivo_desalojo = recibir_operacion(conexion_kernel_cpu_dispatch);
    t_list *paquete = recibir_paquete(conexion_kernel_cpu_dispatch);
    t_contexto *contexto = (t_contexto *)list_get(paquete, 0);
    actualizar_pcb(pcb, contexto); // ACTUALIZAR EL ESTADO DEL PCB Y TENER EN CUENTA QUE NO LO SACA DE EXEC HASTA QUE TENGA OTRO PARA PASAR A EXEC.

    switch (motivo_desalojo)
    {
    case DESALOJO_IO_GEN_SLEEP:
        // Parametros: nombre la interfaz y cantidad de tiempo que se va sleep.
        break;
    case DESALOJO_EXIT:
        enviar_pcb_a_EXIT(pcb);
        break;
    case DESALOJO_FIN_QUANTUM:
        list_add(pcbs_en_READY, pcb);

        break;

    case DESALOJO_WAIT:
        wait_recurso(list_get(paquete, 1), pcb);
        break;
    case DESALOJO_SIGNAL:
        signal_recurso(list_get(paquete, 1), pcb);
        break;

    default:
        log_error(logger_propio, "Motivo de desalojo incorrecto.");
        break;
    }

    return NULL; // creo que hay que retornar un pcb actualizado no un contexto
}

void procesar_pcb_segun_algoritmo(t_pcb *pcb)
{
    char *algoritmo = obtener_algoritmo_planificacion();
    t_contexto *contexto = asignar_valores_pcb_a_contexto(pcb);

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
    enviar_interrupcion_FIN_Q(contexto->PID, conexion_kernel_cpu_interrupt);
    loggear_fin_de_quantum(contexto->PID);
}

void enviar_interrupcion(char *motivo)
{
    t_paquete *paquete = crear_paquete(INTERRUPCION);
    agregar_a_paquete_string(paquete, &motivo);
    enviar_paquete(paquete, conexion_kernel_cpu_interrupt);
    eliminar_paquete(paquete);
}