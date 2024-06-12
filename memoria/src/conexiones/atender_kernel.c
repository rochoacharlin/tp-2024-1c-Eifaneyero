#include "atender_kernel.h"

int socket_kernel;

void atender_kernel(int socket_cliente)
{
    socket_kernel = socket_cliente;
    while (1)
    {
        int op_code = recibir_operacion(socket_kernel);
        retardo_de_peticion();
        switch (op_code)
        {
        case CREAR_PROCESO_KERNEL:
            atender_crear_proceso();
            break;

        case FINALIZAR_PROCESO_KERNEL:
            atender_finalizar_proceso();
            break;

        default:
            log_info(logger_propio, "Codigo de operacion incorrecto en atender_kernel");
            break;
        }
    }
}

void atender_crear_proceso()
{
    uint32_t PID;
    char *path;
    recibir_creacion_proceso(&PID, &path);
    crear_estructuras_administrativas(PID, path);
    sem_post(obtener_sem_instrucciones(PID)); // Libero semáforo para que cpu pueda leer instruccion
}

void recibir_creacion_proceso(uint32_t *PID, char **ptr_path)
{
    t_list *paquete_crear_proceso = recibir_paquete(socket_kernel);
    *PID = *(uint32_t *)list_get(paquete_crear_proceso, 0);
    *ptr_path = string_duplicate((char *)list_get(paquete_crear_proceso, 1));
    list_destroy_and_destroy_elements(paquete_crear_proceso, free);
}

void crear_estructuras_administrativas(uint32_t PID, char *path)
{
    agregar_instrucciones_al_indice(indice_instrucciones, PID, path);
    agregar_proceso_al_indice(PID);
    free(path);
}

void atender_finalizar_proceso(void)
{
    uint32_t PID;
    recibir_pid(&PID);
    liberar_estructuras_administrativas(PID);
    liberar_marcos_proceso(PID);
}

void recibir_pid(uint32_t *PID)
{
    t_list *paquete = recibir_paquete(socket_kernel);
    *PID = *(uint32_t *)list_get(paquete, 0);
    list_destroy_and_destroy_elements(paquete, free);
}

void liberar_estructuras_administrativas(uint32_t PID)
{
    quitar_instrucciones_al_indice(indice_instrucciones, PID);
    quitar_proceso_del_indice(PID);
}
