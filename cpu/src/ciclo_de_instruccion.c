#include <ciclo_de_instruccion.h>

// pthread_mutex_t mutex_interrupt;
char *motivo_interrupcion;
bool continua_ejecucion = true;
bool hay_interrupcion = false;
// bool enviar_interrupcion = false;
t_contexto *contexto; // Diferencia e/ t_contexto* y t_contexto

void ciclo_de_instruccion(t_contexto *contexto_a_ejecutar)
{
    contexto = contexto_a_ejecutar;
    continua_ejecucion = true;
    hay_interrupcion = false;

    while (continua_ejecucion)
    {
        t_instruccion *instruccion = fetch();
        decode(instruccion); // No hace nada, por ahora
        execute(instruccion);
        check_interrupt(instruccion);
        destruir_instruccion(instruccion);
    }
}

// -------------------- FETCH -------------------- //

t_instruccion *fetch()
{
    t_instruccion *instruccion_recibida = malloc(sizeof(t_instruccion));
    uint32_t desplazamiento = obtener_valor_registro(contexto->registros_cpu, "PC"); // TODO: Desplazamiento real?

    solicitar_lectura_de_instruccion(desplazamiento);

    char *instruccion_string = recibir_instruccion_string_memoria();

    instruccion_recibida = convertir_string_a_instruccion(instruccion_string);

    free(instruccion_string);
    loggear_fetch_instrucccion(contexto->PID, obtener_valor_registro(contexto->registros_cpu, "PC"));
    return instruccion_recibida;
}

// char *recibir_instruccion_string_memoria() // TODO F
// {
//     if (recibir_operacion(conexion_cpu_memoria) == INSTRUCCION)
//     {
//         t_list *paquete = recibir_paquete(conexion_cpu_memoria);
//         t_instruccion_cadena *instruccion_string;
//         generar_instruccion(instruccion_string, paquete);

//         return instruccion_string->instruccion;
//     }
// }

char *recibir_instruccion_string_memoria() // TODO F
{
    char *unaInstruccion = string_new();
    unaInstruccion = string_duplicate("SET AX 10");
    // unaInstruccion = string_duplicate("SET PC 10");
    // unaInstruccion = string_duplicate("IO_GEN_SLEEP Generica 10");
    // unaInstruccion = string_duplicate("EXIT");
    return unaInstruccion;
}

t_instruccion *convertir_string_a_instruccion(char *instruccion_string)
{
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    if (instruccion == NULL)
    {
        printf("Error al asignar memoria para la instrucción");
        return NULL; // o exit(1)
    }
    inicializar_instruccion(instruccion);

    char **instruccion_array = string_split(instruccion_string, " ");

    // Asignar el id y los parámetros si existen
    instruccion->id = string_id_to_enum_id(instruccion_array[0]);
    if (instruccion_array[1] != NULL)
        instruccion->param1 = string_duplicate(instruccion_array[1]);
    if (instruccion_array[2] != NULL)
        instruccion->param2 = string_duplicate(instruccion_array[2]);
    if (instruccion_array[3] != NULL)
        instruccion->param3 = string_duplicate(instruccion_array[3]);
    if (instruccion_array[4] != NULL)
        instruccion->param4 = string_duplicate(instruccion_array[4]);
    if (instruccion_array[5] != NULL)
        instruccion->param5 = string_duplicate(instruccion_array[5]);

    free(instruccion_array);
    return instruccion;
}

t_instruccion *inicializar_instruccion(t_instruccion *instruccion)
{
    instruccion->id = EXIT;
    instruccion->param1 = NULL;
    instruccion->param2 = NULL;
    instruccion->param3 = NULL;
    instruccion->param4 = NULL;
    instruccion->param5 = NULL;
    return instruccion;
}

void destruir_instruccion(t_instruccion *instruccion)
{
    if (instruccion->param1 != NULL)
        free(instruccion->param1);
    if (instruccion->param2 != NULL)
        free(instruccion->param2);
    if (instruccion->param3 != NULL)
        free(instruccion->param3);
    if (instruccion->param4 != NULL)
        free(instruccion->param4);
    if (instruccion->param5 != NULL)
        free(instruccion->param5);
    free(instruccion);
}

t_id string_id_to_enum_id(char *id_string)
{
    t_id id_enum;

    if (strcmp(id_string, "SET") == 0)
        id_enum = SET;
    else if (strcmp(id_string, "MOV_IN") == 0)
        id_enum = MOV_IN;
    else if (strcmp(id_string, "MOV_OUT") == 0)
        id_enum = MOV_OUT;
    else if (strcmp(id_string, "SUM") == 0)
        id_enum = SUM;
    else if (strcmp(id_string, "SUB") == 0)
        id_enum = SUB;
    else if (strcmp(id_string, "JNZ") == 0)
        id_enum = JNZ;
    else if (strcmp(id_string, "RESIZE") == 0)
        id_enum = RESIZE;
    else if (strcmp(id_string, "COPY_STRING") == 0)
        id_enum = COPY_STRING;
    else if (strcmp(id_string, "WAIT") == 0)
        id_enum = WAIT;
    else if (strcmp(id_string, "SIGNAL") == 0)
        id_enum = SIGNAL;
    else if (strcmp(id_string, "IO_GEN_SLEEP") == 0)
        id_enum = IO_GEN_SLEEP;
    else if (strcmp(id_string, "IO_STDIN_READ") == 0)
        id_enum = IO_STDIN_READ;
    else if (strcmp(id_string, "IO_STDOUT_WRITE") == 0)
        id_enum = IO_STDOUT_WRITE;
    else if (strcmp(id_string, "IO_FS_CREATE") == 0)
        id_enum = IO_FS_CREATE;
    else if (strcmp(id_string, "IO_FS_DELETE") == 0)
        id_enum = IO_FS_DELETE;
    else if (strcmp(id_string, "IO_FS_TRUNCATE") == 0)
        id_enum = IO_FS_TRUNCATE;
    else if (strcmp(id_string, "IO_FS_WRITE") == 0)
        id_enum = IO_FS_WRITE;
    else if (strcmp(id_string, "IO_FS_READ") == 0)
        id_enum = IO_FS_READ;
    else
        id_enum = EXIT;

    return id_enum;
}

// -------------------- DECODE -------------------- //

void decode(t_instruccion *instruccion)
{
    1;
}

// -------------------- EXECUTE -------------------- //

void execute(t_instruccion *instruccion)
{
    switch (instruccion->id)
    {
    case SET:
        set(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: SET - <%s %s>", contexto->PID, instruccion->param1, instruccion->param2);
        break;

    case SUM:
        sum(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: SUM - <%s %s>", contexto->PID, instruccion->param1, instruccion->param2);
        break;

    case SUB:
        sub(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: SUB - <%s %s>", contexto->PID, instruccion->param1, instruccion->param2);
        break;

    case JNZ:
        jnz(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: JNZ - <%s %s>", contexto->PID, instruccion->param1, instruccion->param2);
        break;

    case IO_GEN_SLEEP:
        io_gen_sleep(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: SLEEP - <%s %s> ", contexto->PID, instruccion->param1, instruccion->param2);
        break;

    case EXIT:
        exit_inst();
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: EXIT", contexto->PID);
        break;
    default:
        break;
    }

    // Incremento PC, al menos que ejecute SET PC XXX o JNZ 0 INSTRUCCION) - ¿o que la instruccion sea exit?
    if ((!(instruccion->id == SET && !strcmp(instruccion->param1, "PC")) && !(instruccion->id == JNZ && instruccion->param1 != 0) /* && !(instruccion->id == EXIT) */))
    {
        uint32_t valor = obtener_valor_registro(contexto->registros_cpu, "PC");
        valor++;
        char *valor_string = string_itoa(valor);
        set("PC", valor_string); // TODO F: Cuando se libera?
    }
}

bool instruccion_bloqueante(t_id id_instruccion)
{
    if (id_instruccion == IO_GEN_SLEEP || id_instruccion == EXIT) // TODO:Agregar nuevas instrucciones bloqueantes
        return true;
    return false;
}

// -------------------- CHECK INTERRUPT -------------------- //

void check_interrupt(t_instruccion *instruccion)
{
    if (!instruccion_bloqueante(instruccion->id))
    {
        continua_ejecucion = false;
        hay_interrupcion = false; // Desacarto interrupcion para que no afecte otro proceso. La maneja el kernel?
        return;
    }
    else if (hay_interrupcion)
    {

        motivo_desalojo motivo = string_interrupcion_to_enum_motivo(motivo_interrupcion);
        hay_interrupcion = false;

        devolver_contexto(motivo, NULL);
        continua_ejecucion = false;

        // TODO F: free(motivo); Cuando te libero? Responsabilidad del kernel
    }
    else
    {
        return;
    }
}

char *recibir_interrupcion() // TODO F: Chequear.
{
    if (recibir_operacion(conexion_cpu_kernel_interrupt) == INTERRUPCION)
    {
        int *size = malloc(sizeof(int));
        recv(conexion_cpu_kernel_interrupt, size, sizeof(int), MSG_WAITALL);
        void *buffer = malloc(*size);
        recv(conexion_cpu_kernel_interrupt, buffer, *size, MSG_WAITALL);
        free(size);
        log_info(logger_propio, "recibir_interrupcion(): motivo interrupcion: %s", (char *)buffer);
        return (char *)buffer;
    }
    else
    {
        log_info(logger_propio, "Error: recibir_interrupcion(): Op Code != INTERRUPCION");
    }
    return NULL; // TODO F ?
}

motivo_desalojo string_interrupcion_to_enum_motivo(char *interrupcion) // TODO F
{
    motivo_desalojo motivo;

    if (strcmp(interrupcion, "EXIT") == 0)
        motivo = DESALOJO_EXIT;
    else if (strcmp(interrupcion, "FIN_QUANTUM") == 0)
        motivo = DESALOJO_FIN_QUANTUM;
    else
        log_info(logger_propio, "Motivo de desalojo inexistente");
    return motivo;
}

// -------------------- INSTRUCCIONES -------------------- //

void set(char *nombre_registro, char *valor)
{
    if (strlen(nombre_registro) == 3 || !strcmp(nombre_registro, "SI") || !strcmp(nombre_registro, "DI") || !strcmp(nombre_registro, "PC")) // caso registros de 4 bytes
    {
        uint32_t *registro = dictionary_get(contexto->registros_cpu, nombre_registro);
        uint32_t val = (uint32_t)*valor;
        *registro = val;
        free(val);
    }
    else if (strlen(nombre_registro) == 2) // caso registros de 1 bytes
    {
        uint8_t *registro = dictionary_get(contexto->registros_cpu, nombre_registro);
        uint8_t val = (uint8_t)*valor;
        *registro = val;
        free(val);
    }
}

void sum(char *nombre_destino, char *nombre_origen)
{
    uint32_t val_destino = obtener_valor_registro(contexto->registros_cpu, nombre_destino);
    uint32_t val_origen = obtener_valor_registro(contexto->registros_cpu, nombre_origen);
    val_destino += val_origen;
    set(nombre_destino, &val_destino);
}

void sub(char *nombre_destino, char *nombre_origen)
{
    uint32_t val_destino = obtener_valor_registro(contexto->registros_cpu, nombre_destino);
    uint32_t val_origen = obtener_valor_registro(contexto->registros_cpu, nombre_origen);
    val_destino -= val_origen;
    set(nombre_destino, &val_destino);
}

void jnz(char *nombre_registro, char *nro_instruccion)
{
    uint32_t val = obtener_valor_registro(contexto->registros_cpu, nombre_registro);
    if (val != 0)
    {
        set("PC", nro_instruccion);
    }
}

void io_gen_sleep(char *nombre, char *unidades)
{
    t_list *param = list_create();
    list_add(param, nombre);
    list_add(param, unidades);
    devolver_contexto(DESALOJO_IO_GEN_SLEEP, param);
}

void exit_inst()
{
    devolver_contexto(DESALOJO_EXIT, NULL);
}

void devolver_contexto(motivo_desalojo motivo_desalojo, t_list *param)
{
    t_paquete *paquete = crear_paquete(motivo_desalojo);
    agregar_a_paquete_contexto(paquete, contexto);

    for (int i = 0; i < list_size(param); i++)
    {
        agregar_a_paquete_string(paquete, (char *)list_get(param, i));
    }

    enviar_paquete(paquete, conexion_cpu_kernel_dispatch);
    eliminar_paquete(paquete);
}