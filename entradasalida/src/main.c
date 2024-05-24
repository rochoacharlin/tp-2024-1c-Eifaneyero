#include "configuraciones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

uint32_t calcular_tiempo_de_espera(int unidades_de_trabajo);
void atender_gen(int conexion);
void atender_stdin(int conexion);
void atender_stdout(int conexion);
void atender_dialfs(int conexion);

int main(int argc, char *argv[])
{
    logger_propio = crear_logger("entradasalida_propio");

    if (argc < 3)
    {
        log_info(logger_propio, "Uso: %s [nombre de interfaz] [ruta de archivo de configuracion]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int conexion;
    char *nombre = malloc(strlen(argv[1]) + 1);
    strcpy(nombre, argv[1]);
    logger_obligatorio = crear_logger("entradasalida_obligatorio");
    log_info(logger_propio, "Iniciando Interfaz de I/O %s", nombre);
    free(nombre);

    char *config_ruta = string_new();
    string_append(&config_ruta, "configuraciones/");
    string_append(&config_ruta, argv[2]);
    config = iniciar_config(logger_propio, config_ruta);
    free(config_ruta);

    conexion = crear_conexion(logger_propio, obtener_ip_kernel(), obtener_puerto_kernel());
    int32_t handshake = 4; // TODO no sé si este handshake debería seguir así o mejor hacemos que le diga su nombre
    int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);

    if (handshake_respuesta)
    {
        char *tipo_interfaz = obtener_tipo_interfaz();
        if (strcmp(tipo_interfaz, "GENERICA") == 0)
        {
            atender_gen(conexion);
        }
        else if (strcmp(tipo_interfaz, "STDIN") == 0)
        {
            atender_gen(conexion);
        }
        else if (strcmp(tipo_interfaz, "STDOUT") == 0)
        {
            atender_gen(conexion);
        }
        else if (strcmp(tipo_interfaz, "DIALFS") == 0)
        {
            atender_gen(conexion);
        }
        free(tipo_interfaz);
    }

    terminar_programa(conexion, logger_propio, config);

    return 0;
}

void atender_gen(int conexion)
{

    uint8_t respuesta;
    t_list *valores = list_create();
    t_id cod_op;
    while (1)
    {
        respuesta = OK;
        cod_op = recibir_operacion(conexion);
        /*if (recv(conexion, &(paquete->codigo_operacion), sizeof(uint8_t), 0) == -1)
        {
            log_error(logger_propio, "Error al recibir cod. op. : %s", strerror(errno));
            respuesta = OPERACION_INVALIDA;
            send(conexion, (void *)OPERACION_INVALIDA, sizeof(uint8_t), 0);
        }*/

        if (cod_op == IO_GEN_SLEEP)
        {
            valores = recibir_paquete(conexion);
            loggear_operacion((int *)list_get(valores, 0), comandos[cod_op]);
            int unidades_de_trabajo = atoi((char *)list_get(valores, 1));
            uint32_t tiempo_de_espera_ms = calcular_tiempo_de_espera(unidades_de_trabajo) * 1000;
            usleep((__useconds_t)tiempo_de_espera_ms);
        }
        else
        {
            respuesta = OPERACION_INVALIDA;
        }

        enviar_cod_op(respuesta, conexion);
    }
}

uint32_t calcular_tiempo_de_espera(int unidades_de_trabajo)
{
    return unidades_de_trabajo * obtener_tiempo_unidad_trabajo();
}
