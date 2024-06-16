#include "manejo_interfaces.h"

t_list *interfaces; // falta inicializarlo en algun lado
pthread_mutex_t mutex_interfaces;
char *operaciones_stdout[] = {"IO_STDOUT_WRITE"};
char *operaciones_stdin[] = {"IO_STDIN_READ"};
char *operaciones_generic[] = {"IO_GENERIC_SLEEP"};
char *operaciones_fs[] = {"IO_FS_CREATE", "IO_FS_DELETE", "IO_FS_TRUNCATE", "IO_FS_WRITE", "IO_FS_READ"};

// NO OLVIDARSE DE LOGGEAR AL MOMENTO QUE UN PROCESO PASA DE EXEC A BLOCKED

void *ejecutar_espera_interfaces(void) // ESTO SE RELACIONA CON LA FUNCION DE CONEXIONES "SERVIDOR()"
{
    inicializar_interfaces();
    while (1)
    {
        int fd_cliente = esperar_cliente(logger_propio, servidor_kernel_fd);
        int op = recibir_operacion(fd_cliente);
        t_list *interfaz = recibir_paquete(fd_cliente);
        agregar_a_lista_io_global((char *)list_get(interfaz, 0), (char *)list_get(interfaz, 1), fd_cliente);
    }
}

void inicializar_interfaces()
{
    interfaces = list_create();
    pthread_mutex_init(&mutex_interfaces, NULL);
}

void agregar_a_lista_io_global(char *nombre, char *tipo, int fd)
{
    t_io_list *interfaz = malloc_or_die(sizeof(t_io_list), "No se pudo reservar memeoria para interfaz");
    interfaz->fd = fd;
    interfaz->nombre = malloc(strlen(nombre) + 1);
    interfaz->tipo = malloc(strlen(tipo) + 1);
    strcpy(interfaz->nombre, nombre);
    strcpy(interfaz->tipo, tipo);
    interfaz->procesos_bloqueados = list_create();
    pthread_mutex_init(&(interfaz->cola_bloqueados), NULL);
    sem_init(&(interfaz->procesos_en_cola), 0, 0);

    pthread_mutex_lock(&mutex_interfaces);
    list_add(interfaces, (void *)interfaz);
    pthread_mutex_lock(&mutex_interfaces);

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

    t_io_list *io = buscar_interfaz(nombre_interfaz); // Verifico que se conecto

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

t_io_list *buscar_interfaz(char *nombre_io)
{
    t_io_list *io = NULL;
    int tamanio = list_size(interfaces);
    for (int i = 0; i < tamanio; i++)
    {

        io = (t_io_list *)list_get(interfaces, 0);
        if (strcmp(io->nombre, nombre_io) == 0)
        {
            return io;
        }
    }
    return NULL;
}

bool puede_realizar_operacion(t_io_list *io, char *operacion)
{
    if (strcmp(io->tipo, "STDOUT") == 0)
    {
        return strcmp(operaciones_stdout[0], operacion) == 0;
    }
    else if (strcmp(io->tipo, "STDIN") == 0)
    {
        return strcmp(operaciones_stdin[0], operacion) == 0;
    }
    else if (strcmp(io->tipo, "GENERIC") == 0)
    {
        return strcmp(operaciones_generic[0], operacion) == 0;
    }
    else if (strcmp(io->tipo, "DIALFS") == 0)
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
        t_proceso_bloqueado *proceso = (t_proceso_bloqueado *)list_remove(io->procesos_bloqueados, 0);
        char *op_a_realizar = (char *)list_remove(proceso->parametros, 0);
        int op_interfaz = string_to_enum_io(op_a_realizar);
        t_paquete *p_interfaz = crear_paquete(op_interfaz);
        uint32_t *pid = &proceso->pcb->PID;
        agregar_a_paquete(p_interfaz, (void *)pid, sizeof(uint32_t));
        agregar_parametros_a_paquete(p_interfaz, proceso->parametros);
        int estado_al_enviar = enviar_paquete_interfaz(p_interfaz, io->fd);

        if (estado_al_enviar != -1)
        {
            int respuesta;
            recv(io->fd, &respuesta, sizeof(int), MSG_WAITALL); // recibe resultado de realizar interfaz
            if (respuesta == OK)
            {

                pthread_mutex_lock(&mutex_lista_BLOCKED);
                int indice = buscar_indice_pcb(proceso->pcb->PID);
                list_remove(pcbs_en_BLOCKED, indice);
                pthread_mutex_lock(&mutex_lista_BLOCKED);

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
            eliminar_paquete(p_interfaz);
            break;
        }

        free(op_a_realizar);
        eliminar_proceso(proceso);
        eliminar_paquete(p_interfaz);
    }

    liberar_interfaz(io);
    return NULL;
}

int buscar_indice_pcb(uint32_t pid)
{
    int tamanio = list_size(pcbs_en_BLOCKED);
    int indice = -1;
    for (int i = 0; i < tamanio; i++)
    {
        t_pcb *pcb = (t_pcb *)list_get(pcbs_en_BLOCKED, i);
        if (pid == pcb->PID)
        {
            indice = i;
        }
    }

    return indice;
}

void liberar_interfaz(t_io_list *io)
{

    free(io->nombre);
    free(io->tipo);
    liberar_procesos_io(io->procesos_bloqueados);
    pthread_mutex_destroy(&io->cola_bloqueados);
    sem_destroy(&io->procesos_en_cola);
    free(io);
}

void liberar_procesos_io(t_list *procesos_io)
{
    int size = list_size(procesos_io);
    for (int i = 0; i <= size; i++)
    {
        t_proceso_bloqueado *proceso = list_remove(procesos_io, i);
        enviar_pcb_a_EXIT(proceso->pcb, INVALID_INTERFACE);
        eliminar_proceso(proceso);
    }
}

void eliminar_proceso(t_proceso_bloqueado *proceso)
{
    list_destroy_and_destroy_elements(proceso->parametros, free);
    free(proceso);
}

int string_to_enum_io(char *str)
{

    if (strcmp(str, "IO_GEN_SLEEP") == 0)
        return IO_GEN_SLEEP;
    if (strcmp(str, "IO_STDIN_READ") == 0)
        return IO_STDIN_READ;
    if (strcmp(str, "IO_STDOUT_WRITE") == 0)
        return IO_STDOUT_WRITE;
    if (strcmp(str, "IO_FS_CREATE") == 0)
        return IO_FS_CREATE;
    if (strcmp(str, "IO_FS_DELETE") == 0)
        return IO_FS_DELETE;
    if (strcmp(str, "IO_FS_TRUNCATE") == 0)
        return IO_FS_TRUNCATE;
    if (strcmp(str, "IO_FS_WRITE") == 0)
        return IO_FS_WRITE;
    if (strcmp(str, "IO_FS_READ") == 0)
        return IO_FS_READ;
    // Este caso no lo considero por que no podrian enviar otros que no sean los anteriores
    return INVALID_INSTRUCTION;
}

// sn