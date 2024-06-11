#include <conexiones/conexiones.h>

#define TOPE_DE_IDS 100 // Elegí numero aleatorio.

int server_fd;
int sockets[3];
sem_t semaforos[TOPE_DE_IDS]; // TODOF: t_list, agregar semaforo a medida que inicio proceso

void iniciar_semaforo_para_peticiones(void) // Para semaforear carga scripts, antes de poder solicitar lectura de instrucciones.
{
    uint32_t i;
    for (i = 0; i < TOPE_DE_IDS; i++)
    {
        if (sem_init(&semaforos[i], 0, 0) != 0)
        {
            log_info(logger_propio, "Error en semaforos, choque inminente");
            exit(1);
        }
    }
}

void iniciar_servidor_memoria(void)
{
    server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha());
    log_info(logger_propio, "Memoria lista para recibir clientes");
}

void iniciar_conexiones()
{
    iniciar_servidor_memoria();

    sockets[0] = -1;
    sockets[1] = -1;
    sockets[2] = -1;

    while (sockets[0] == -1 || sockets[1] == -1 || sockets[2] == -1)
    {
        int conexion_entrante = esperar_cliente(logger_propio, server_fd);
        int codigo_de_operacion = recibir_operacion(conexion_entrante);

        if (codigo_de_operacion == CONEXION_KERNEL)
        {
            sockets[0] = conexion_entrante;
            log_info(logger_propio, "Se conectó la CPU como cliente!");
        }
        if (codigo_de_operacion == CONEXION_CPU)
        {
            sockets[1] = conexion_entrante;
            log_info(logger_propio, "Se conectó la Kernel como cliente!");
        }
        if (codigo_de_operacion == CONEXION_IO)
        {
            sockets[2] = conexion_entrante;
            log_info(logger_propio, "Se conectó la IO como cliente!");
        }
    }

    pthread_t hilo_kernel;
    pthread_create(&hilo_kernel, NULL, (void *)atender_kernel, &(sockets[0]));
    pthread_detach(hilo_kernel);

    pthread_t hilo_cpu;
    pthread_create(&hilo_cpu, NULL, (void *)atender_cpu, &(sockets[1]));
    pthread_detach(hilo_cpu);

    pthread_t hilo_io;
    pthread_create(&hilo_io, NULL, (void *)atender_io, &(sockets[2]));
    pthread_join(hilo_io, NULL);
}

void atender_kernel(int socket_cliente)
{
    while (1)
    {
        switch (recibir_operacion(socket_cliente))
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

void atender_cpu(int socket_cliente)
{
    while (1)
    {
        switch (recibir_operacion(socket_cliente))
        {
        case SOLICITUD_INSTRUCCION:
            atender_solicitud_instruccion(); // Para arrancar, el kernel tiene que mandar el archivo a abrir.
            break;

        case ACCESO_ESPACIO_USUARIO_ESCRITURA:
            atender_escritura_espacio_usuario(socket_cliente);
            break;

        case ACCESO_ESPACIO_USUARIO_LECTURA:
            atender_lectura_espacio_usuario(socket_cliente);
            break;

        case SOLICITUD_MARCO:
            atender_solicitud_marco();
            break;

        case RESIZE_PROCESO:
            atender_resize();
            break;

        default:
            log_info(logger_propio, "Codigo de operacion incorrecto en atender_cpu");
            break;
        }
    }
}

void atender_io(int socket_cliente)
{
    while (1)
    {
        switch (recibir_operacion(socket_cliente))
        {

        case ACCESO_ESPACIO_USUARIO_ESCRITURA:
            atender_escritura_espacio_usuario(socket_cliente);
            break;

        case ACCESO_ESPACIO_USUARIO_LECTURA:
            atender_lectura_espacio_usuario(socket_cliente);
            break;

        default:
            log_info(logger_propio, "Codigo de operacion incorrecto en atender_io");
            break;
        }
    }
}

void enviar_marco(int marco)
{
    t_paquete *paquete = crear_paquete(SOLICITUD_MARCO);
    agregar_a_paquete(paquete, &marco, sizeof(int));
    enviar_paquete(paquete, sockets[1]);
    eliminar_paquete(paquete);
}

void recibir_solicitud_marco(uint32_t *PID, int *pagina)
{
    t_list *valores_paquete = recibir_paquete(sockets[1]);
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
    t_list *valores_paquete = recibir_paquete(sockets[1]);
    *PID = *(uint32_t *)list_get(valores_paquete, 0);
    *tamanio_proceso = *(uint32_t *)list_get(valores_paquete, 1);
    list_destroy_and_destroy_elements(valores_paquete, free);
}

void atender_resize()
{
    uint32_t PID, tamanio_proceso;
    recibir_solicitud_resize(&PID, &tamanio_proceso);
    op_code respuesta = resize(PID, tamanio_proceso);
    enviar_cod_op(respuesta, sockets[1]);
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

void atender_crear_proceso()
{
    uint32_t PID;
    char *path;
    recibir_creacion_proceso(&PID, &path);
    crear_estructuras_administrativas(PID, path);
    sem_post(&semaforos[PID]); // Libero semáforo para que cpu pueda leer instruccion
}

void recibir_creacion_proceso(uint32_t *PID, char **ptr_path)
{
    t_list *paquete_crear_proceso = recibir_paquete(sockets[0]);
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
    t_list *paquete = recibir_paquete(sockets[0]);
    *PID = *(uint32_t *)list_get(paquete, 0);
    list_destroy_and_destroy_elements(paquete, free);
}

void liberar_estructuras_administrativas(uint32_t PID)
{
    quitar_instrucciones_al_indice(indice_instrucciones, PID);
    quitar_proceso_del_indice(PID);
}

void atender_solicitud_instruccion(void)
{
    uint32_t PID, PC;
    recibir_solicitud_instruccion(&PID, &PC);

    sem_wait(&semaforos[PID]); // Blocked hasta que las instrucciones del proceso se hayan cargado.
    log_info(logger_propio, "Semáforo cruzado en verde tras recibir solicitud de instrucción");

    char *instruccion = obtener_instruccion_de_indice(PID, PC);
    usleep(obtener_retardo_respuesta() * 1000);
    enviar_instruccion_a_cpu(instruccion);

    sem_post(&semaforos[PID]); // Libero. Ya no hay que esperar la carga de intrucciones.
}

void recibir_solicitud_instruccion(uint32_t *PID, uint32_t *PC)
{
    t_list *pid_y_pc = recibir_paquete(sockets[1]);
    *PID = *(uint32_t *)list_get(pid_y_pc, 0);
    *PC = *(uint32_t *)list_get(pid_y_pc, 1);
    list_destroy_and_destroy_elements(pid_y_pc, free);
}

char *obtener_instruccion_de_indice(uint32_t PID, uint32_t PC)
{
    t_list *instrucciones = (t_list *)dictionary_get(indice_instrucciones, string_itoa(PID));
    char *instruccion = list_get(instrucciones, PC);
    return instruccion;
}

void enviar_instruccion_a_cpu(char *instruccion)
{
    t_paquete *paquete_instruccion = crear_paquete(INSTRUCCION);
    agregar_a_paquete_string(paquete_instruccion, instruccion);
    enviar_paquete(paquete_instruccion, sockets[1]);
    eliminar_paquete(paquete_instruccion);
}

void atender_escritura_espacio_usuario(int sockete)
{
    uint32_t PID, direccion_fisica, tamanio; // Orden
    void *valor_a_escribir = NULL;
    recibir_escritura_espacio_usuario(sockete, &PID, &direccion_fisica, valor_a_escribir, &tamanio);
    escribir_espacio_usuario(direccion_fisica, valor_a_escribir, tamanio);
    enviar_cod_op(OK, sockete);

    loggear_escritura_espacio_de_usuario(PID, direccion_fisica, tamanio);
    free(valor_a_escribir);
}

void recibir_escritura_espacio_usuario(int sockete, uint32_t *PID, uint32_t *direccion_fisica,
                                       void *valor_a_escribir, uint32_t *tamanio)
{
    t_list *paquete_escritura_espacio = recibir_paquete(sockete);
    *PID = *(uint32_t *)list_get(paquete_escritura_espacio, 0);
    *direccion_fisica = *(uint32_t *)list_get(paquete_escritura_espacio, 1);
    void *valor_temp = list_get(paquete_escritura_espacio, 2);
    *tamanio = *(uint32_t *)list_get(paquete_escritura_espacio, 3);

    valor_a_escribir = malloc(*tamanio);
    memcpy(valor_a_escribir, valor_temp, *tamanio);

    list_destroy_and_destroy_elements(paquete_escritura_espacio, free);
}

void escribir_espacio_usuario(uint32_t direccion, void *valor, uint32_t tamanio)
{
    pthread_mutex_lock(&mutex_memoria);
    memcpy(espacio_usuario + direccion, valor, tamanio);
    pthread_mutex_unlock(&mutex_memoria);
}

void atender_lectura_espacio_usuario(int sockete)
{
    uint32_t PID, direccion_fisica, tamanio_a_leer;
    recibir_lectura_espacio_usuario(sockete, &PID, &direccion_fisica, &tamanio_a_leer);
    void *valor_leido = leer_espacio_usuario(direccion_fisica, tamanio_a_leer);
    enviar_valor_leido(sockete, valor_leido, tamanio_a_leer);

    loggear_lectura_espacio_de_usuario(PID, direccion_fisica, tamanio_a_leer);
    free(valor_leido);
}

void recibir_lectura_espacio_usuario(int sockete, uint32_t *PID, uint32_t *direccion_fisica, uint32_t *tamanio_a_leer)
{
    t_list *paquete_lectura_espacio = recibir_paquete(sockete);
    *PID = *(uint32_t *)list_get(paquete_lectura_espacio, 0);
    *direccion_fisica = *(uint32_t *)list_get(paquete_lectura_espacio, 1);
    *tamanio_a_leer = *(uint32_t *)list_get(paquete_lectura_espacio, 2);
    list_destroy_and_destroy_elements(paquete_lectura_espacio, free);
}

void *leer_espacio_usuario(uint32_t direccion, uint32_t tamanio_a_leer)
{
    void *valor = malloc(tamanio_a_leer);
    pthread_mutex_lock(&mutex_memoria);
    memcpy(&valor, espacio_usuario + direccion, tamanio_a_leer);
    pthread_mutex_unlock(&mutex_memoria);

    return valor;
}

void enviar_valor_leido(int sockete, void *valor_leido, uint32_t tamanio_valor)
{
    t_paquete *paquete_valor_leido = crear_paquete(OK);
    agregar_a_paquete(paquete_valor_leido, valor_leido, tamanio_valor);
    enviar_paquete(paquete_valor_leido, sockete);
    eliminar_paquete(paquete_valor_leido);
}