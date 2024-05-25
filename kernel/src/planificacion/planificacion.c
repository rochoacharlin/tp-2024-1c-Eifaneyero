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
        planificar_a_corto_plazo(proximo_a_ejecutar_segun_FIFO);
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

        t_contexto *contexto = procesar_pcb_segun_algoritmo(pcb_proximo, algoritmo);
        // int rafaga_CPU = contexto_ejecucion->rafaga_CPU_ejecutada;
        list_remove_element(pcbs_en_EXEC, pcb_proximo);
        list_add(pcbs_en_BLOCKED, pcb_proximo); // provisional
        // retorno_contexto(pcb_proximo, contexto_ejecucion);
    }
}

t_pcb *proximo_a_ejecutar_segun_FIFO(void)
{
    return desencolar_pcb(pcbs_en_READY);
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

void proximo_a_ejecutar_segun_algoritmo(t_pcb *pcb)
{
    char *algoritmo = obtener_algoritmo_planificacion();

    if (strcmp(algoritmo, "FIFO") == 0 |)
    {
        contexto = ejecutar_segun_FIFO(pcb);
    }
    else if (strcmp(algoritmo, "RR") == 0)
    {
        contexto = ejecutar_segun_RR(pcb);
    }
    else if (strcmp(algoritmo, "VRR") == 0)
    {
        // TODO
    }
    else
    {
        log_error(logger_propio, "Algoritmo invalido. Debe ingresar FIFO, RR o VRR");
        abort();
    }
}

t_contexto *ejecutar_segun_FIFO()
{

    // TODO
    //  contexto = esperar_contexto()
}

// Implementacion a probar
t_contexto *ejecutar_segun_RR(t_pcb *pcb)
{
    // falta saber si se hara con un contexto
    enviar_contexto_actualizado(pcb->contexto, conexion_dispatch);
    int quantum = obtener_quantum();

    pthread_create(&hilo_Q, NULL, ejecutar_quamtum, (*void)&pcb);
    pthread_detach(hilo_Q);
    // t_contexto_ejecucion *contexto;
    // contexto = esperar_contexto(pcb);
    pthread_cancel(hilo_Q);

    return contexto;
}
// podria ser un void
t_contexto *esperar_contexto(t_pcb *pcb)
{
    // Probar si funciona o implementar de otra manera
    // recibir_contexto_y_actualizar(conexion_dispatch )
    int algo = recibir_operacion(conexion_a_cpu_dispatch);
    if (algo == IO_GENERIC_SLEPP) // pregunta si es interfaz
    {
        char *interfaz_s = cambiar_interfaz(IO_GENERIC);
        int interfaz_e = IO_GENERIC;
        t_list *lista = recibir_paquete(conexion_a_cpu_dispatch);
        // pcb=actualizar_pcb(lista);
        // remover_de_lista_en_ejecucion(pcb);
        // parametros =obtener_parametros(lista);

        // void*a_modificar=devolver_tipo_void*_que contiene al pcb, parametros y agregar que va a realizar
        // pthread_create(&hilo_manejar_interfaz, NULL, manejadorr_interfaz,a_modificar);
    }
    else
    {
        // recibir_contexto_y_actualizar(conexion_dispatch)
    }
}

void manejador_interfaz(void *arg)
{
    // se hace el casteo de los parametros, pcb, y a_ejecutar

    // pthreaad_mutex_lock(&semaforo_lista_io))
    // POR AHORA SOLO IMPLENTA PARA IO_GENERIC
    t_io_list *io = buscar_interfaz(int interfaz_e);
    if (io != NULL)
    {

        if (strcmp(io->tipo, interfaz_s))
        {

            pthread_mutex_lock(&cola_pcb_bloqueados);
            agregar_a_lista_bloqueados(pcb);
            pthreas_mutex_unlock(&cola_pcb_bloqueados);

            pthread_mutex_lock(&generic_cola_bloqueados);
            agregar_a_cola_io(io->cola_pcb_bloqueados, pcb);
            pthread_mutex_unlock(&generic_cola_bloqueados);

            sem_post(&semaforo_hay_procesos_en_generic);
        }
        if ()
    }
}

void *ejecutar_io_generic()
{

    while (1)
    {

        sem_wait(&semaforo_hay_procesos_en_generic);
        pthread_mutex_lock(&generic_cola_bloqueados);
        /*
        proceso=obtener_proceso_de_cola()
        pthread_mutex_unlock(&generic_cola_bloqueados);
        armar_paquete_para_io();
        enviar_paquete();
        respuesta= esperar_respuesta_io();
        if(respuesta=OK){

            enviar_proceso_a_ready(pcb);
        }
        */
    }
}

t_io_list *buscar_interfaz(int interfaz)
{
}

void *ejecutar_quamtum(void *pcb)
{
    t_pcb *pcb_q = (t_pcb *)pcb;

    usleep(pcb_q->quamtum);
    enviar_interrupcion_FIN_Q(pcb->PID, conexion_interrupt);
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

void *ejecutar_espera_interfaces(void)
{

    t_io_list interfaces[4];

    while (1)
    {

        // conexion_de_interface= servidor(); servidor alterado - me devuelde el fd_cliente
        int op;
        op = recibir_operacion(conexion_de_interfaz);

        switch (op)
        {
        case IO_GENERIC:
            char *interfaz = "IO_GENERIC";
            char *tipo = "SLEEP";
            t_pcb *cola_pcb = NULL;

            agregar_a_lista_global(IO_GENERIC, interfaz, tipo);

            break;
        case IO_STDIN:
            char *interfaz = "IO_STDIN";
            char *tipo = "READ";
            t_pcb *cola_pcb = NULL;

            agregar_a_lista_global(IO_STDIN, interfaz, tipo);

        case IO_STDOUT:
            char *interfaz = "IO_STDOUT";
            char *tipo = "WRITE";
            t_pcb *cola_pcb = NULL;

            agregar_a_lista_global(IO_STDOUT, interfaz, tipo);

        case IO_DIALFS:
            char *interfaz = "IO_DIALFS";
            // char *tipo = "TODO";
            t_pcb *cola_pcb = NULL;

            agregar_a_lista_global(IO_DIALFS, interfaz, tipo);

        default:
            break;
        }
    }

    void crear_array_IO(void)
    {
        // TODO;
    }
}

void agregar_a_lista_global(int interfaz, char *nombre, char *tipo)
{

    interfaces[interfaz].conexion = conexion_de_interfaz;
    strcpy(interfaces[interfaz].nombre_interfaz, nombre);
    strcpy(interfaces[interfaz].tipo, tipo);
    interfaces[interfaz]->cola_pcb_bloqueados = NULL
}

void inicializar_interfaz(char *nombre, char *tipo)
{

    // TODO
}