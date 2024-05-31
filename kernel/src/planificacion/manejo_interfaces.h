#ifndef KERNEL_MANEJO_INTERFACES_H
#define KERNEL_MANEJO_INTERFACES_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include "../configuraciones.h"
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/estructuras_compartidas.h>
#include "pcb.h"

extern int fd_servidor;
extern t_list *interfaces;
extern int conexion_kernel_cpu_dispatch;
extern int servidor_kernel_fd;

typedef struct
{
    int fd;
    char *nombre;
    char *tipo;
    t_list *procesos_bloqueados;
} t_io_list;

void *ejecutar_espera_interfaces(void);
void agregar_a_lista_io_global(char *nombre, char *tipo, int fd);
t_io_list *buscar_interfaz(int interfaz);
t_contexto *esperar_contexto(t_pcb *pcb); // hay que buscarle un mejor nombre a esto
void manejador_interfaz(void *arg);
void *ejecutar_io_generica(void);

#endif