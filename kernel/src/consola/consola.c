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
        sleep(1); // es una mala practica? se puede hacer en este caso?
        leido = readline("> ");
        token = strtok(leido, " ");
    }

    free(leido);
    destruir_semaforos_planificacion();
    destruir_listas_planificacion();
}

void ejecutar_script(char *path)
{
    FILE *archivo = fopen(path, "r");
    if (archivo == NULL)
    {
        log_error(logger_propio, "Error al abrir el archivo.");
        return;
    }

    char comando[256];

    while (fgets(comando, sizeof(comando), archivo) != NULL)
    {
        // Eliminar el carácter de nueva línea si está presente
        size_t longitud = strlen(comando);
        if (longitud > 0 && comando[longitud - 1] == '\n')
        {
            comando[longitud - 1] = '\0';
        }

        // Ejecutar el comando
        buscar_y_ejecutar_comando(comando);
    }

    fclose(archivo);
}

void iniciar_proceso(char *path)
{
    // COMPLETAR: Falta toda la parte de la memoria y filesystem.
    t_pcb *pcb = crear_pcb();
    ingresar_pcb_a_NEW(pcb);
}

void finalizar_proceso(char *PID)
{
    // TODO: Liberar recursos, archivos y memoria
    t_pcb *pcb = buscar_pcb_por_PID(pcbs_en_memoria, PID);
    if (pcb == NULL)
        log_error(logger_propio, "No existe un PCB con ese PID.");
    else
    {
        list_remove_element(pcbs_en_memoria, pcb);
        pcb->estado = EXIT;
        list_add(pcbs_en_EXIT, pcb);
    }
}

void detener_planificacion(void)
{
    // COMPLETAR: El proceso que se encuentra en ejecución NO es desalojado,
    //            pero una vez que salga de EXEC se va a pausar el manejo de su motivo de desalojo.
    sem_wait(&planificacion_liberada);
    sem_post(&planificacion_pausada);
}

void iniciar_planificacion(void)
{
    sem_wait(&planificacion_pausada); // para asegurar que libere la planificacion cuando este pausada.
    sem_post(&planificacion_liberada);
}

void cambiar_grado_multiprogramacion(char *valor)
{
    // TODO: Cambiar el nivel de multiprogramacion
    // Lo cambiamos directamente en el archivo de configuracion o en una variable del codigo?
}

void listar_procesos_por_cada_estado(void)
{
    listar_procesos_por_estado("NEW", pcbs_en_NEW);
    listar_procesos_por_estado("READY", pcbs_en_READY);
    listar_procesos_por_estado("EXEC", pcbs_en_EXEC);
    listar_procesos_por_estado("BLOCKED", pcbs_en_BLOCKED);
    listar_procesos_por_estado("EXIT", pcbs_en_EXIT);
}

void listar_procesos_por_estado(char *estado, t_list *lista)
{
    printf("%s: \n", estado);
    for (int i = 0; i < list_size(lista); i++)
    {
        t_pcb *pcb = (t_pcb *)list_get(lista, i);
        printf("    PID: %d\n", pcb->PID);
    }
}

void buscar_y_ejecutar_comando(char *token)
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
        log_error(logger_propio, "Comando invalido.");
}