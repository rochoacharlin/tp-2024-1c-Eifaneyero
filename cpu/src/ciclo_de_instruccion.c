#include "ciclo_de_instruccion.h"

// pthread_mutex_t mutex_interrupt;
char *motivo_interrupcion;
bool continua_ejecucion = true;
bool hay_interrupcion = false;
// bool enviar_interrupcion = false;
t_contexto *contexto; // Diferencia e/ t_contexto* y t_contexto?

t_TLB *tlb; // TODO esta tlb se debe crear en otro lado (a pesar de que se usará en este archivo)

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

    list_destroy_and_destroy_elements(instruccion->direcciones_fisicas, free);
    free(instruccion);
}

void ciclo_de_instruccion(t_contexto *contexto_a_ejecutar)
{
    contexto = contexto_a_ejecutar;
    continua_ejecucion = true;
    hay_interrupcion = false;

    while (continua_ejecucion)
    {
        char *instruccion_leida = fetch();
        t_instruccion *instruccion = decode(instruccion_leida);

        free(instruccion_leida);

        execute(instruccion);
        check_interrupt(instruccion);

        destruir_instruccion(instruccion);
    }
}

// -------------------- FETCH -------------------- //

char *fetch()
{
    solicitar_lectura_de_instruccion(contexto->PID, obtener_valor_registro(contexto->registros_cpu, "PC"));
    char *instruccion_string = recibir_instruccion_string();
    loggear_fetch_instrucccion(contexto->PID, obtener_valor_registro(contexto->registros_cpu, "PC"));
    return instruccion_string;
}

char *recibir_instruccion_string()
{
    if (recibir_operacion(conexion_cpu_memoria) == INSTRUCCION)
    {
        char *instruccion_string = recibir_string(conexion_cpu_memoria);
        log_info(logger_propio, "String recibido de memoria: %s", instruccion_string);
        return instruccion_string;
    }
    else
    {
        log_info(logger_propio, "Codigo de operacion al recibir instruccion, incorrecto");
        return NULL;
    }
}

t_instruccion *convertir_string_a_instruccion(char *instruccion_string)
{
    t_instruccion *instruccion = malloc_or_die(sizeof(t_instruccion), "Error al asignar memoria para la instrucción");

    inicializar_instruccion(instruccion);

    char **instruccion_array = string_split(instruccion_string, " ");
    char *instruccion_array_fixed[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

    for (int i = 0; i < 6 && instruccion_array[i] != NULL; i++)
    {
        instruccion_array_fixed[i] = instruccion_array[i];
    }

    instruccion->id = string_id_to_enum_id(instruccion_array_fixed[0]);
    if (instruccion_array_fixed[1] != NULL)
        instruccion->param1 = string_duplicate(instruccion_array_fixed[1]);
    if (instruccion_array_fixed[2] != NULL)
        instruccion->param2 = string_duplicate(instruccion_array_fixed[2]);
    if (instruccion_array_fixed[3] != NULL)
        instruccion->param3 = string_duplicate(instruccion_array_fixed[3]);
    if (instruccion_array_fixed[4] != NULL)
        instruccion->param4 = string_duplicate(instruccion_array_fixed[4]);
    if (instruccion_array_fixed[5] != NULL)
        instruccion->param5 = string_duplicate(instruccion_array_fixed[5]);

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
    instruccion->direcciones_fisicas = list_create();
    return instruccion;
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

t_instruccion *decode(char *instruccion_leida)
{
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    instruccion = convertir_string_a_instruccion(instruccion_leida);

    // traducir direcciones lógicas si corresponde
    uint32_t direccion_logica = 0;
    int tamanio_a_operar = 0;

    switch (instruccion->id)
    {
    case IO_STDIN_READ:
    case IO_STDOUT_WRITE:
        direccion_logica = obtener_valor_registro(contexto->registros_cpu, instruccion->param2);
        tamanio_a_operar = obtener_valor_registro(contexto->registros_cpu, instruccion->param3);
        break;

    case MOV_IN:
        direccion_logica = obtener_valor_registro(contexto->registros_cpu, instruccion->param2);
        tamanio_a_operar = tamanio_de_registro(instruccion->param1);
        break;

    case MOV_OUT:
        direccion_logica = obtener_valor_registro(contexto->registros_cpu, instruccion->param1);
        tamanio_a_operar = tamanio_de_registro(instruccion->param2);
        break;

    default:
        break;
    }

    if (tamanio_a_operar > 0)
    {
        int pagina, desplazamiento, direccion_fisica = 0;
        pagina = floor(direccion_logica / tamanio_pagina);
        desplazamiento = direccion_logica - pagina * tamanio_pagina;
        direccion_fisica = calcular_direccion_fisica(tlb, contexto->PID, direccion_logica);
        list_add(instruccion->direcciones_fisicas, &direccion_fisica);
        int bytes_disponibles_en_marco = tamanio_pagina - desplazamiento;

        if (bytes_disponibles_en_marco < tamanio_a_operar)
        {
            direccion_logica += bytes_disponibles_en_marco;
            tamanio_a_operar -= bytes_disponibles_en_marco;
            int cantidad_pags_necesarias = (tamanio_a_operar + tamanio_pagina - 1) / tamanio_pagina;

            for (int i = 0; i < cantidad_pags_necesarias; i++)
            {
                direccion_fisica = calcular_direccion_fisica(tlb, contexto->PID, direccion_logica);
                list_add(instruccion->direcciones_fisicas, &direccion_fisica);
                direccion_logica += tamanio_pagina;
            }
        }
    }

    return instruccion;
}

uint8_t tamanio_de_registro(char *registro)
{
    if (strlen(registro) == 3 || !strcmp(registro, "SI") || !strcmp(registro, "DI") || !strcmp(registro, "PC")) // caso registros de 4 bytes
    {
        return 4;
    }

    else if (strlen(registro) == 2) // caso registros de 1 bytes
    {
        return 1;
    }

    return 0;
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
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: IO_GEN_SLEEP - <%s %s> ", contexto->PID, instruccion->param1, instruccion->param2);
        break;

    case IO_STDIN_READ:
        io_stdin_read(instruccion->param1, instruccion->direcciones_fisicas, instruccion->param3);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: IO_STDIN_READ - <%s %s %s> ", contexto->PID, instruccion->param1, instruccion->param2, instruccion->param3);
        break;

    case IO_STDOUT_WRITE:
        io_stdout_write(instruccion->param1, instruccion->direcciones_fisicas, instruccion->param3);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: IO_STDOUT_WRITE - <%s %s %s> ", contexto->PID, instruccion->param1, instruccion->param2, instruccion->param3);
        break;

    case MOV_IN:
        mov_in(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: MOV_IN - <%s %s> ", contexto->PID, instruccion->param1, instruccion->param2);
        break;

    case MOV_OUT:
        mov_out(instruccion->param1, instruccion->param2);
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: MOV_OUT - <%s %s> ", contexto->PID, instruccion->param1, instruccion->param2);
        break;

    case EXIT:
        exit_inst();
        log_info(logger_obligatorio, "PID: <%d> - Ejecutando: EXIT", contexto->PID);
        break;

    default:
        break;
    }

    // Incremento PC, al menos que ejecute SET PC XXX o JNZ 0 INSTRUCCION)
    if ((!(instruccion->id == SET && !strcmp(instruccion->param1, "PC")) && !(instruccion->id == JNZ && instruccion->param1 != 0)))
    {
        uint32_t valor = obtener_valor_registro(contexto->registros_cpu, "PC");
        valor++;
        char *valor_string = string_itoa(valor);
        set("PC", valor_string);
    }
}

bool instruccion_bloqueante(t_id id_instruccion)
{
    if (id_instruccion == IO_GEN_SLEEP || id_instruccion == EXIT) // TODO F: Agregar nuevas instrucciones bloqueantes
        return true;
    return false;
}

// -------------------- CHECK INTERRUPT -------------------- //

void check_interrupt(t_instruccion *instruccion)
{
    if (instruccion_bloqueante(instruccion->id))
    {
        continua_ejecucion = false;
        hay_interrupcion = false;
        return;
    }
    else if (hay_interrupcion)
    {

        motivo_desalojo motivo = string_interrupcion_to_enum_motivo(motivo_interrupcion);
        hay_interrupcion = false;

        devolver_contexto(motivo, NULL);
        continua_ejecucion = false;

        // TODO F: free(motivo); Cuando te libero? {Responsabilidad del kernel}
    }
    else
    {
        continua_ejecucion = false; // solo para hacer un test
        return;
    }
}

// -------------------- INSTRUCCIONES -------------------- //

void set(char *nombre_registro, char *valor)
{
    uint32_t valor_num = atoi(valor);
    if (strlen(nombre_registro) == 3 || !strcmp(nombre_registro, "SI") || !strcmp(nombre_registro, "DI") || !strcmp(nombre_registro, "PC")) // caso registros de 4 bytes
    {
        uint32_t *registro = dictionary_get(contexto->registros_cpu, nombre_registro);
        *registro = valor_num;
    }
    else if (strlen(nombre_registro) == 2) // caso registros de 1 bytes
    {
        uint8_t *registro = dictionary_get(contexto->registros_cpu, nombre_registro);
        *registro = (uint8_t)valor_num;
    }
}

void sum(char *nombre_destino, char *nombre_origen)
{
    uint32_t val_destino = obtener_valor_registro(contexto->registros_cpu, nombre_destino);
    uint32_t val_origen = obtener_valor_registro(contexto->registros_cpu, nombre_origen);
    val_destino += val_origen;
    set(nombre_destino, string_itoa(val_destino));
}

void sub(char *nombre_destino, char *nombre_origen)
{
    uint32_t val_destino = obtener_valor_registro(contexto->registros_cpu, nombre_destino);
    uint32_t val_origen = obtener_valor_registro(contexto->registros_cpu, nombre_origen);
    val_destino -= val_origen;
    set(nombre_destino, string_itoa(val_destino));
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
    list_add(param, string_itoa(IO_GEN_SLEEP));
    list_add(param, unidades);
    devolver_contexto(DESALOJO_IO, param);
}

void io_stdin_read(char *nombre, t_list *direcciones_fisicas, char *registro_tamanio)
{
    t_list *param = list_create();
    list_add(param, nombre);
    list_add(param, string_itoa(IO_STDIN_READ));
    list_add(param, string_itoa(obtener_valor_registro(contexto->registros_cpu, registro_tamanio)));
    for (int i = 0; i < list_size(direcciones_fisicas); i++)
        list_add(param, list_get(direcciones_fisicas, i));

    devolver_contexto(DESALOJO_IO, param);
}

void io_stdout_write(char *nombre, t_list *direcciones_fisicas, char *registro_tamanio)
{
    t_list *param = list_create();
    list_add(param, nombre);
    list_add(param, string_itoa(IO_STDOUT_WRITE));
    list_add(param, string_itoa(obtener_valor_registro(contexto->registros_cpu, registro_tamanio)));
    for (int i = 0; i < list_size(direcciones_fisicas); i++)
        list_add(param, list_get(direcciones_fisicas, i));

    devolver_contexto(DESALOJO_IO, param);
}

void mov_in(char *registro_datos_destino, char *registro_con_direccion_logica)
{
    // Ro, como defino el tipo de dato de direccion lógica? 8 o 32? Me explicassss? Creo que tengo ese problema en toda la funcion.
    uint32_t direccion_logica = obtener_valor_registro(contexto->registros_cpu, registro_con_direccion_logica);
    int direccion_fisica = calcular_direccion_fisica(tlb, contexto->PID, direccion_logica);

    t_paquete *paquete_direccion = crear_paquete(ACCESO_ESPACIO_USUARIO_LECTURA);
    agregar_a_paquete_uint32(paquete_direccion, direccion_fisica);
    enviar_paquete(paquete_direccion, conexion_cpu_memoria);
    eliminar_paquete(paquete_direccion);

    if (recibir_operacion(conexion_cpu_memoria) == OK)
    {
        t_list *paquete_valor = recibir_paquete(conexion_cpu_memoria);
        uint32_t valor = *(uint32_t *)list_get(paquete_valor, 0);

        set(registro_datos_destino, string_itoa(valor));
        list_destroy_and_destroy_elements(paquete_valor, free);
    }
    else
    {
        log_info(logger_propio, "Conflicto en mov_in"); // Temp
    }
}

void mov_out(char *registro_con_direccion_destino, char *registro_datos)
{
    // Ro, como defino el tipo de dato de direccion lógica? 8 o 32? Me explicassss? Creo que tengo ese problema en toda la funcion.
    uint32_t valor = obtener_valor_registro(contexto->registros_cpu, registro_datos);
    uint32_t direccion_logica = obtener_valor_registro(contexto->registros_cpu, registro_con_direccion_destino);
    int direccion_fisica = calcular_direccion_fisica(tlb, contexto->PID, direccion_logica);

    // Envio PID, dir fisica y valor a escribir.
    t_paquete *paquete_direccion = crear_paquete(ACCESO_ESPACIO_USUARIO_LECTURA);
    agregar_a_paquete_uint32(paquete_direccion, contexto->PID);
    agregar_a_paquete_uint32(paquete_direccion, direccion_fisica);
    agregar_a_paquete_uint32(paquete_direccion, valor);
    enviar_paquete(paquete_direccion, conexion_cpu_memoria);
    eliminar_paquete(paquete_direccion);

    if (recibir_operacion(conexion_cpu_memoria) == OK)
    {
        log_info(logger_propio, "Mov_out ejecutado correctamente");
    }
    else
    {
        log_info(logger_propio, "Conflicto en mov_out"); // Temp
    }
}

void exit_inst()
{
    devolver_contexto(DESALOJO_EXIT_SUCCESS, NULL);
}

void devolver_contexto(motivo_desalojo motivo_desalojo, t_list *param)
{
    t_paquete *paquete = crear_paquete(motivo_desalojo);
    agregar_a_paquete_contexto(paquete, contexto);

    if (param != NULL)
    {
        for (int i = 0; i < list_size(param); i++)
        {
            agregar_a_paquete_string(paquete, (char *)list_get(param, i));
        }
    }
    enviar_paquete(paquete, conexion_cpu_kernel_dispatch);
    eliminar_paquete(paquete);
}
