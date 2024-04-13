#include <comunicacion/comunicacion.h>

int crear_conexion(t_log *logger, char *ip, char *puerto)
{
    struct addrinfo hints, *server_info;
    int r;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    r = getaddrinfo(ip, puerto, &hints, &server_info);
    if (r)
    {
        log_error(logger, "No se pudo obtener información para conectar al puerto %s en la IP %s: %s", puerto, ip, gai_strerror(r));
        exit(EXIT_FAILURE);
    }

    int socket_cliente = socket(server_info->ai_family,
                                server_info->ai_socktype,
                                server_info->ai_protocol);
    if (socket_cliente == -1)
    {
        log_error(logger, "No se pudo crear el socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    r = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
    if (r == -1)
    {
        log_error(logger, "No se pudo conectar el socket al puerto %s en la IP %s: %s", puerto, ip, strerror(errno));
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(server_info);

    return socket_cliente;
}

int conectar_a(t_config *config, t_log *logger, char *clave_ip, char *clave_puerto)
{
    char *ip = config_get_string_value(config, "IP_MEMORIA");
    char *puerto = config_get_string_value(config, "PUERTO_MEMORIA");
    int conexion = crear_conexion(logger, ip, puerto);

    free(ip);
    free(puerto);

    return conexion;
}