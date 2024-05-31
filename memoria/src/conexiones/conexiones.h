#ifndef MEMORIA_CONEXIONES_H
#define MEMORIA_CONEXIONES_H

#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/estructuras_compartidas.h>

extern t_log *logger_propio;
extern int server_fd;
extern int *sockets[3];

void esperar_a(char *tipo, int *socket, int server);
void iniciar_conexiones();

void atender_cpu();

#endif
