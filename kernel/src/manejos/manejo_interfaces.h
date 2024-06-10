#ifndef KERNEL_MANEJO_INTERFACES_H
#define KERNEL_MANEJO_INTERFACES_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include "../configuraciones.h"
#include <utils/comunicacion/comunicacion.h>
#include "../planificacion/pcb.h"
#include "../planificacion/planificacion.h"

extern int fd_servidor;
extern t_list *interfaces;
extern int conexion_kernel_cpu_dispatch;
extern int servidor_kernel_fd;

static char *operaciones_stdout[] = {"IO_STDOUT_WRITE"};
static char *operaciones_stdin[] = {"IO_STDIN_READ"};
static char *operaciones_generic[] = {"IO_GENERIC_SLEEP"};
static char *operaciones_fs[] = {"IO_FS_CREATE", "IO_FS_DELETE", "IO_FS_TRUNCATE", "IO_FS_WRITE", "IO_FS_READ"};
typedef struct
{
    int fd;
    char *nombre;
    char *tipo;
    t_list *procesos_bloqueados;
    pthread_mutex_t cola_bloqueados;
    sem_t procesos_en_cola;

} t_io_list;

typedef struct
{
    t_pcb pcb;
    t_list *parametros;

} t_proceso_bloqueado;

bool puede_realizar_operacion(t_io_list *io, char *operacion);
void *ejecutar_espera_interfaces(void);
void agregar_a_lista_io_global(char *nombre, char *tipo, int fd);
t_io_list *buscar_interfaz(char *interfaz);
t_contexto *esperar_contexto(t_pcb *pcb); // hay que buscarle un mejor nombre a esto
void manejador_interfaz(void *arg);
void *ejecutar_io_generica(void);
void *atender_interfaz(void *interfaz);
void liberar_procesos_io(t_list *procesos_io);
void eliminar_process(t_proceso_bloqueado *process);

#endif