#ifndef KERNEL_CONSOLA_H_
#define KERNEL_CONSOLA_H_

// #include <string.h>
#include <readline/readline.h>
#include <stdlib.h>
#include "../planificacion/pcb.h"
#include "../planificacion/planificacion.h"
#include <unistd.h>

extern sem_t planificacion_liberada;
extern sem_t planificacion_pausada;

typedef struct t_comando
{
    char *nombre;
    void (*funcion_de_comando)();
} t_comando;

void consola_interactiva(void);
void ejecutar_script(char *path);
void iniciar_proceso(char *path);
void finalizar_proceso(char *PID);
void detener_planificacion(void);
void iniciar_planificacion(void);
void cambiar_grado_multiprogramacion(char *valor);
void listar_procesos_por_cada_estado(void);
void listar_procesos_por_estado(char *estado, t_list *lista);

void buscar_y_ejecutar_comando(char *token);

#endif