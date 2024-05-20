#include "configuraciones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

// TODO momentáneo hasta que estén las funciones de serializar y empaquetar
typedef struct
{
    uint8_t codigo_operacion;
    t_buffer *buffer;
} t_paquete;

typedef struct
{
    uint32_t size;   // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void *stream;    // Payload
} t_buffer;

int main(int argc, char *argv[])
{
    logger_propio = crear_logger("entradasalida_propio");

    if (argc < 3)
    {
        log_info("Uso: %s [nombre de interfaz] [ruta de archivo de configuracion]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int conexion;
    char *nombre = malloc(strlen(argv[1]) + 1);
    strcpy(nombre, argv[1]);
    logger_obligatorio = crear_logger("entradasalida_obligatorio");
    log_info(logger_propio, strcat("Iniciando Interfaz de I/O ", nombre));

    char *config_ruta = ".configuraciones/";
    string_append(&config_ruta, argv[2]);
    config = iniciar_config(logger_propio, config_ruta);

    conexion = crear_conexion(logger_propio, obtener_ip_kernel(), obtener_puerto_kernel());
    int32_t handshake = 4; // TODO no sé si este handshake debería seguir así o mejor hacemos que le diga su nombre
    int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);

    if (handshake_respuesta)
    {
        t_paquete *paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));
        while (1)
        {
            if (recv(conexion, &(paquete->codigo_operacion), sizeof(uint8_t), 0) == -1)
            {
                log_error(logger_propio, "Error al recibir cod. op. : %s", strerror(errno));
                break;
            }

            switch (obtener_tipo_interfaz())
            {
            case "GENERICA":
                atender_operacion_gen(paquete);
                break;
            }
        }
    }

    terminar_programa(conexion, logger_propio, config);

    switch (obtener_tipo_interfaz())
    {
        /*
            case CONEXION_MEMORIA:
            {
                conexion = crear_conexion(logger_propio, obtener_ip_memoria(), obtener_puerto_memoria());
                int32_t handshake = 1;
                int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);
                terminar_programa(conexion, logger_propio, config);
            };
            break;
        */
    default:
        log_error(logger_propio, "Modo de ejecucion invalido");
        log_destroy(logger_propio);
        config_destroy(config);
        break;
    }

    return 0;
}

void atender_operacion_gen(t_paquete *paquete)
{
    if (paquete->codigo_operacion == IO_GEN_SLEEP)
    {
        int unidades_de_trabajo = 3; // TODO falta obtener este dato del paquete buffer stream
        int tiempo_de_espera_ms = calcular_tiempo_de_espera(unidades_de_trabajo);
        usleep(tiempo_de_espera_ms);
        send(conexion, OK, sizeof(uint8_t), 0);
    }
    else
    {
        send(conexion, ERROR, sizeof(uint8_t), 0);
    }
}

int calcular_tiempo_de_espera(int unidades_de_trabajo)
{
    return unidades_de_trabajo * obtener_tiempo_unidad_trabajo();
}
