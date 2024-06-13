#include "consola.h"

void ejecutar_script(char *path)
{
    FILE *archivo;

    if (path != NULL)
    {
        archivo = fopen(path, "r");
        if (archivo == NULL)
        {
            log_error(logger_propio, "Error al abrir el archivo.");
            return;
        }

        char comando[256];
        while (!feof(archivo))
        {
            fgets(comando, sizeof(comando), archivo);
            char *token = strtok(comando, " ");

            // Eliminar el carácter de nueva línea si está presente
            size_t longitud = strlen(comando);
            if (longitud > 0 && comando[longitud - 1] == '\n')
                comando[longitud - 1] = '\0';

            buscar_y_ejecutar_comando(token);
        }

        fclose(archivo);
    }
    else
    {
        log_error(logger_propio, "No se ingreso un path para iniciar el proceso.");
    }
}

void iniciar_proceso(char *path)
{
    // COMPLETAR: falta tener en cuenta el filesystem.
    if (path != NULL)
    {
        t_pcb *pcb = crear_pcb();

        t_paquete *paquete = crear_paquete(CREAR_PROCESO_KERNEL);
        agregar_a_paquete_uint32(paquete, pcb->PID);
        agregar_a_paquete_string(paquete, path);
        enviar_paquete(paquete, conexion_kernel_memoria);
        eliminar_paquete(paquete);

        if (recibir_operacion(conexion_kernel_memoria) == OK)
        {
            log_info(logger_propio, "Creacion de proceso exitosa en memoria.");
            ingresar_pcb_a_NEW(pcb);
        }
        else
            log_info(logger_propio, "Creacion de proceso fallida en memoria.");
    }
    else
    {
        log_error(logger_propio, "No se ingreso un path para iniciar el proceso.");
    }
}

void finalizar_proceso(char *PID)
{
    // COMPLETAR: Liberar archivos
    t_pcb *pcb = NULL;

    if (PID != NULL)
    {
        pcb = buscar_pcb_por_PID(pcbs_en_memoria, (uint32_t)atoi(PID));

        if (pcb == NULL)
            log_error(logger_propio, "No existe un PCB con ese PID.");
        else if (pcb->estado == EXEC)
            enviar_interrupcion("EXIT");
        else
            enviar_pcb_a_EXIT(pcb, INTERRUPTED_BY_USER);
    }
    else
    {
        log_error(logger_propio, "No se ingreso un PID para finalizar el proceso.");
    }
}

void detener_planificacion(void)
{
    sem_wait(&planificacion_largo_plazo_liberada);
    sem_wait(&planificacion_corto_plazo_liberada);
    sem_wait(&desalojo_liberado);
    sem_post(&planificacion_pausada);
}

void iniciar_planificacion(void)
{
    sem_wait(&planificacion_pausada); // para asegurar que solo se libere la planificacion cuando este pausada.
    sem_post(&planificacion_largo_plazo_liberada);
    sem_post(&planificacion_corto_plazo_liberada);
    sem_post(&desalojo_liberado);
}

void cambiar_grado_multiprogramacion(char *valor_deseado)
{
    if (valor_deseado == NULL)
        log_error(logger_propio, "No se indico el numero de grado de multiprogramacion");
    else
    {
        int valor_resultante, valor_actual, valor_inicial;
        valor_inicial = obtener_grado_multiprogramacion();
        sem_getvalue(&sem_grado_multiprogramacion, &valor_actual);

        valor_resultante = atoi(valor_deseado) - valor_inicial + valor_actual;
        cambiar_valor_de_semaforo(&sem_grado_multiprogramacion, valor_resultante);
        config_set_value(config, "GRADO_MULTIPROGRAMACION", valor_deseado);

        sem_getvalue(&sem_grado_multiprogramacion, &valor_actual);
        log_info(logger_propio, "Grado multiprogramacion actual: %d", valor_actual);

        // VERIFICAR: En caso de que se tengan más procesos ejecutando que lo que permite el grado de
        //       multiprogramación, no se tomarán acciones sobre los mismos y se esperará su finalización normal.
    }
}

void listar_procesos_por_cada_estado(void)
{
    listar_procesos_por_estado("NEW", pcbs_en_NEW);
    listar_procesos_por_estado("READY", pcbs_en_READY);
    printf("EXEC: %s\n", pcb_en_EXEC != NULL ? string_itoa(pcb_en_EXEC->PID) : " ");
    listar_procesos_por_estado("BLOCKED", pcbs_en_BLOCKED);
    listar_procesos_por_estado("EXIT", pcbs_en_EXIT);
}