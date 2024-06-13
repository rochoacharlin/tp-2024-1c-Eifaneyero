#include "planificacion.h"

t_list *pcbs_en_EXIT;
t_list *pcbs_en_READY;
t_list *pcbs_en_aux_READY;
t_list *pcbs_en_NEW;
t_list *pcbs_en_memoria;
t_pcb *pcb_en_EXEC;
t_list *pcbs_en_BLOCKED;

sem_t hay_pcbs_NEW;
sem_t hay_pcbs_READY;
sem_t sem_grado_multiprogramacion;
sem_t planificacion_largo_plazo_liberada;
sem_t planificacion_corto_plazo_liberada;
sem_t desalojo_liberado;
sem_t planificacion_pausada;

pthread_mutex_t mutex_lista_NEW;
pthread_mutex_t mutex_cola_READY;
pthread_mutex_t mutex_cola_aux_READY;
pthread_mutex_t mutex_lista_BLOCKED;
pthread_mutex_t mutex_pcb_EXEC;
pthread_mutex_t mutex_lista_memoria;
pthread_mutex_t mutex_lista_EXIT;

int32_t procesos_creados = 0;
char *algoritmo;

void planificar_a_largo_plazo(void)
{
    while (1)
    {
        sem_wait(&planificacion_largo_plazo_liberada);
        sem_wait(&hay_pcbs_NEW);
        sem_wait(&sem_grado_multiprogramacion);

        t_pcb *pcb = obtener_siguiente_pcb_READY();

        // VERIFICAR: Cuando se crea un proceso en NEW se carga completamente en memoria
        //            o hay que esperar a que pase a READY para que se creen todas
        //            sus estructuras?

        estado anterior = pcb->estado;
        pcb->estado = READY;
        list_add(pcbs_en_memoria, pcb);

        // log minimo y obligatorio
        loggear_cambio_de_estado(pcb->PID, anterior, pcb->estado);

        ingresar_pcb_a_READY(pcb);
        sem_post(&planificacion_largo_plazo_liberada);
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
    pthread_mutex_lock(&mutex_cola_READY);
    encolar_pcb(pcbs_en_READY, pcb);
    pthread_mutex_unlock(&mutex_cola_READY);

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

void inicializar_listas_planificacion(void)
{
    algoritmo = obtener_algoritmo_planificacion();

    pcbs_en_NEW = list_create();
    pcbs_en_READY = list_create();

    if (strcmp(obtener_algoritmo_planificacion(), "VRR") == 0)
        pcbs_en_aux_READY = list_create();

    pcbs_en_memoria = list_create();
    pcbs_en_BLOCKED = list_create();
    pcbs_en_EXIT = list_create();
}

void destruir_listas_planificacion(void)
{
    list_destroy_and_destroy_elements(pcbs_en_NEW, (void *)destruir_pcb);
    list_destroy_and_destroy_elements(pcbs_en_READY, (void *)destruir_pcb);

    if (strcmp(obtener_algoritmo_planificacion(), "VRR") == 0)
        list_destroy_and_destroy_elements(pcbs_en_aux_READY, (void *)destruir_pcb);

    list_destroy_and_destroy_elements(pcbs_en_memoria, (void *)destruir_pcb);
    destruir_pcb(pcb_en_EXEC);
    list_destroy_and_destroy_elements(pcbs_en_BLOCKED, (void *)destruir_pcb);
    list_destroy_and_destroy_elements(pcbs_en_EXIT, (void *)destruir_pcb);
}

void inicializar_semaforos_planificacion(void)
{
    pthread_mutex_init(&mutex_lista_NEW, NULL);
    pthread_mutex_init(&mutex_cola_READY, NULL);
    pthread_mutex_init(&mutex_cola_aux_READY, NULL);
    pthread_mutex_init(&mutex_lista_BLOCKED, NULL);
    pthread_mutex_init(&mutex_pcb_EXEC, NULL);
    pthread_mutex_init(&mutex_lista_memoria, NULL);
    pthread_mutex_init(&mutex_lista_EXIT, NULL);
    sem_init(&hay_pcbs_NEW, 0, 0);
    sem_init(&hay_pcbs_READY, 0, 0);
    sem_init(&sem_grado_multiprogramacion, 0, obtener_grado_multiprogramacion());
    sem_init(&planificacion_largo_plazo_liberada, 0, 1);
    sem_init(&planificacion_corto_plazo_liberada, 0, 1);
    sem_init(&desalojo_liberado, 0, 1);
}

void destruir_semaforos_planificacion(void)
{
    pthread_mutex_destroy(&mutex_lista_NEW);
    pthread_mutex_destroy(&mutex_cola_READY);
    pthread_mutex_destroy(&mutex_cola_aux_READY);
    pthread_mutex_destroy(&mutex_lista_BLOCKED);
    pthread_mutex_destroy(&mutex_pcb_EXEC);
    pthread_mutex_destroy(&mutex_lista_memoria);
    pthread_mutex_destroy(&mutex_lista_EXIT);
    sem_close(&hay_pcbs_NEW);
    sem_close(&hay_pcbs_READY);
    sem_close(&sem_grado_multiprogramacion);
    sem_close(&planificacion_largo_plazo_liberada);
    sem_close(&planificacion_corto_plazo_liberada);
    sem_close(&desalojo_liberado);
}

void enviar_pcb_a_EXIT(t_pcb *pcb, int motivo)
{
    remover_pcb_de_listas_globales(pcb);
    pcb->estado = EXIT;

    sem_post(&sem_grado_multiprogramacion);

    pthread_mutex_lock(&mutex_lista_EXIT);
    list_add(pcbs_en_EXIT, pcb);
    pthread_mutex_unlock(&mutex_lista_EXIT);

    // log minimo y obligatorio
    loggear_fin_de_proceso(pcb->PID, motivo);

    liberar_recursos(pcb);

    // libero memoria
    t_paquete *paquete = crear_paquete(FINALIZAR_PROCESO_KERNEL);
    agregar_a_paquete_uint32(paquete, pcb->PID);
    enviar_paquete(paquete, conexion_kernel_memoria);
    eliminar_paquete(paquete);

    if (recibir_operacion(conexion_kernel_memoria) == OK)
        log_info(logger_propio, "Finalizacion de proceso exitosa en memoria.");

    else
        log_info(logger_propio, "Finalizacion de proceso fallida en memoria.");
}

void remover_pcb_de_listas_globales(t_pcb *pcb)
{
    pthread_mutex_lock(&mutex_lista_memoria);
    list_remove_element(pcbs_en_memoria, pcb);
    pthread_mutex_unlock(&mutex_lista_memoria);

    switch (pcb->estado)
    {
    case NEW:
        pthread_mutex_lock(&mutex_lista_NEW);
        list_remove_element(pcbs_en_NEW, pcb);
        pthread_mutex_unlock(&mutex_lista_NEW);
        break;

    case READY:
        pthread_mutex_lock(&mutex_cola_READY);
        list_remove_element(pcbs_en_READY, pcb);
        pthread_mutex_unlock(&mutex_cola_READY);

        if (strcmp("VRR", algoritmo) == 0)
        {
            pthread_mutex_lock(&mutex_cola_aux_READY);
            list_remove_element(pcbs_en_aux_READY, pcb);
            pthread_mutex_unlock(&mutex_cola_aux_READY);
        }
        break;

    case EXEC:
        pthread_mutex_lock(&mutex_pcb_EXEC);
        pcb_en_EXEC = NULL;
        pthread_mutex_unlock(&mutex_pcb_EXEC);
        break;

    case BLOCKED:
        pthread_mutex_lock(&mutex_lista_BLOCKED);
        list_remove_element(pcbs_en_BLOCKED, pcb);
        pthread_mutex_unlock(&mutex_lista_BLOCKED);
        break;

    default:
        log_error(logger_propio, "Error al remover un pcb de una lista global.");
        break;
    }

    // elimino el pcb de las colas de recursos
    for (int i = 0; i < list_size(colas_de_recursos); i++)
    {
        t_list *cola = list_get(colas_de_recursos, i);
        list_remove_element(cola, pcb);
    }
}