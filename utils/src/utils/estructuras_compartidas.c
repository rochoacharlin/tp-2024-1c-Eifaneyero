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
    agregar_a_paquete(paquete, solicitud, tamanio_solicitud_de_instruccion(solicitud));
}

void deserializar_solicitud_de_instruccion()
{
    // TODO
}

int tamanio_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud)
{
    return sizeof(int32_t); // por ahora
}