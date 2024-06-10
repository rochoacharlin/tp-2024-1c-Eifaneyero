#include <conexiones/conexiones.h>

#define TOPE_DE_IDS 10 // Elegí numero aleatorio.

int server_fd;
int sockets[3];
sem_t semaforos[TOPE_DE_IDS];
t_dictionary *indice_de_instrucciones;

// Para semaforear que se hayan cargado scripts de los procesos, antes de poder solicitar lectura de instrucciones.
void iniciar_semaforo_para_peticiones(void)
{
    uint32_t i;
    for (i = 0; i < TOPE_DE_IDS; i++)
    {
        if (sem_init(&semaforos[i], 0, 0) != 0)
        {
            log_info(logger_propio, "Error en semaforos, choque inminente"); // TODO: logueo diferente los errores?
            exit(1);
        }
    }
}
// TODO: Definir correctamente y reubicar. Por si desean probar. Hallar proxima instrucciion a ejecutar
char *obtener_instruccion_de_script(uint32_t PID, uint32_t PC)
{
    char *string = string_new();
    string = string_duplicate("SET EAX 37");
    return string;
}

void enviar_instruccion_a_cpu(int socket, char *instruccion)
{
    t_paquete *paquete_instruccion = crear_paquete(INSTRUCCION);
    agregar_a_paquete_string(paquete_instruccion, instruccion);
    enviar_paquete(paquete_instruccion, socket);
    eliminar_paquete(paquete_instruccion);
}

// void iniciar_conexiones() // esperar Conexiones
// {
//     // esperar cpu

//     esperar_a("CPU", sockets, server_fd);
//     log_info(logger_propio, "Se conectó la CPU como cliente!");

//     // esperar Kernel

//     usleep(1000 * 500);
//     esperar_a("Kernel", (sockets + 1), server_fd);
//     log_info(logger_propio, "Se conectó la Kenel como cliente!");

//     // eperar E/S

//     usleep(1000 * 500);
//     esperar_a("E/S", (sockets + 2), server_fd);
//     log_info(logger_propio, "Se conectó la E/S como cliente!");
// }

void iniciar_servidor_memoria(void)
{
    server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha()); // Inicio el server memoria
    log_info(logger_propio, "Memoria lista para recibir clientes");
}

void iniciar_conexiones()
{
    iniciar_servidor_memoria();

    sockets[0] = esperar_cliente(logger_propio, server_fd); // socket cpu
    log_info(logger_propio, "Se conectó la CPU como cliente!");

    sockets[1] = esperar_cliente(logger_propio, server_fd); // socket kernel
    log_info(logger_propio, "Se conectó la Kernel como cliente!");

    sockets[2] = esperar_cliente(logger_propio, server_fd); // socket io
    log_info(logger_propio, "Se conectó la IO como cliente!");

    pthread_t hilo_cpu;
    pthread_create(&hilo_cpu, NULL, (void *)atender_cpu, &(sockets[0]));
    pthread_detach(hilo_cpu);

    pthread_t hilo_kernel;
    pthread_create(&hilo_kernel, NULL, (void *)atender_kernel, &(sockets[1]));
    pthread_detach(hilo_kernel);

    pthread_t hilo_io;
    pthread_create(&hilo_io, NULL, (void *)atender_io, &(sockets[2]));
    pthread_join(hilo_io, NULL);
}

void atender_kernel(int socket_cliente)
{
    t_dictionary *indice_de_instrucciones = crear_indice_de_instrucciones();
    while (1)
    {
        op_code codigo = recibir_operacion(sockets[1]);
        log_info(logger_propio, "Recibi algo del kernel en memoria");

        switch (codigo)
        {
        case CREAR_PROCESO_KERNEL: // Recibo path y preparo estructuras administrativas necesarias

            log_info(logger_propio, "Llegó orden para crear un proceso");
            t_list *paquete = recibir_paquete(sockets[1]);
            uint32_t PID = *(uint32_t *)list_get(paquete, 0);
            char *PATH = (char *)list_get(paquete, 1);
            agregar_instrucciones_al_indice(indice_de_instrucciones, PID, PATH);
            sem_post(&semaforos[PID]); // Libero semáforo para que cpu pueda leer instruccion
            break;

        case FINALIZAR_PROCESO_KERNEL:

            log_info(logger_propio, "Orden para finalizar proceso");
            char *PID_a_finalizar = recibir_string(sockets[1]); // TODO: Verificar como se envía el PID
            // Recibo PID
            log_info(logger_propio, "Proceso a finalizar. PID: %s", PID_a_finalizar);
            // TODO: Liberar estructuras. Marcar frames como libres.
            break;

        default:
            log_info(logger_propio, "Codigo de operacion incorrecto");
            break;
        }
    }
}

void atender_cpu(int socket_cliente)
{

    while (1)
    {

        op_code codigo = recibir_operacion(sockets[0]);
        log_info(logger_propio, "Recibí algo de cpu en memoria");

        switch (codigo)
        {

        case SOLICITUD_INSTRUCCION: // En base a PID y PC devuelvo instruccion solicitada

            // 1º El kernel manda el archivo a abrir

            t_list *pid_y_pc = recibir_paquete(sockets[0]);
            uint32_t PID = *(uint32_t *)list_get(pid_y_pc, 0);
            uint32_t PC = *(uint32_t *)list_get(pid_y_pc, 1);

            // TODO: Liberar pid y pc

            sem_wait(&semaforos[PID]); // Blocked hasta que las instrucciones del proceso se hayan cargado.

            log_info(logger_propio, "Semáforo cruzado en verde tras solicitud de instrucción");

            char *instruccion = obtener_instruccion_de_script(PID, PC); // TODO: Obtengo instr cargada en t_dictionary *script_segun_PID

            enviar_instruccion_a_cpu(sockets[0], instruccion);

            log_info(logger_propio, "A hacer noni");
            usleep(obtener_retardo_respuesta() * 500); // TODO: *1000 o *500?
            log_info(logger_propio, "Se termino la siesta");

            sem_post(&semaforos[PID]); // Libero semáforo. Pues ya fueron cargadas las intrucciones.

            break;

        // MOV_OUT desde CPU
        case ACCESO_ESPACIO_USUARIO_ESCRITURA:

            // TODO:
            break;

        // MOV IN desde cpu
        case ACCESO_ESPACIO_USUARIO_LECTURA:

            // TODO:
            break;

        // recibo el numero de pagina del proceso buscado de cpu
        case SOLICITUD_MARCO:
            atender_solicitud_marco();
            break;

        case RESIZE_PROCESO:
            atender_resize();
            break;

        default:

            log_info(logger_propio, "Codigo de operacion incorrecto");
            break;
        }
    }
}

void atender_io(int socket_cliente)
{
    1;
}

void enviar_marco(int marco)
{
    t_paquete *paquete = crear_paquete(SOLICITUD_MARCO);
    agregar_a_paquete(paquete, &marco, sizeof(int));
    enviar_paquete(paquete, sockets[0]);
    eliminar_paquete(paquete);
}

void recibir_solicitud_marco(uint32_t *PID, int *pagina)
{
    t_list *valores_paquete = recibir_paquete(sockets[0]);
    *PID = *(uint32_t *)list_get(valores_paquete, 0);
    *pagina = *(int *)list_get(valores_paquete, 1);
    list_destroy_and_destroy_elements(valores_paquete, free);
}

void atender_solicitud_marco()
{
    uint32_t PID;
    int pagina;
    recibir_solicitud_marco(&PID, &pagina);
    int marco = buscar_marco(PID, pagina);
    loggear_acceso_tablas_de_paginas(PID, pagina, marco);
    enviar_marco(marco);
}

void recibir_solicitud_resize(uint32_t *PID, uint32_t *tamanio_proceso)
{
    t_list *valores_paquete = recibir_paquete(sockets[0]);
    *PID = *(uint32_t *)list_get(valores_paquete, 0);
    *tamanio_proceso = *(uint32_t *)list_get(valores_paquete, 1);
    list_destroy_and_destroy_elements(valores_paquete, free);
}

void atender_resize()
{
    uint32_t PID, tamanio_proceso;
    recibir_solicitud_resize(&PID, &tamanio_proceso);
    op_code respuesta = resize(PID, tamanio_proceso);
    enviar_cod_op(respuesta, sockets[0]);
}

op_code resize(uint32_t PID, uint32_t tamanio_proceso)
{
    t_list *tp_de_proceso = obtener_tp_de_proceso(PID);
    int cantidad_pags = list_size(tp_de_proceso);
    int pags_solicitadas = (tamanio_proceso + obtener_tam_pagina() - 1) / obtener_tam_pagina();
    op_code resultado = OK;

    if (pags_solicitadas < cantidad_pags)
    {
        loggear_reduccion_de_proceso(PID, cantidad_pags * obtener_tam_pagina(), tamanio_proceso);

        for (int i = cantidad_pags - 1; i >= pags_solicitadas; i--)
        {
            quitar_ultima_pagina(tp_de_proceso);
        }
    }

    if (pags_solicitadas > cantidad_pags)
    {
        loggear_ampliacion_de_proceso(PID, cantidad_pags * obtener_tam_pagina(), tamanio_proceso);

        int pags_adicionales = pags_solicitadas - cantidad_pags;
        if (cantidad_marcos_libres() >= pags_adicionales)
        {
            for (int i = 0; i < pags_adicionales; i++)
            {
                agregar_pagina(tp_de_proceso);
            }
        }
        else
        {
            resultado = OUT_OF_MEMORY;
        }
    }

    return resultado;
}