#include <ciclo_de_instruccion.h>

pthread_mutex_t mutex_interrupt;
char *motivo_interrupcion;
bool continua_ejecucion = true;
bool hay_interrupcion = false;
// bool enviar_interrupcion = false;
t_contexto *contexto;

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
        check_interrupt(instruccion); // TODO F: id = EXIT, no se chequean interrupciones. Si no me desago de la interrupción puede cortar al proceso que sigue?

        destruir_instruccion(instruccion);
    }
}

// -------------------- FETCH -------------------- //

t_instruccion *fetch()
{
    t_instruccion *instruccion_recibida = malloc(sizeof(t_instruccion));
    uint32_t desplazamiento = obtener_valor_registro(contexto->registros_cpu, "PC"); // TODO: Desplazamiento real?

    solicitar_lectura_de_instruccion(conexion_cpu_memoria, desplazamiento);

    char *instruccion_string = recibir_instruccion_string_memoria(conexion_cpu_memoria); // TODO F: Adaptar a estructura que envia Nehuen.

    t_instruccion *instruccion_recibida = convertir_string_a_instruccion(instruccion_string);

    free(instruccion_string);
    loggear_fetch_instrucccion(contexto->PID, obtener_valor_registro(contexto->registros_cpu, "PC"));
    return instruccion_recibida;
}

char *recibir_instruccion_string_memoria(conexion_cpu_memoria) // TODO F
{
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
        exit();
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: EXIT", contexto->PID);
        break;
    default:
        break;
    }

    // Incremento PC, al menos que ejecute SET PC XXX o JNZ 0 INSTRUCCION) - ¿o que la instruccion sea exit?
    if ((!(instruccion->id == SET && instruccion->param1 == "PC") && !(instruccion->id == JNZ && instruccion->param1 != 0) && !(instruccion->id == EXIT)))
    {
        uint32_t valor = obtener_valor_registro(contexto->registros_cpu, "PC");
        valor++;
        set("PC", &valor);
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
    if (!instruccion_bloqueante(instruccion))
    {
        continua_ejecucion = false;
        hay_interrupcion = false; // Desacarto interrupcion para que no afecte otro proceso. La maneja el kernel?
        return;
    }
    else if (hay_interrupcion)
    {

        char *motivo = malloc(sizeof(char) * 14) //"matar_proceso\0" o "planificacion\0"
            *motivo = *motivo_interrupcion;
        hay_interrupcion = false;

        devolver_contexto(contexto, motivo, NULL);
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
        recv(conexion_cpu_kernel_interrupt, buffer, *size, MSG_WAITALL); // matar_proceso\0 o planificacion\0 ?
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
        uint32_t *registro = dictionary_get(contexto->registros_cpu, nombre_registro);
        uint32_t *val = (uint32_t *)valor;
        *registro = *val;
        free(val);
    }
    else if (strlen(nombre_registro) == 2) // caso registros de 1 bytes
    {
        uint8_t *registro = dictionary_get(contexto->registros_cpu, nombre_registro);
        uint8_t *val = (uint8_t *)valor;
        *registro = *val;
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

void jnz(char *nombre_registro, void *nro_instruccion)
{
    uint32_t val = obtener_valor_registro(contexto->registros_cpu, nombre_registro);
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
}

void exit()
{
    devolver_contexto(contexto, EXIT, NULL);
}

void devolver_contexto(char *motivo_desalojo, t_list *param)
{
    t_id operacion = string_id_to_enum_id(motivo_desalojo);
    t_paquete paquete = crear_paquete(operacion);
    agregar_contexto_a_paquete(contexto, paquete);
    // agregar_motivo_a_paquete()? op_code
    for (int i = 0; i < list_size(param); i++)
    {
        agregar_string_a_paquete(paquete, list_get(param, i)); // TODO: agregar_string_a_paquete(t_paquete, char*)
    }

    enviar_paquete(paquete, conexion_cpu_kernel_dispatch);
    destruir_paquete(paquete);
    destruir_contexto(contexto);
}

//
// TODO: POSIBLE ERROR. Qué puede ser? Enviar contexto a kernel -> terminar proceeso
//
