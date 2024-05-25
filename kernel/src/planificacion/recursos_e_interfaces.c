#include "recursos_e_interfaces.h"

int *instancias_recursos;
t_list *recursos;
char **nombres_recursos;

void crear_colas_de_bloqueo(void)
{
    instancias_recursos = NULL;
    recursos = list_create();
    nombres_recursos = obtener_recursos();
    char **instancias_aux = obtener_instancias_recursos();

    for (int i = 0; i < string_array_size(instancias_aux); i++)
    {
        int instancia_en_entero = atoi(instancias_aux[i]);
        instancias_recursos = realloc(instancias_recursos, (i + 1) * sizeof(int));
        instancias_recursos[i] = instancia_en_entero;

        t_list *cola_bloqueo = list_create();
        list_add(recursos, (void *)cola_bloqueo);
    }

    destruir_lista_string(instancias_aux);
}

void destruir_lista_string(char **lista_string)
{
    for (int i = 0; i < string_array_size(lista_string); i++)
        free(lista_string[i]);
    free(lista_string);
}

void *ejecutar_espera_interfaces(void)
{
    interfaces = list_create();
    while (1)
    {
        int fd_cliente = esperar_cliente(logger_propio, fd_servidor);
        int op = recibir_operacion(fd_cliente);
        char *nombre_interfaz, *tipo_interfaz;
        // nombre_interfaz = recibir_mensaje(fd_cliente);
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
    list_add(interfaces, (void *)interfaz);
}

// podria ser un void
t_contexto *esperar_contexto(t_pcb *pcb)
{
    // Probar si funciona o implementar de otra manera
    // recibir_contexto_y_actualizar(conexion_dispatch )
    int motivo_desalojo = recibir_operacion(conexion_kernel_cpu_dispatch);
    t_list *lista_cyp = recibir_paquete(conexion_kernel_cpu_dispatch);

    switch (motivo_desalojo)
    {
    case IO_GENERIC_SLEEP:
    case io_in:
    case io ...:
        /* code */
        break;

    default:
        break;
    }
    if (motivo_desalojo == IO_GENERIC_SLEEP) // pregunta si es interfaz
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
    t_io_list *io = buscar_interfaz(interfaz_e); // que es interfaz_e?
    if (io != NULL)
    {

        if (strcmp(io->tipo, interfaz_s)) // que es interfaz_e?
        {

            pthread_mutex_lock(&cola_pcb_bloqueados);
            agregar_a_lista_bloqueados(pcb);
            pthreas_mutex_unlock(&cola_pcb_bloqueados);

            pthread_mutex_lock(&generic_cola_bloqueados);
            agregar_a_cola_io(io->cola_pcb_bloqueados, pcb);
            pthread_mutex_unlock(&generic_cola_bloqueados);

            sem_post(&semaforo_hay_procesos_en_generic);
        }
    }
}

void *ejecutar_io_generica(void)
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
    // TODO
}