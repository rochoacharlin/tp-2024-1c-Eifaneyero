#include "planificacion_largo_plazo.h"

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
sem_t planificacion_liberada;
sem_t planificacion_pausada;
sem_t termina_comando;

pthread_mutex_t mutex_lista_NEW;
pthread_mutex_t mutex_lista_READY;
pthread_mutex_t mutex_lista_BLOCKED;
pthread_mutex_t mutex_pcb_EXEC;
pthread_mutex_t mutex_lista_memoria;
pthread_mutex_t mutex_lista_EXIT;

int32_t procesos_creados = 0;

void planificar_a_largo_plazo(void)
{
    while (1)
    {
        sem_wait(&planificacion_liberada);
        sem_wait(&hay_pcbs_NEW);
        sem_wait(&sem_grado_multiprogramacion);

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

void inicializar_listas_planificacion(void)
{
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
    pthread_mutex_init(&mutex_lista_READY, NULL);
    pthread_mutex_init(&mutex_lista_BLOCKED, NULL);
    pthread_mutex_init(&mutex_pcb_EXEC, NULL);
    pthread_mutex_init(&mutex_lista_memoria, NULL);
    pthread_mutex_init(&mutex_lista_EXIT, NULL);
    sem_init(&hay_pcbs_NEW, 0, 0);
    sem_init(&hay_pcbs_READY, 0, 0);
    sem_init(&sem_grado_multiprogramacion, 0, obtener_grado_multiprogramacion());
    sem_init(&planificacion_liberada, 0, 1);
    sem_init(&termina_comando, 0, 0);
}
void destruir_semaforos_planificacion(void)
{
    pthread_mutex_destroy(&mutex_lista_NEW);
    pthread_mutex_destroy(&mutex_lista_READY);
    pthread_mutex_destroy(&mutex_lista_BLOCKED);
    pthread_mutex_destroy(&mutex_pcb_EXEC);
    pthread_mutex_destroy(&mutex_lista_memoria);
    pthread_mutex_destroy(&mutex_lista_EXIT);
    sem_close(&hay_pcbs_NEW);
    sem_close(&hay_pcbs_READY);
    sem_close(&sem_grado_multiprogramacion);
}