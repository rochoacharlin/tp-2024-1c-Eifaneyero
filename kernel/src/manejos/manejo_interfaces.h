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
extern pthread_mutex_t mutex_interfaces;

extern char *operaciones_stdout[];
extern char *operaciones_stdin[];
extern char *operaciones_generic[];
extern char *operaciones_fs[];
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
    t_pcb *pcb;
    t_list *parametros;

} t_proceso_bloqueado;

typedef enum
{
    IO_GEN_SLEEP = 10,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    INVALID_INSTRUCTION = -1,
} t_id_io;

bool puede_realizar_operacion(t_io_list *io, char *operacion);
void *ejecutar_espera_interfaces(void);
void inicializar_interfaces();
void agregar_a_lista_io_global(char *nombre, char *tipo, int fd);
t_io_list *buscar_interfaz(char *interfaz);
t_contexto *esperar_contexto(t_pcb *pcb); // hay que buscarle un mejor nombre a esto
void manejador_interfaz(t_pcb *pcb, t_list *parametros);
void *atender_interfaz(void *interfaz);
void liberar_procesos_io(t_list *procesos_io);
void eliminar_proceso(t_proceso_bloqueado *proceso);
int string_to_enum_io(char *str);
void liberar_interfaz(t_io_list *io);
int buscar_indice_pcb(uint32_t pid);

#endif