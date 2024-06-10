#include "manejo_interfaces.h"

t_list *interfaces; // falta inicializarlo en algun lado

// NO OLVIDARSE DE LOGGEAR AL MOMENTO QUE UN PROCESO PASA DE EXEC A BLOCKED

void *ejecutar_espera_interfaces(void) // ESTO SE RELACIONA CON LA FUNCION DE CONEXIONES "SERVIDOR()"
{
    // inicializar_interfaces(interfaces);
    while (1)
    {
        int fd_cliente = esperar_cliente(logger_propio, servidor_kernel_fd);
        int op = recibir_operacion(fd_cliente);
        t_list *interfaz = recibir_paquete(fd_cliente);
        agregar_a_lista_io_global((char *)list_get(interfaz, 0), (char *)list_get(interfaz, 1), fd_cliente);
    }
}

void agregar_a_lista_io_global(char *nombre, char *tipo, int fd)
{
    t_io_list *interfaz = malloc(sizeof(t_io_list));
    interfaz->fd = fd;
    strcpy(interfaz->nombre, nombre);
    strcpy(interfaz->tipo, tipo);
    interfaz->procesos_bloqueados = list_create();
    pthread_mutex_init(&(interfaz->cola_bloqueados), NULL);
    sem_init(&(interfaz->procesos_en_cola), 0, 0);
    list_add(interfaces, (void *)interfaz);

    // creo hilo para atender cada interfaz se conecta
    // atender_io(interfaz);
    pthread_t hilo_interfaz;
    if (pthread_create(&hilo_interfaz, NULL, atender_interfaz, (void *)interfaz) != 0)
    {
        perror("Error al crear el hilo");
    }
    pthread_detach(hilo_interfaz);
}

void manejador_interfaz(t_pcb *pcb, t_list *parametros)
{
    char *nombre_interfaz = (char *)list_remove(parametros, 0);

    char *tipo_de_operacion = (char *)list_get(parametros, 0);

    t_io_list *io; // buscar_interfaz(nombre_interfaz); // Verifico que se conecto

    if (io != NULL)
    {
        if (puede_realizar_operacion(io, tipo_de_operacion)) // verifico que puede hacer el tipo de operación
        {

            pthread_mutex_lock(&mutex_lista_BLOCKED);
            // Agrego pcb a bloqueados ;
            list_add(pcbs_en_BLOCKED, (void *)pcb);
            pthread_mutex_unlock(&mutex_lista_BLOCKED);

            // LOGGEO CAMBIO DE ESTADO DE EXEC A BLOCKED;
            loggear_cambio_de_estado(pcb->PID, EXEC, BLOCKED);
            // LOGGEO MOTIVO DE BLOQUEO
            loggear_motivo_de_bloqueo(pcb->PID, nombre_interfaz);

            // Creo la estructura para guardar el pcb y los parametros
            t_proceso_bloqueado *proceso_bloqueado = malloc_or_die(sizeof(t_proceso_bloqueado), "No se pudo asignar memoria a proceso_bloqueado");
            proceso_bloqueado->parametros = parametros;
            proceso_bloqueado->pcb = pcb;

            pthread_mutex_lock(&io->cola_bloqueados);
            // agrego a la lista de bloqueados de la io
            list_add(io->procesos_bloqueados, (void *)proceso_bloqueado);
            pthread_mutex_unlock(&io->cola_bloqueados);

            sem_post(&io->procesos_en_cola);
        }
        else
        {
            enviar_pcb_a_EXIT(pcb, INVALID_INTERFACE);
        }
    }
    enviar_pcb_a_EXIT(pcb, INVALID_INTERFACE);
}

bool puede_realizar_operacion(t_io_list *io, char *operacion)
{
    if (strcmp(io->tipo, "IO_STDOUT") == 0)
    {
        return strcmp(operaciones_stdout[0], operacion) == 0;
    }
    else if (strcmp(io->tipo, "IO_STDIN") == 0)
    {
        return strcmp(operaciones_stdin[0], operacion) == 0;
    }
    else if (strcmp(io->tipo, "IO_GENERIC") == 0)
    {
        return strcmp(operaciones_generic[0], operacion) == 0;
    }
    else if (strcmp(io->tipo, "IO_FS") == 0)
    {
        for (int i = 0; i < 5; i++)
        {
            if (strcmp(operaciones_fs[i], operacion) == 0)
            {
                return true;
            }
        }
        return false;
    }
    else
    {
        return false;
    }
}

void *atender_interfaz(void *interfaz)
{
    t_io_list *io = (t_io_list *)interfaz;

    while (1)
    {
        sem_wait(&io->procesos_en_cola);

        pthread_mutex_lock(&io->cola_bloqueados);
        t_proceso_bloqueado *proceso = list_remove(io->procesos_bloqueados, 0);
        char *op_a_realizar = (char *)list_remove(proceso->parametros, 0);
        int op_interfaz = string_itoa(op_a_realizar);
        t_paquete *p_interfaz = crear_paquete(op_interfaz);
        agregar_a_paquete(p_interfaz, (void *)proceso->pcb->PID, sizeof(int));
        agregar_parametros_a_paquete(p_interfaz, proceso->parametros);
        int estado_al_enviar; // enviar_paquete_interfaz(p_interfaz, io->fd);

        if (estado_al_enviar != -1)
        {
            int respuesta;
            recv(io->fd, &respuesta, sizeof(int), MSG_WAITALL); // recibe resultado de realizar interfaz
            if (respuesta == OK)
            {
                // podria ser redundante tener dos lista de bloqueados
                // ver que hacer

                /*
                pthread_mutex_lock(&mutex_lista_BLOCKED);
                int indice = buscar_indice_pcb(proceso->pcb->PID);
                list_remove(pcbs_en_BLOCKED,indice);
                pthread_mutex_lock(&mutex_lista_BLOCKED);
                */

                // agrego a ready el pcb;

                pthread_mutex_lock(&mutex_cola_READY);
                list_add(pcbs_en_READY, (void *)proceso->pcb);
                pthread_mutex_unlock(&mutex_cola_READY);

                // AGREGO LOG DE CAMBIO DE ESTADO DE BLOCKED A READY
                loggear_cambio_de_estado(proceso->pcb->PID, BLOCKED, READY);
            }
            // se podria hacer algo más si el resultado no es ok ???
            // se deberia considerar
        }
        else
        {
            enviar_pcb_a_EXIT(proceso->pcb, INVALID_INTERFACE);
            eliminar_proceso(proceso);
            liberar_procesos_io(io->procesos_bloqueados);
            eliminar_paquete(p_interfaz);
        }

        eliminar_proceso(proceso);
        eliminar_paquete(p_interfaz);
    }
}

void liberar_procesos_io(t_list *procesos_io)
{
    int size = list_size(procesos_io);
    for (int i = 0; i <= size; i++)
    {
        t_proceso_bloqueado *proceso = list_remove(procesos_io, i);
        // enviar_proceso_a_EXIT(proceso->pcb, INVALID_INTERFACE); con que motivo desalojas al proceso aca???
        eliminar_proceso(proceso);
    }
}

void agregar_parametros_a_paquete(t_paquete *paquete, t_list *parametros)
{
    for (int i = 0; i < list_size(parametros); i++)
    {
        char *param = list_get(parametros, i);
        agregar_a_paquete(paquete, (void *)param, sizeof(param));
    }
}

void eliminar_proceso(t_proceso_bloqueado *proceso)
{
    list_destroy_and_destroy_elements(proceso->parametros, free);
    free(proceso);
}
