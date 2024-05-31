#include "registros_cpu.h"

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
