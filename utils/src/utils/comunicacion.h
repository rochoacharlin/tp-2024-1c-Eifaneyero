#ifndef UTILS_COMUNICACION_H_
#define UTILS_COMUNICACION_H_

#include <errno.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    socklen_t ai_addrlen;
    struct sockaddr *ai_addr;
    char *ai_canonname;
    struct addrinfo *ai_next;
} addrinfo;

int crear_conexion(t_log *logger, char *ip, char *puerto);
int conectar_a(t_config *config, t_log *logger, char *clave_ip, char *clave_puerto);
int iniciar_servidor(t_config *config, t_log *logger, char *clave_puerto);
int esperar_cliente(t_log *logger, int server_socket);
void terminar_programa(int conexion, t_log *logger, t_config *config);
int handshake_cliente(t_log *logger, int conexion, int handshake);
int handshake_servidor(t_log *logger, int conexion, int handshake);

#endif