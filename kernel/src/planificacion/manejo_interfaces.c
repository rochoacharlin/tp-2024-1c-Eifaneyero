#include "manejo_interfaces.h"

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

void manejador_interfaz(void *arg)
{
    // se hace el casteo de los parametros, pcb, y a_ejecutar

    // pthreaad_mutex_lock(&semaforo_lista_io))
    // POR AHORA SOLO IMPLENTA PARA IO_GENERIC
    /*t_io_list *io = buscar_interfaz(interfaz_e); // que es interfaz_e?
    if (io != NULL)
    {
        if (strcmp(io->tipo, interfaz_s)) // que es interfaz_s?
        {

            pthread_mutex_lock(&cola_pcb_bloqueados);
            agregar_a_lista_bloqueados(pcb);
            pthreas_mutex_unlock(&cola_pcb_bloqueados);

            pthread_mutex_lock(&generic_cola_bloqueados);
            agregar_a_cola_io(io->cola_pcb_bloqueados, pcb);
            pthread_mutex_unlock(&generic_cola_bloqueados);

            sem_post(&semaforo_hay_procesos_en_generic);
        }
    }*/
}

void *ejecutar_io_generica(void)
{
    while (1)
    {
        /*
        sem_wait(&semaforo_hay_procesos_en_generic);
        pthread_mutex_lock(&generic_cola_bloqueados);

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