#include <conexiones/conexiones.h>

#define TOPE_DE_IDS 10 // Elegì numero aleatorio, debería ser el grado de multiprogramacion?

int server_fd;
int sockets[3];
uint32_t PID = 0;
sem_t semaforos[TOPE_DE_IDS];
t_dictionary *script_segun_PID;

// Diccionario que relaciona PIDs con scripts de instrucciones. Reubicar.
t_dictionary *crear_diccionario_script_segun_PID(void)
{
    t_dictionary *diccionario = dictionary_create();
    return diccionario;
}

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
    t_dictionary *script_segun_PID = crear_diccionario_script_segun_PID();
    while (1)
    {
        op_code codigo = recibir_operacion(sockets[1]);
        log_info(logger_propio, "Recibi algo del kernel en memoria");

        switch (codigo)
        {
        case CREAR_PROCESO_KERNEL: // Recibo path y preparo estructuras administrativas necesarias (?)

            log_info(logger_propio, "Orden para crear un proceso");
            char *path = recibir_string(sockets[1]);
            t_list *instrucciones_por_script = subir_instrucciones(path);
            dictionary_put(script_segun_PID, string_itoa(PID), (void *)instrucciones_por_script); // Agrego PID y script a diccionario.
            sem_post(&semaforos[PID]);                                                            // Libero semáforo para que cpu pueda leer instruccion
            PID++;                                                                                // Incremento para que cuando se cree otro proceso coincida PID de Kernel
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
            uint32_t pid_recibido = *(uint32_t *)list_get(pid_y_pc, 0);
            uint32_t pc_recibido = *(uint32_t *)list_get(pid_y_pc, 1);

            // TODO: Liberar pid y pc

            sem_wait(&semaforos[pid_recibido]); // Blocked hasta que las instrucciones del proceso se hayan cargado.

            log_info(logger_propio, "Semáforo cruzado en verde tras solicitud de instrucción");

            char *instruccion = obtener_instruccion_de_script(pid_recibido, pc_recibido); // TODO: Obtengo instr cargada en t_dictionary *script_segun_PID

            enviar_instruccion_a_cpu(sockets[0], instruccion);

            log_info(logger_propio, "A hacer noni");
            usleep(obtener_retardo_respuesta() * 500); // TODO: *1000 o *500?
            log_info(logger_propio, "Se termino la siesta");

            sem_post(&semaforos[pid_recibido]); // Libero semáforo. Pues ya fueron cargadas las intrucciones.

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

int buscar_marco(uint32_t PID, int pagina)
{
    t_list *tp_de_proceso = dictionary_get(indice_tablas, string_itoa(PID));
    int marco = *(int *)list_get(tp_de_proceso, pagina);
    return marco;
}

void atender_solicitud_marco()
{
    uint32_t PID;
    int pagina;
    recibir_solicitud_marco(&PID, &pagina);
    int marco = buscar_marco(PID, pagina);
    enviar_marco(marco);
}
