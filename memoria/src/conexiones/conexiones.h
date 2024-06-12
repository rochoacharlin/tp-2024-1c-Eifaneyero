#ifndef MEMORIA_CONEXIONES_H
#define MEMORIA_CONEXIONES_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/instrucciones.h>
#include <configuraciones.h>
#include "../tablas/tablas.h"
#include "atender_kernel.h"
#include "atender_cpu.h"
#include "atender_io.h"
#include "gestion_memoria.h"

extern t_log *logger_propio;
extern int server_fd;

void iniciar_semaforo_para_peticiones(void);

void esperar_a(char *tipo, int *socket, int server);

void iniciar_servidor_memoria(void);
void iniciar_conexiones();

#endif
