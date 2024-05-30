#include "planificacion.h"

t_list *pcbs_en_EXIT;
t_list *pcbs_en_READY;
t_list *pcbs_en_NEW;
t_list *pcbs_en_memoria;
t_list *pcbs_en_EXEC;
t_list *pcbs_en_BLOCKED;

sem_t hay_pcbs_NEW;
sem_t hay_pcbs_READY;
sem_t sem_grado_multiprogramacion;
pthread_mutex_t mutex_lista_NEW;
pthread_mutex_t mutex_lista_READY;
sem_t planificacion_liberada;
sem_t planificacion_pausada;
int32_t procesos_creados = 0;
pthread_t hilo_Q;

void planificar_a_largo_plazo(void)
{
    while (1)
    {
        sem_wait(&hay_pcbs_NEW);
        sem_wait(&sem_grado_multiprogramacion);
        sem_wait(&planificacion_liberada);

        t_pcb *pcb = obtener_siguiente_pcb_READY();

        // recibir_estructuras_iniciales_memoria(pcb);

        estado anterior = pcb->estado;
        pcb->estado = READY;
        list_add(pcbs_en_memoria, pcb);

        // log minimo y obligatorio
        loggear_cambio_de_estado(pcb->PID, anterior, pcb->estado);

        sem_post(&planificacion_liberada);
        ingresar_pcb_a_READY(pcb);
    }
}

t_pcb *obtener_siguiente_pcb_READY(void)
{
    pthread_mutex_lock(&mutex_lista_NEW);
    t_pcb *pcb = desencolar_pcb(pcbs_en_NEW);
    pthread_mutex_unlock(&mutex_lista_NEW);
    return pcb;
}

void ingresar_pcb_a_READY(t_pcb *pcb)
{
    pthread_mutex_lock(&mutex_lista_READY);
    encolar_pcb(pcbs_en_READY, pcb);
    pthread_mutex_unlock(&mutex_lista_READY);

    sem_post(&hay_pcbs_READY);

    // log minimo y obligatorio
    lista_PIDS = string_new();
    mostrar_PIDS(pcbs_en_READY);
    loggear_ingreso_a_READY(lista_PIDS);
    free(lista_PIDS);
}

void ingresar_pcb_a_NEW(t_pcb *pcb)
{
    pthread_mutex_lock(&mutex_lista_NEW);
    encolar_pcb(pcbs_en_NEW, pcb);
    pthread_mutex_unlock(&mutex_lista_NEW);

    // log minimo y obligatorio
    loggear_creacion_proceso(pcb->PID);
    sem_post(&hay_pcbs_NEW);
}

void planificar_a_corto_plazo_segun_algoritmo(void)
{
    char *algoritmo = obtener_algoritmo_planificacion();

    if (strcmp(algoritmo, "FIFO") == 0 || strcmp(algoritmo, "RR") == 0)
    {
        planificar_a_corto_plazo(proximo_a_ejecutar_segun_FIFO_o_RR);
    }
    else if (strcmp(algoritmo, "VRR") == 0)
    {
        // TODO: Solucion VRR
        // se debe fija la lista_ready+
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
        t_pcb *pcb_proximo = proximo_a_ejecutar();

        estado anterior = pcb_proximo->estado;
        pcb_proximo->estado = EXEC;
        list_add(pcbs_en_EXEC, pcb_proximo);

        // log minimo y obligatorio
        loggear_cambio_de_estado(pcb_proximo->PID, anterior, pcb_proximo->estado);

        t_contexto *contexto = procesar_pcb_segun_algoritmo(pcb_proximo);
        // int rafaga_CPU = contexto->rafaga_CPU_ejecutada;
        list_remove_element(pcbs_en_EXEC, pcb_proximo);
        list_add(pcbs_en_BLOCKED, pcb_proximo); // provisional
        // retorno_contexto(pcb_proximo, contexto);
    }
}

t_pcb *proximo_a_ejecutar_segun_FIFO_o_RR(void)
{
    return desencolar_pcb(pcbs_en_READY);
}

t_pcb *proximo_a_ejecutar_segun_VRR(void)
{
    // TODO
}

void inicializar_listas_planificacion(void)
{
    pcbs_en_NEW = list_create();
    pcbs_en_READY = list_create();
    pcbs_en_memoria = list_create();
    pcbs_en_EXEC = list_create();
    pcbs_en_BLOCKED = list_create();
    pcbs_en_EXIT = list_create();
}

void destruir_listas_planificacion(void)
{
    list_destroy_and_destroy_elements(pcbs_en_NEW, (void *)destruir_pcb);
    list_destroy_and_destroy_elements(pcbs_en_READY, (void *)destruir_pcb);
    list_destroy_and_destroy_elements(pcbs_en_memoria, (void *)destruir_pcb);
    list_destroy_and_destroy_elements(pcbs_en_EXEC, (void *)destruir_pcb);
    list_destroy_and_destroy_elements(pcbs_en_BLOCKED, (void *)destruir_pcb);
    list_destroy_and_destroy_elements(pcbs_en_EXIT, (void *)destruir_pcb);
}

void inicializar_semaforos_planificacion(void)
{
    pthread_mutex_init(&mutex_lista_NEW, NULL);
    pthread_mutex_init(&mutex_lista_READY, NULL);
    sem_init(&hay_pcbs_NEW, 0, 0);
    sem_init(&hay_pcbs_READY, 0, 0);
    sem_init(&sem_grado_multiprogramacion, 0, obtener_grado_multiprogramacion());
    sem_init(&planificacion_liberada, 0, 1);
}
void destruir_semaforos_planificacion(void)
{
    pthread_mutex_destroy(&mutex_lista_NEW);
    pthread_mutex_destroy(&mutex_lista_READY);
    sem_close(&hay_pcbs_NEW);
    sem_close(&hay_pcbs_READY);
    sem_close(&sem_grado_multiprogramacion);
}

// REVISAR
t_contexto *procesar_pcb_segun_algoritmo(t_pcb *pcb)
{
    char *algoritmo = obtener_algoritmo_planificacion();

    if (strcmp(algoritmo, "FIFO") == 0)
    {
        return ejecutar_segun_FIFO(pcb);
    }
    else if (strcmp(algoritmo, "RR") == 0)
    {
        return ejecutar_segun_RR(pcb);
    }
    else if (strcmp(algoritmo, "VRR") == 0)
    {
        return ejecutar_segun_VRR(pcb);
    }
    else
    {
        log_error(logger_propio, "Algoritmo invalido. Debe ingresar FIFO, RR o VRR");
        abort();
    }
}

t_contexto *ejecutar_segun_FIFO(t_pcb *pcb)
{
    // TODO
    // contexto = esperar_contexto()
}

t_contexto *ejecutar_segun_VRR(t_pcb *pcb)
{
    // TODO
}

// Implementacion a probar
t_contexto *ejecutar_segun_RR(t_pcb *pcb)
{
    // falta saber si se hara con un contexto
    enviar_contexto_a_cpu(pcb->contexto, conexion_kernel_cpu_dispatch);
    int quantum = obtener_quantum();

    pthread_create(&hilo_Q, NULL, ejecutar_quantum, (*void)&pcb);
    pthread_detach(hilo_Q);
    // t_contexto *contexto;
    // contexto = esperar_contexto(pcb);
    pthread_cancel(hilo_Q);

    return contexto;
}

void *ejecutar_quantum(void *pcb)
{
    t_pcb *pcb_q = (t_pcb *)pcb;

    usleep(pcb_q->quantum);
    enviar_interrupcion_FIN_Q(pcb->PID, conexion_kernel_cpu_interrupt);
    loggear_fin_de_quantum(pcb->PID);
}

void enviar_interrupcion_FIN_Q(int PID, int fd)
{
    t_paquete *paquete = crear_paquete_interrupcion(PID);
    enviar_paquete(paquete, fd);
    eliminar_paquete(paquete);
}

t_paquete *crear_paquete_interrupcion(int PID)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = INTERRUPCION;
    // en aqui queres que te mande planificacion ???
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = sizeof(int);
    paquete->buffer->stream = malloc(buffer->size);

    int offset = 0;

    memcpy(paquete->buffer->stream + offset, &(interrupcion->pid), sizeof(int));

    return paquete;
}
