#include "gestor_interfaces.h"

op_code (*atender)(int cod_op, t_list *parametros) = NULL;

void atender_segun_tipo_interfaz(void)
{
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
}

uint32_t calcular_tiempo_de_espera(int unidades_de_trabajo)
{
    return unidades_de_trabajo * obtener_tiempo_unidad_trabajo();
}

op_code atender_gen(int cod_op, t_list *parametros)
{
    op_code respuesta = OK;

    if (cod_op == IO_GEN_SLEEP)
    {
        loggear_operacion(*(uint32_t *)list_get(parametros, 0), nombres_de_instrucciones[cod_op]);

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

op_code atender_stdin(int cod_op, t_list *parametros)
{
    op_code respuesta = OK;

    if (cod_op == IO_STDIN_READ)
    {
        uint32_t *PID = (uint32_t *)list_get(parametros, 0);
        loggear_operacion(*PID, nombres_de_instrucciones[cod_op]);

        int *tam = (int *)list_get(parametros, 1);
        char lectura[*tam];

        printf("Ingresar texto para STDIN: ");
        scanf("%s", lectura);

        int desplazamiento = 0;
        for (int i = 2; i < list_size(parametros); i += 2)
        {
            uint32_t *direccion_fisica = (uint32_t *)list_get(parametros, i);
            uint32_t *bytes_a_operar = (uint32_t *)list_get(parametros, i + 1);

            char texto_a_enviar[*bytes_a_operar];
            strncpy(texto_a_enviar, lectura + desplazamiento, *bytes_a_operar);

            t_paquete *paquete = crear_paquete(ACCESO_ESPACIO_USUARIO_ESCRITURA);
            agregar_a_paquete_uint32(paquete, *PID);
            agregar_a_paquete_uint32(paquete, *direccion_fisica);
            agregar_a_paquete_string(paquete, texto_a_enviar);
            agregar_a_paquete_uint32(paquete, *bytes_a_operar);
            enviar_paquete(paquete, conexion_memoria);

            desplazamiento += *bytes_a_operar;
            respuesta = recibir_operacion(conexion_memoria);

            eliminar_paquete(paquete);
            free(direccion_fisica);
            free(bytes_a_operar);
        }

        free(tam);
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
        uint32_t *PID = (uint32_t *)list_get(parametros, 0);
        loggear_operacion(*PID, nombres_de_instrucciones[cod_op]);
        int *tam = (int *)list_get(parametros, 1);
        char valor_leido_completo[*tam];

        for (int i = 1; i < list_size(parametros); i += 2)
        {
            uint32_t *direccion_fisica = (uint32_t *)list_get(parametros, i);
            uint32_t *bytes_a_operar = (uint32_t *)list_get(parametros, i + 1);

            t_paquete *paquete = crear_paquete(ACCESO_ESPACIO_USUARIO_LECTURA);
            agregar_a_paquete_uint32(paquete, *PID);
            agregar_a_paquete_uint32(paquete, *direccion_fisica);
            agregar_a_paquete_uint32(paquete, *bytes_a_operar);
            enviar_paquete(paquete, conexion_memoria);

            if (recibir_operacion(conexion_memoria) == OK)
            {
                t_list *paquete_recibido = recibir_paquete(conexion_memoria);
                char *valor_leido = (char *)list_get(paquete_recibido, 0);
                strncat(valor_leido_completo, valor_leido, *bytes_a_operar);
                list_destroy_and_destroy_elements(paquete_recibido, free);
            }

            free(direccion_fisica);
            free(bytes_a_operar);
            eliminar_paquete(paquete);
        }

        log_info(logger_propio, "El valor leido de la memoria para STDOUT es: %s", valor_leido_completo);
        free(tam);
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