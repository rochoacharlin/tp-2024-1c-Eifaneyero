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
        bloque_utilizados = 0; // esto no serviría para las siguientes ejecuciones
        iniciar_bitmap();
        leer_bloques();
        FILE *ruta_base_fs = fopen(obtener_path_base_dialfs(), "w");
        if (ruta_base_fs == NULL)
        {
            log_error(logger_propio, "No se pudo crear el directorio base del sistema de archivos.");
            exit(EXIT_FAILURE);
        }
        fclose(ruta_base_fs);
    }
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
        usleep((__useconds_t)tiempo_de_espera_ms);
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

        int tam = atoi(list_get(parametros, 1));

        char *lectura = NULL;
        while (!lectura)
        {
            lectura = readline("Ingresar texto para STDIN:");
            if (strlen(lectura) != tam)
            {
                log_info(logger_propio, "Se esperaba una string de %d caracteres", tam);
                free(lectura);
                lectura = NULL;
            }
        }

        int desplazamiento = 0;
        char *texto_a_enviar = NULL;
        for (int i = 2; i < list_size(parametros); i += 2)
        {
            uint32_t direccion_fisica = atoi(list_get(parametros, i));
            uint32_t bytes_a_operar = atoi(list_get(parametros, i + 1));
            texto_a_enviar = string_substring(lectura, desplazamiento, bytes_a_operar);

            t_paquete *paquete = crear_paquete(ACCESO_ESPACIO_USUARIO_ESCRITURA);
            agregar_a_paquete_uint32(paquete, *PID);
            agregar_a_paquete_uint32(paquete, direccion_fisica);
            agregar_a_paquete_string(paquete, texto_a_enviar);
            agregar_a_paquete_uint32(paquete, bytes_a_operar);
            enviar_paquete(paquete, conexion_memoria);

            desplazamiento += bytes_a_operar;
            respuesta = recibir_operacion(conexion_memoria);
            if (respuesta != OK)
            {
                log_info(logger_propio, "Se produjo un error al intentar escribir %s en la memoria", texto_a_enviar);
                respuesta = OPERACION_INVALIDA;
                break;
            }

            eliminar_paquete(paquete);
            free(texto_a_enviar);
        }

        free(lectura);
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
        int cantidad_caracteres = atoi(list_get(parametros, 1));
        int tam = sizeof(char) * cantidad_caracteres + 1;
        char *valor_leido_completo = malloc(tam);
        valor_leido_completo[tam - 1] = '\0';
        int desplazamiento = 0;

        for (int i = 2; i < list_size(parametros); i += 2)
        {
            uint32_t direccion_fisica = atoi(list_get(parametros, i));
            uint32_t bytes_a_operar = atoi(list_get(parametros, i + 1));

            t_paquete *paquete = crear_paquete(ACCESO_ESPACIO_USUARIO_LECTURA);
            agregar_a_paquete_uint32(paquete, *PID);
            agregar_a_paquete_uint32(paquete, direccion_fisica);
            agregar_a_paquete_uint32(paquete, bytes_a_operar);
            enviar_paquete(paquete, conexion_memoria);

            if (recibir_operacion(conexion_memoria) == OK)
            {

                t_list *paquete_recibido = recibir_paquete(conexion_memoria);
                void *valor_leido = list_get(paquete_recibido, 0);
                memcpy(valor_leido_completo + desplazamiento, valor_leido, sizeof(char) * bytes_a_operar);
                list_destroy_and_destroy_elements(paquete_recibido, free);
                desplazamiento += bytes_a_operar;
            }
            else
            {
                log_info(logger_propio, "Se produjo un error al intentar leer %d bytes en la df %d", bytes_a_operar, direccion_fisica);
                respuesta = OPERACION_INVALIDA;
                break;
            }

            eliminar_paquete(paquete);
        }

        log_info(logger_propio, "El valor leido de la memoria para STDOUT es: %s", (char *)valor_leido_completo);
        free(valor_leido_completo);
    }
    else
    {
        respuesta = OPERACION_INVALIDA;
    }

    return respuesta;
}

op_code atender_dialfs(int cod_op, t_list *parametros)
{
    op_code respuesta = OK;
    uint32_t *PID = list_get(parametros, 0);

    switch (cod_op)
    {
    case IO_FS_CREATE:
        crear_archivo(PID, (char *)list_get(parametros, 1));
        break;
    case IO_FS_DELETE:
        eliminar_archivo(PID, (char *)list_get(parametros, 1));
        break;
    case IO_FS_TRUNCATE:
        truncar_archivo(PID, (char *)list_get(parametros, 1), *(int *)list_get(parametros, 2));
        break;
    case IO_FS_READ:
        leer_archivo(PID, (char *)list_get(parametros, 1), *(int *)list_get(parametros, 2), (int *)list_get(parametros, 3));
        break;
    case IO_FS_WRITE:
        escribir_archivo(PID, (char *)list_get(parametros, 1), *(int *)list_get(parametros, 2), (int *)list_get(parametros, 3));
        break;
    default:
        respuesta = OPERACION_INVALIDA;
        break;
    }

    return respuesta;
}

void setear_config(char *archivo_config_io)
{
    char *config_ruta = malloc(strlen("configuraciones/") + strlen(archivo_config_io));
    strcpy(config_ruta, "configuraciones/");
    string_append(&config_ruta, archivo_config_io);
    config = iniciar_config(logger_propio, config_ruta);
    free(config_ruta);
}