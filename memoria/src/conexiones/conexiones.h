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
#include "gestion_memoria.h"

extern t_log *logger_propio;
extern int server_fd;
extern int sockets[3];
extern t_dictionary *script_segun_PID;

void iniciar_semaforo_para_peticiones(void);

void esperar_a(char *tipo, int *socket, int server);

void iniciar_servidor_memoria(void);
void iniciar_conexiones();

void atender_kernel(int socket_cliente);
void atender_cpu(int socket_cliente);
void atender_io(int socket_cliente);

void crear_estructuras_administrativas(uint32_t PID, char *path);
void recibir_creacion_proceso(uint32_t *PID, char **ptr_path);
void atender_crear_proceso(void);

void liberar_estructuras_administrativas(uint32_t PID);
void recibir_pid(uint32_t *PID);
void atender_finalizar_proceso(void);

void enviar_instruccion_a_cpu(char *instruccion);
char *obtener_instruccion_de_indice(uint32_t PID, uint32_t PC);
void recibir_solicitud_instruccion(uint32_t *PID, uint32_t *PC);
void atender_solicitud_instruccion(void);

void escribir_espacio_usuario(uint32_t direccion, void *valor, uint32_t tamanio);
void recibir_escritura_espacio_usuario(int sockete, uint32_t *PID, uint32_t *direccion_fisica, void *valor_a_escribir, uint32_t *tamanio);
void atender_escritura_espacio_usuario(int sockete);

void enviar_valor_leido(int sockete, void *valor_leido, uint32_t tamanio_valor);
void *leer_espacio_usuario(uint32_t direccion, uint32_t tamanio_a_leer);
void recibir_lectura_espacio_usuario(int sockete, uint32_t *PID, uint32_t *direccion_fisica, uint32_t *tamanio_a_leer);
void atender_lectura_espacio_usuario(int sockete);

void enviar_marco(int marco);
void recibir_solicitud_marco(uint32_t *PID, int *pagina);
void atender_solicitud_marco();

void recibir_solicitud_resize(uint32_t *PID, uint32_t *tamanio_proceso);
void atender_resize();
op_code resize(uint32_t PID, uint32_t tamanio_proceso);

#endif
