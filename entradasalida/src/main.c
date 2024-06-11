#include "configuraciones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;
int conexion_memoria;

uint32_t calcular_tiempo_de_espera(int unidades_de_trabajo);
op_code atender_gen(int cod_op, t_list *parametros);
op_code atender_stdin(int cod_op, t_list *parametros);
op_code atender_stdout(int cod_op, t_list *parametros);
op_code atender_dialfs(int cod_op, t_list *parametros);
void setear_config(char *archivo_config_io);

int main(int argc, char *argv[])
{
    logger_propio = crear_logger("entradasalida_propio");

    if (argc < 3)
    {
        log_info(logger_propio, "Uso: %s [nombre de interfaz] [ruta de archivo de configuracion]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *nombre = malloc(strlen(argv[1]) + 1);
    strcpy(nombre, argv[1]);
    logger_obligatorio = crear_logger("entradasalida_obligatorio");
    log_info(logger_propio, "Iniciando Interfaz de I/O %s", nombre);

    setear_config(argv[2]);

    // conectar a memoria
    // conexion_memoria = crear_conexion(logger_propio, obtener_ip_memoria(), obtener_puerto_kernel());

    // conectar al kernel y presentarse
    int conexion = crear_conexion(logger_propio, obtener_ip_kernel(), obtener_puerto_kernel());
    t_paquete *paquete = crear_paquete(PAQUETE);
    agregar_a_paquete_string(paquete, nombre);
    agregar_a_paquete_string(paquete, obtener_tipo_interfaz());
    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete);

    op_code (*atender)(int cod_op, t_list *parametros) = NULL;

    char *tipo_interfaz = obtener_tipo_interfaz();
    if (strcmp(tipo_interfaz, "GENERICA") == 0)
    {
        atender = atender_gen;
    }
    else if (strcmp(tipo_interfaz, "STDIN") == 0)
    {
        atender = atender_stdin;
    }
    else if (strcmp(tipo_interfaz, "STDOUT") == 0)
    {
        atender = atender_stdout;
    }
    else if (strcmp(tipo_interfaz, "DIALFS") == 0)
    {
        atender = atender_dialfs;
    }
    free(tipo_interfaz);

    uint8_t respuesta;
    int cod_op;
    t_list *parametros = list_create();

    while (1)
    {
        cod_op = recibir_operacion(conexion);
        parametros = recibir_paquete(conexion);

        respuesta = atender(cod_op, parametros);

        enviar_cod_op(respuesta, conexion);
    }

    terminar_programa(conexion, logger_propio, config);
    log_destroy(logger_obligatorio);

    return 0;
}

op_code atender_gen(int cod_op, t_list *parametros)
{
    op_code respuesta = OK;

    if (cod_op == IO_GEN_SLEEP)
    {
        loggear_operacion(*(int *)list_get(parametros, 0), nombres_de_instrucciones[cod_op]);

        int unidades_de_trabajo = atoi((char *)list_get(parametros, 1));
        uint32_t tiempo_de_espera_ms = calcular_tiempo_de_espera(unidades_de_trabajo) * 1000;
        // TODO borrar
        log_info(logger_propio, "Voy a dormir por %d unidades", unidades_de_trabajo);

        usleep((__useconds_t)tiempo_de_espera_ms);

        // TODO borrar
        log_info(logger_propio, "Ya desperté");
    }
    else
    {
        respuesta = OPERACION_INVALIDA;
    }

    return respuesta;
}

uint32_t calcular_tiempo_de_espera(int unidades_de_trabajo)
{
    return unidades_de_trabajo * obtener_tiempo_unidad_trabajo();
}

op_code atender_stdin(int cod_op, t_list *parametros)
{
    op_code respuesta = OK;

    if (cod_op == IO_STDIN_READ)
    {
        loggear_operacion(*(int *)list_get(parametros, 0), nombres_de_instrucciones[cod_op]);

        int *tam = (int *)list_get(parametros, 1);
        char lectura[*tam];

        printf("Ingresar texto para STDIN: ");
        scanf("%s", lectura);

        int desplazamiento = 0;
        for (int i = 2; i < list_size(parametros); i += 2)
        {
            int *direccion_fisica = (int *)ist_get(parametros, i);
            int *bytes_a_operar = (int *)list_get(parametros, i + 1);

            char texto_a_enviar[*bytes_a_operar];
            strncpy(texto_a_enviar, lectura + desplazamiento, *bytes_a_operar);

            t_paquete *paquete = crear_paquete(ACCESO_ESPACIO_USUARIO_ESCRITURA);
            agregar_a_paquete(paquete, direccion_fisica, sizeof(int));
            agregar_a_paquete_string(paquete, texto_a_enviar);
            enviar_paquete(paquete, conexion_memoria);

            eliminar_paquete(paquete);
            desplazamiento += *bytes_a_operar;
            respuesta = recibir_operacion(conexion_memoria);
        }
    }
    else
    {
        respuesta = OPERACION_INVALIDA;
    }

    return respuesta;
}

op_code atender_stdout(int cod_op, t_list *parametros)
{
    op_code respuesta = OK;

    if (cod_op == IO_STDOUT_WRITE)
    {
        // creo que el primer parametro de la lista no es un PID cuando se manda en el kernel, habria que verificar eso
        loggear_operacion(*(int *)list_get(parametros, 0), nombres_de_instrucciones[cod_op]);
        int *tam = (int *)list_get(parametros, 3);
        char valor_leido_completo[*tam];

        for (int i = 3; i < list_size(parametros); i++)
        {
            t_paquete *paquete = crear_paquete(ACCESO_ESPACIO_USUARIO_LECTURA);
            int *direccion_fisica = list_get(parametros, i);
            agregar_a_paquete(paquete, direccion_fisica, sizeof(int));
            enviar_paquete(paquete, conexion_memoria);

            char *valor_leido = recibir_buffer(conexion_memoria, tam);
            // el valor_leido debe con

            eliminar_paquete(paquete);
        }

        log_info(logger_propio, "El valor leido de la memoria para STDOUT es: %s", valor_leido_completo);
    }
    else
    {
        respuesta = OPERACION_INVALIDA;
    }

    return respuesta;
}

op_code atender_dialfs(int cod_op, t_list *parametros)
{
    return 1; // TODO
}

void setear_config(char *archivo_config_io)
{
    char *config_ruta = string_new();
    string_append(&config_ruta, "configuraciones/");
    string_append(&config_ruta, archivo_config_io);
    config = iniciar_config(logger_propio, config_ruta);
    free(config_ruta);
}