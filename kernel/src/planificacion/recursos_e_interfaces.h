#ifndef KERNEL_RECURSOS_E_INTERFACES_H
#define KERNEL_RECURSOS_E_INTERFACES_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include "../configuraciones.h"
#include <utils/contexto_ejecucion.h>
#include "pcb.h"

extern int *instancias_recursos;
extern t_list *recursos;
extern char **nombres_recursos;

extern int fd_servidor;
extern t_list *interfaces;
extern int conexion_kernel_cpu_dispatch;

typedef struct
{
    int fd;
    char *nombre;
    char *tipo;
    t_list *procesos_bloqueados;
} t_io_list;

void crear_colas_de_bloqueo(void);
void destruir_lista_string(char **lista_string);

void *ejecutar_espera_interfaces(void);
void agregar_a_lista_io_global(char *nombre, char *tipo, int fd);
t_io_list *buscar_interfaz(int interfaz);
t_contexto *esperar_contexto(t_pcb *pcb); // hay que buscarle un mejor nombre a esto
void manejador_interfaz(void *arg);
void *ejecutar_io_generica(void);

#endif