#include "configuraciones.h"
#include "consola/consola.h"
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include "conexiones/conexiones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

pthread_t hilo_planificador_largo_plazo;
pthread_t hilo_planificador_corto_plazo;
pthread_t hilo_servidor;
void chicken_test();

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("kernel_obligatorio");
    logger_propio = crear_logger("kernel_propio");
    log_info(logger_propio, "Iniciando Kernel ...");

    config = iniciar_config(logger_propio, "kernel.config");

    // en el caso del servidor creo que tiene que ser un hilo y en el resto de poder reintentar automaticamente conectarse con los otros modulos
    // servidor_kernel_fd = servidor();
    // if (pthread_create(&hilo_servidor, NULL, (void *)servidor, NULL))
    //    log_error(logger_propio, "Error creando el hilo servidor");
    conexion_dispatch_con_CPU();
    // conexion_interrupt_con_CPU();
    // conexion_memoria();

    inicializar_listas_planificacion();
    inicializar_semaforos_planificacion();

    if (pthread_create(&hilo_planificador_largo_plazo, NULL, (void *)planificar_a_largo_plazo, NULL))
        log_error(logger_propio, "Error creando el hilo del planificador de largo plazo");
    if (pthread_create(&hilo_planificador_corto_plazo, NULL, (void *)planificar_a_corto_plazo_segun_algoritmo, NULL))
        log_error(logger_propio, "Error creando el hilo del planificador de corto plazo");

    consola_interactiva();

    // chicken_test();

    close(conexion_kernel_cpu_dispatch);
    close(conexion_kernel_cpu_interrupt);
    close(conexion_kernel_memoria);
    close(servidor_kernel_fd);
    log_destroy(logger_obligatorio);
    log_destroy(logger_propio);
    config_destroy(config);

    return 0;
}

void chicken_test()
{
    conexion_dispatch_con_CPU();
    t_contexto *myLittleContext = iniciar_contexto();
    myLittleContext->PID = 2;

    u_int32_t num = 2;
    u_int32_t *p = &num;
    dictionary_put(myLittleContext->registros_cpu, "PC", p);

    enviar_contexto(conexion_kernel_cpu_dispatch, myLittleContext);

    int motivo_desalojo = recibir_operacion(conexion_kernel_cpu_dispatch);
    if (motivo_desalojo == DESALOJO_EXIT)
    {
        t_contexto *myBigContext = recibir_contexto(conexion_kernel_cpu_dispatch);
        log_info(logger_propio, "PID: %d", myBigContext->PID);
        log_info(logger_propio, "PC: %d", obtener_valor_registro(myBigContext->registros_cpu, "PC"));
        log_info(logger_propio, "EBX: %d", obtener_valor_registro(myBigContext->registros_cpu, "EBX"));
        log_info(logger_propio, "AX: %d", obtener_valor_registro(myBigContext->registros_cpu, "AX"));
        destruir_contexto(myBigContext);
    }
    else if (motivo_desalojo == DESALOJO_IO_GEN_SLEEP)
    { // no hago el recibir contexto porque me deja los parámetros extra afuera... TODO lograr eso en una función o varias
        t_contexto *contexto = iniciar_contexto();

        // Orden en lista: AX, EAX, BX, EBX, CX, ECX, DX, EDX, PC, SI, DI
        t_list *registros_contexto = recibir_paquete(conexion_kernel_cpu_dispatch);

        memcpy(&(contexto->PID), (uint32_t *)list_get(registros_contexto, 0), sizeof(uint32_t));
        dictionary_put(contexto->registros_cpu, "AX", (uint8_t *)list_get(registros_contexto, 1));
        dictionary_put(contexto->registros_cpu, "EAX", (uint32_t *)list_get(registros_contexto, 2));
        dictionary_put(contexto->registros_cpu, "BX", (uint8_t *)list_get(registros_contexto, 3));
        dictionary_put(contexto->registros_cpu, "EBX", (uint32_t *)list_get(registros_contexto, 4));
        dictionary_put(contexto->registros_cpu, "CX", (uint8_t *)list_get(registros_contexto, 5));
        dictionary_put(contexto->registros_cpu, "ECX", (uint32_t *)list_get(registros_contexto, 6));
        dictionary_put(contexto->registros_cpu, "DX", (uint8_t *)list_get(registros_contexto, 7));
        dictionary_put(contexto->registros_cpu, "EDX", (uint32_t *)list_get(registros_contexto, 8));
        dictionary_put(contexto->registros_cpu, "PC", (uint32_t *)list_get(registros_contexto, 9));
        dictionary_put(contexto->registros_cpu, "SI", (uint32_t *)list_get(registros_contexto, 10));
        dictionary_put(contexto->registros_cpu, "DI", (uint32_t *)list_get(registros_contexto, 11));

        log_info(logger_propio, "PC: %d", contexto->PID);
        log_info(logger_propio, "instruccion: %s", (char *)list_get(registros_contexto, 12)); // da 10 y está bien eso
        log_info(logger_propio, "nombre: %s", (char *)list_get(registros_contexto, 13));
        log_info(logger_propio, "tiempo: %s", (char *)list_get(registros_contexto, 14));

        list_destroy_and_destroy_elements(registros_contexto, free);
    }
}
