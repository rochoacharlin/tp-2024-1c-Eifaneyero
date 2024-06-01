#include "consola.h"

t_comando comandos[] = {
    {"EJECUTAR_SCRIPT", ejecutar_script},
    {"INICIAR_PROCESO", iniciar_proceso},
    {"FINALIZAR_PROCESO", finalizar_proceso},
    {"DETENER_PLANIFICACION", detener_planificacion},
    {"INICIAR_PLANIFICACION", iniciar_planificacion},
    {"MULTIPROGRAMACION", cambiar_grado_multiprogramacion},
    {"PROCESO_ESTADO", listar_procesos_por_cada_estado},
};

void consola_interactiva(void)
{
    char *leido = readline("> ");
    char *token = strtok(leido, " ");

    while (1)
    {
        buscar_y_ejecutar_comando(token);
        free(leido);

        sleep(1);
        leido = readline("> ");
        token = strtok(leido, " ");
    }

    free(leido);
    destruir_semaforos_planificacion();
    destruir_listas_planificacion();
}

void listar_procesos_por_estado(char *estado, t_list *lista)
{
    printf("%s: ", estado);
    for (int i = 0; i < list_size(lista); i++)
    {
        t_pcb *pcb = (t_pcb *)list_get(lista, i);
        printf(" %d ", pcb->PID);
    }
    printf("\n");
}

void buscar_y_ejecutar_comando(char *token)
{
    if (token != NULL)
    {
        int i;
        for (i = 0; i < sizeof(comandos) / sizeof(t_comando); i++)
        {
            if (strcmp(token, comandos[i].nombre) == 0)
            {
                token = strtok(NULL, " ");
                comandos[i].funcion_de_comando(token);
                break;
            }
        }

        if (i == sizeof(comandos) / sizeof(t_comando))
        {
            log_error(logger_propio, "Comando invalido.");
        }
    }
}

void cambiar_valor_de_semaforo(sem_t *sem, int valor_resultante)
{
    if (valor_resultante < 0)
        log_error(logger_propio, "Grado de multiprogramacion no valido");
    else
    {
        int valor_actual;
        // Obtener el valor actual del semáforo
        sem_getvalue(sem, &valor_actual);

        // Realizar operaciones para ajustar el semáforo al valor deseado
        while (valor_actual > valor_resultante)
        {
            sem_wait(sem);
            valor_actual--;
        }
        while (valor_actual < valor_resultante)
        {
            sem_post(sem);
            valor_actual++;
        }
    }
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
        pthread_mutex_lock(&mutex_lista_READY);
        list_remove_element(pcbs_en_READY, pcb);
        pthread_mutex_unlock(&mutex_lista_READY);
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
}