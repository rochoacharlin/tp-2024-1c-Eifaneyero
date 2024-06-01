#ifndef KERNEL_CONSOLA_H_
#define KERNEL_CONSOLA_H_

// #include <string.h>
#include <readline/readline.h>
#include <stdlib.h>
#include "../planificacion/pcb.h"
#include "../planificacion/planificacion_largo_plazo.h"
#include "../planificacion/planificacion_corto_plazo.h"
#include <unistd.h>

extern sem_t planificacion_liberada;
extern sem_t planificacion_pausada;
extern sem_t sem_grado_multiprogramacion;
extern sem_t termina_comando;

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
void cambiar_valor_de_semaforo(sem_t *sem, int valor_resultante);
void remover_pcb_de_listas_globales(t_pcb *pcb);

#endif