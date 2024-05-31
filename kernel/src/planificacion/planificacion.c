#include "planificacion.h"

t_list *pcbs_en_EXIT;
t_list *pcbs_en_READY;
t_list *pcbs_en_aux_READY;
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

char *algoritmo = obtener_algoritmo_planificacion();

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
        t_pcb *pcb_proximo = proximo_a_ejecutar();

        estado anterior = pcb_proximo->estado;
        pcb_proximo->estado = EXEC;
        list_add(pcbs_en_EXEC, pcb_proximo);

        // log minimo y obligatorio
        loggear_cambio_de_estado(pcb_proximo->PID, anterior, pcb_proximo->estado);

        t_contexto *contexto = procesar_pcb_segun_algoritmo(pcb_proximo);
        esperar_contexto_y_actualizarlo(pcb_proximo);
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
t_contexto *esperar_contexto_y_actualizar_pcb(t_pcb *pcb, t_contexto *contexto)
{
    int motivo_desalojo = recibir_operacion(conexion_kernel_cpu_dispatch);
    t_list *paquete = recibir_paquete(conexion_kernel_cpu_dispatch);
    t_contexto *contexto = list_get(paquete, 0);
    actualizar_pcb(pcb, contexto);

    switch (motivo_desalojo)
    {
    case IO_GEN_SLEEP:
    case EXIT:
    case FIN_QUANTUM:
        // PARA QUE NECESITO LOS PARAMETROS??
        break;

    default:
        log_error(logger_propio, "Motivo de desalojo incorrecto.");
        break;
    }
}

void inicializar_listas_planificacion(void)
{
    pcbs_en_NEW = list_create();
    pcbs_en_READY = list_create();

    if (strcmp(algoritmo, "VRR") == 0)
        pcbs_en_aux_READY = list_create();

    pcbs_en_memoria = list_create();
    pcbs_en_EXEC = list_create();
    pcbs_en_BLOCKED = list_create();
    pcbs_en_EXIT = list_create();
}

void destruir_listas_planificacion(void)
{
    list_destroy_and_destroy_elements(pcbs_en_NEW, (void *)destruir_pcb);
    list_destroy_and_destroy_elements(pcbs_en_READY, (void *)destruir_pcb);

    if (strcmp(algoritmo, "VRR") == 0)
        list_destroy_and_destroy_elements(pcbs_en_aux_READY, (void *)destruir_pcb);

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

t_contexto *procesar_pcb_segun_algoritmo(t_pcb *pcb)
{
    t_contexto *contexto = asignar_valores_pcb_a_contexto(pcb);

    if (strcmp(algoritmo, "FIFO") == 0)
    {
        return ejecutar_segun_FIFO(contexto);
    }
    else if (strcmp(algoritmo, "RR") == 0 || strcmp(algoritmo, "VRR") == 0)
    {
        return ejecutar_segun_RR_o_VRR(contexto);
    }
    else
    {
        log_error(logger_propio, "Algoritmo invalido. Debe ingresar FIFO, RR o VRR");
        abort();
    }
}

t_contexto *ejecutar_segun_FIFO(t_contexto *contexto)
{
    enviar_contexto(conexion_kernel_cpu_dispatch, contexto);
}

t_contexto *ejecutar_segun_RR_o_VRR(t_contexto *contexto)
{
    enviar_contexto(conexion_kernel_cpu_dispatch, contexto);

    usleep(obtener_quantum());
    enviar_interrupcion_FIN_Q(contexto->PID, conexion_kernel_cpu_interrupt);
    loggear_fin_de_quantum(contexto->PID);

    return contexto;
}

void enviar_interrupcion_FIN_Q(int PID, int fd_servidor_cpu)
{
    t_paquete *paquete = crear_paquete_interrupcion(PID);
    enviar_paquete(paquete, fd_servidor_cpu);
    eliminar_paquete(paquete);
}

t_paquete *crear_paquete_interrupcion(int PID) // considerar sacarlo y reubicarlo en otro lado
{
    /* version anterior sin usar las funciones ya creadas

        t_paquete *paquete = malloc(sizeof(t_paquete));
        paquete->codigo_operacion = INTERRUPCION;
        // en aqui queres que te mande planificacion ???
        paquete->buffer = malloc(sizeof(t_buffer));
        paquete->buffer->size = sizeof(int);
        paquete->buffer->stream = malloc(buffer->size);

        // estan seguros que este paquete esta bien creado?
        memcpy(paquete->buffer->stream, &PID, sizeof(int));
    */

    t_paquete *paquete = crear_paquete(INTERRUPCION);
    agregar_a_paquete(paquete, &PID, sizeof(int));

    return paquete;
}

typedef enum
{
    IO_GEN_SLEEP,
    EXIT,
    FIN_QUANTUM
} motivos_desalojo;