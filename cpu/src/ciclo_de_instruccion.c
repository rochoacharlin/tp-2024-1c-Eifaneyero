#include <ciclo_de_instruccion.h>

pthread_mutex_t mutex_interrupt;
char *motivo_interrupcion;
bool continua_ejecucion = true;
bool hay_interrupcion = false;
bool enviar_interrupcion = false;

void ciclo_de_instruccion(t_contexto contexto)
{
    continua_ejecucion = true;
    hay_interrupcion = false;

    while (continua_ejecucion)
    {
        t_instruccion *instruccion = fetch(contexto);
        decode(instruccion); // No hace nada, por ahora
        execute(instruccion);
        check_interrupt(contexto);
    }
}

// -------------------- FETCH -------------------- //

t_instruccion *fetch(t_contexto contexto)
{
    t_instruccion *instruccion_recibida = malloc(sizeof(t_instruccion));
    uint32_t desplazamiento = obtener_valor_registro(contexto->registros_cpu, "PC"); // TODO: Desplazamiento real?

    solicitar_lectura_instruccion_memoria(conexion_cpu_memoria, desplazamiento);

    char *instruccion_string = recibir_instruccion_string_memoria(conexion_cpu_memoria); // TODO: Entiendo que me va a llegar un paquete: int + string

    t_instruccion *instruccion_recibida = convertir_string_a_instruccion(instruccion_string);

    free(instruccion_string);
    loggear_fetch_instrucccion(registros_cpu->PID, obtener_valor_registro(registros_cpu, "PC"));
    return instruccion_recibida;
}

void solicitar_instruccion_memoria(int socket, uint64_t desplazamiento)
{
    // TODO
}

char *recibir_instruccion_string_memoria(conexion_cpu_memoria)
{
    // TODO
    char *instruccion_string;
    return instruccion_string;
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
        instruccion->param1 = instruccion_array[1];
    if (instruccion_array[2] != NULL)
        instruccion->param2 = instruccion_array[2];
    if (instruccion_array[3] != NULL)
        instruccion->param3 = instruccion_array[3];
    if (instruccion_array[4] != NULL)
        instruccion->param4 = instruccion_array[4];
    if (instruccion_array[5] != NULL)
        instruccion->param5 = instruccion_array[5];

    // Version 2.0 (instruccion->parametros[]):
    // int i = 1;
    // while(instruccion_array[i] != NULL) {
    //     instruccion->param[i] = instruccion_array[i];
    //     i++;
    // }

    return instruccion;
}

// Setea miembros en NULL
t_instruccion *inicializar_instruccion(t_instruccion *instruccion)
{
    instruccion->param1 = NULL;
    instruccion->param2 = NULL;
    instruccion->param3 = NULL;
    instruccion->param4 = NULL;
    instruccion->param5 = NULL;
    return instruccion;
}

// Recibo instruccion en string y casteo a enum para switch.
// Si llega una instruccion desconocida sale por EXIT.
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
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: SET - <%s %s>", contexto_ejecucion->PID, instruccion->param1, instruccion->param2);
        break;

    case SUM:
        sum(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: SUM - <%s %s>", contexto_ejecucion->PID, instruccion->param1, instruccion->param2);
        break;

    case SUB:
        sub(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: SUB - <%s %s>", contexto_ejecucion->PID, instruccion->param1, instruccion->param2);
        break;

    case JNZ:
        jnz(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: JNZ - <%s %s>", contexto_ejecucion->PID, instruccion->param1, instruccion->param2);
        break;

    case IO_GEN_SLEEP:
        io_gen_sleep(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: SLEEP - <%s %s> ", contexto_ejecucion->PID, instruccion->param1, instruccion->param2);
        break;

        // case EXIT:
        //     //exit_instruccion(): Enviar contexto de ejecucion a Kernel
        //     log_info(logger_obligatorio, "PID: <%d> - Ejecutando: EXIT", contexto_ejecucion->PID);
        //     continua_ejecucion = 0;
        //     break;
        // default:
        //     break;
    }

    // Incremento PC, al menos que ejecute SET PC XXX o JNZ 0 INSTRUCCION)
    if (!(instruccion->id == SET && instruccion->param1 == "PC") && !(instruccion->id == JNZ && instruccion->param1 != 0))
    {
        uint32_t valor = obtener_valor_registro(registros_cpu, "PC"); // TODO: Donde, como, cuando se crean los registros?
        valor++;
        set("PC", &valor);
    }
}

bool instruccion_bloqueante(t_id id_instruccion)
{
    if (id_instruccion == IO_GEN_SLEEP)
        return true;
    return false;
}

// -------------------- CHECK INTERRUPT -------------------- //

void check_interrupt(t_contexto *contexto)
{
    pthread_mutex_lock(&mutex_interrupt);

    if (hay_interrupcion)
    {
        enviar_interrupcion = true;
        char *motivo = malloc(sizeof(char) * 14) //"matar_proceso\0" o "planificacion\0"
            *motivo = *motivo_interrupcion;      // Por si llega ora interrupcion y cambia el motivo
        hay_interrupcion = false;
    }

    pthread_mutex_unlock(&mutex_interrupt);

    // Si hay interrupcion, envio contexto y no continua ejecución. Se reinicia ciclo con nuevo contexto.
    if (enviar_interrupcion)
    {
        t_list *lista_null = NULL;                       // Solo para pasar a devolver contexto
        devolver_contexto(contexto, motivo, lista_null); // TODO: No llega el motivo a acá. Mierda
        continua_ejecucion = false;
        enviar_interrupcion = false;
        // free(string); Cuando te libero?
    }

    // TODO: @R8A Interpreto que ya devolviste el contexto en la ejecucion de la instruccion?
    // Quien tiene prioridad Syscall o Interrupcion? :|
    if (instruccion_bloqueante(instruccion->id))
        continua_ejecucion = false;
}

char *recibir_interrupcion() // TODO: Chequear y poner donde vaya.
{
    if (recibir_operacion(conexion_cpu_kernel_interrupt) == INTERRUPCION) //
    {
        int *size = malloc(sizeof(int));
        recv(conexion_cpu_kernel_interrupt, size, sizeof(int), MSG_WAITALL);
        void *buffer = malloc(*size);
        recv(conexion_cpu_kernel_interrupt, buffer, *size, MSG_WAITALL); //"matar_proceso\0" o "planificacion\0"
        free(size);
        return (char *)buffer;
    }
    else
    {
        // No hay else
    }
}

// -------------------- INSTRUCCIONES -------------------- //

void set(char *nombre_registro, void *valor)
{
    if (strlen(nombre_registro) == 3 || !strcmp(nombre_registro, "SI") || !strcmp(nombre_registro, "DI") || !strcmp(nombre_registro, "PC")) // caso registros de 4 bytes
    {
        uint32_t *registro = dictionary_get(registros_cpu, nombre_registro);
        uint32_t *val = (uint32_t *)valor;
        *registro = *val;
        free(val);
    }
    else if (strlen(nombre_registro) == 2) // caso registros de 1 bytes
    {
        uint8_t *registro = dictionary_get(registros_cpu, nombre_registro);
        uint8_t *val = (uint8_t *)valor;
        *registro = *val;
        free(val);
    }
}

void sum(char *nombre_destino, char *nombre_origen)
{
    uint32_t val_destino = obtener_valor_registro(registros_cpu, nombre_destino);
    uint32_t val_origen = obtener_valor_registro(registros_cpu, nombre_origen);
    val_destino += val_origen;
    set(nombre_destino, &val_destino);
}

void sub(char *nombre_destino, char *nombre_origen)
{
    uint32_t val_destino = obtener_valor_registro(registros_cpu, nombre_destino);
    uint32_t val_origen = obtener_valor_registro(registros_cpu, nombre_origen);
    val_destino -= val_origen;
    set(nombre_destino, &val_destino);
}

void jnz(char *nombre_registro, void *nro_instruccion)
{
    uint32_t val = obtener_valor_registro(registros_cpu, nombre_registro);
    if (val != 0)
    {
        set("PC", nro_instruccion);
    }
}

void io_gen_sleep(char *nombre, char *unidades)
{
    t_list *param = create_list();
    list_add(param, unidades);
    // devolver_contexto(nombre, motivo, param); //TODO: Inconsistencia de parametros.
    // La instruccion no tiene el contexto. Lo hacemos global? Iba acá devolver contexto? Ya estoy quemado.
}

void devolver_contexto(t_contexto *contexto, char *motivo_desalojo, t_list *param)
{
    t_id operacion = string_id_to_enum_id(motivo_desalojo);
    t_paquete paquete = crear_paquete(operacion);
    agregar_contexto_a_paquete(contexto, paquete);

    for (int i = 0; i < list_size(param); i++)
    {
        agregar_string_a_paquete(paquete, list_get(param, i)); // TODO: agregar_string_a_paquete(t_paquete, char*)
    }

    enviar_paquete(paquete, conexion_cpu_kernel_dispatch);
    destruir_paquete(paquete);
    destruir_contexto(contexto);
}

// TODO: POSIBLE ERROR. Qué puede ser? Enviar contexto a kernel -> terminar proceeso
