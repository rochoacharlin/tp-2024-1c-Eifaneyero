#include "estructuras_compartidas.h"

// REGISTROS DE CPU

t_dictionary *crear_registros_cpu()
{
    t_dictionary *registros_cpu = dictionary_create();
    if (!dictionary_size(registros_cpu))
    {
        dictionary_put(registros_cpu, "PC", memset(malloc(sizeof(uint32_t *)), 0, 4));
        dictionary_put(registros_cpu, "AX", memset(malloc(sizeof(uint8_t *)), 0, 1));
        dictionary_put(registros_cpu, "BX", memset(malloc(sizeof(uint8_t *)), 0, 1));
        dictionary_put(registros_cpu, "CX", memset(malloc(sizeof(uint8_t *)), 0, 1));
        dictionary_put(registros_cpu, "DX", memset(malloc(sizeof(uint8_t *)), 0, 1));
        dictionary_put(registros_cpu, "EAX", memset(malloc(sizeof(uint32_t *)), 0, 4));
        dictionary_put(registros_cpu, "EBX", memset(malloc(sizeof(uint32_t *)), 0, 4));
        dictionary_put(registros_cpu, "ECX", memset(malloc(sizeof(uint32_t *)), 0, 4));
        dictionary_put(registros_cpu, "EDX", memset(malloc(sizeof(uint32_t *)), 0, 4));
        dictionary_put(registros_cpu, "SI", memset(malloc(sizeof(uint32_t *)), 0, 4));
        dictionary_put(registros_cpu, "DI", memset(malloc(sizeof(uint32_t *)), 0, 4));
    }

    return registros_cpu;
}

uint32_t obtener_valor_registro(t_dictionary *registros_cpu, char *nombre_registro)
{
    uint32_t valor;

    if (strlen(nombre_registro) == 3 || !strcmp(nombre_registro, "SI") || !strcmp(nombre_registro, "DI") || !strcmp(nombre_registro, "PC")) // caso registros de 4 bytes
    {
        uint32_t *registro = dictionary_get(registros_cpu, nombre_registro);
        valor = *registro;
    }

    else if (strlen(nombre_registro) == 2) // caso registros de 1 bytes
    {
        uint8_t *registro = dictionary_get(registros_cpu, nombre_registro);
        valor = *registro;
    }

    return valor;
}

void destruir_registros_cpu(t_dictionary *registros_cpu)
{
    dictionary_destroy_and_destroy_elements(registros_cpu, free);
}

t_dictionary *copiar_registros_cpu(t_dictionary *a_copiar)
{
    t_dictionary *copia = dictionary_create();

    const char *keys[] = {"PC", "AX", "BX", "CX", "DX", "EAX", "EBX", "ECX", "EDX", "SI", "DI"}; // Lista de claves y sus tamaños correspondientes
    const size_t sizes[] = {sizeof(uint32_t), sizeof(uint8_t), sizeof(uint8_t), sizeof(uint8_t), sizeof(uint8_t), sizeof(uint32_t),
                            sizeof(uint32_t), sizeof(uint32_t), sizeof(uint32_t), sizeof(uint32_t), sizeof(uint32_t)};
    const size_t num_keys = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < num_keys; i++) // Itero sobre cada clave para copiar los valores
    {
        void *value = dictionary_get(a_copiar, keys[i]);
        void *value_copy = malloc(sizes[i]);
        memcpy(value_copy, value, sizes[i]);
        dictionary_put(copia, keys[i], value_copy);
    }

    return copia;
}

// PROVISORIO solicitud_de_instruccion

t_solicitud_de_instruccion *crear_solicitud_de_instruccion()
{

    t_solicitud_de_instruccion *solicitud_de_instruccion = malloc(sizeof(int32_t));
    if (solicitud_de_instruccion != NULL)
    {
        solicitud_de_instruccion->desplazamiento = 0;
    }
    return solicitud_de_instruccion;
}

void destruir_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud_de_instruccion)
{
    free(solicitud_de_instruccion);
}

void serializar_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud, t_paquete *paquete)
{
    int tamanio_solicitud = tamanio_solicitud_de_instruccion(solicitud);

    agregar_a_paquete(paquete, &(solicitud->desplazamiento), tamanio_solicitud);
}

void generar_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud, t_list *valores)
{
    // Obtener el desplazamiento del paquete
    solicitud->desplazamiento = *(int32_t *)(valores->head->data);
}

int tamanio_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud)
{
    return sizeof(int32_t); // por ahora
}

// PROVISORIO INSTRUCCIONES

t_instruccion_cadena *crear_instruccion()
{
    t_instruccion_cadena *instruccion = malloc(sizeof(char *));
    if (instruccion != NULL)
    {
        instruccion->instruccion = NULL;
    }
    instruccion->instruccion = NULL; // Inicialmente no hay instrucción
    return instruccion;
}

void destruir_instruccion(t_instruccion_cadena *instruccion)
{
    if (instruccion->instruccion != NULL)
    {
        free(instruccion->instruccion);
    }
    free(instruccion);
}

void serializar_instruccion(t_instruccion_cadena *instruccion, t_paquete *paquete)
{
    if (instruccion->instruccion == NULL)
    {
        // Manejo del error
        fprintf(stderr, "Error: La instrucción no está inicializada.\n");
        return;
    }

    uint32_t longitud_instruccion = strlen(instruccion->instruccion) + 1; // +1 para incluir el null terminator
    agregar_a_paquete(paquete, instruccion->instruccion, longitud_instruccion);
}

void generar_instruccion(t_instruccion_cadena *instruccion, t_list *valores)
{
    // Calcular el tamaño total necesario para la instrucción
    int longitud_total = 0;
    for (int i = 0; i < list_size(valores); i++)
    {
        char *valor = list_get(valores, i);
        longitud_total += strlen(valor) + 1; // +1 para el espacio o el terminador nulo
    }

    // Reservar memoria para la instrucción
    instruccion->instruccion = string_new();

    // Concatenar los valores en una sola cadena
    for (int i = 0; i < list_size(valores); i++)
    {
        char *valor = list_get(valores, i);
        strcat(instruccion->instruccion, valor);
        if (i < list_size(valores) - 1)
        {
            strcat(instruccion->instruccion, " "); // Agregar espacio entre valores
        }
    }
}

int tamanio_instruccion(t_instruccion_cadena *instruccion)
{
    return sizeof(uint32_t) + strlen(instruccion->instruccion) + 1; // +1 para incluir el null terminator
}
