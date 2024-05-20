#include "utils/serializacion.h"

// ------------------------ CLIENTE ------------------------ //

void *serializar_paquete(t_paquete *paquete, int bytes)
{
    void *magic = malloc(bytes);
    int desplazamiento = 0;

    memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
    desplazamiento += paquete->buffer->size;

    return magic;
}

t_paquete *crear_paquete(void) // No debería recibir OpCode?
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = PAQUETE;
    crear_buffer(paquete);
    return paquete;
}

// lo que hacen las funciones crear buffer y crear paquete ya lo hace la funcion enviar mensaje
void crear_buffer(t_paquete *paquete)
{
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio)
{
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

    memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
    memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

    paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete *paquete, int socket)
{
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serializar_paquete(paquete, bytes);

    // debug ("Enviando paquete con tamaño %d, de %d bytes.", paquete->buffer->size, bytes);

    send(socket, a_enviar, bytes, 0);

    free(a_enviar);
}

void enviar_cod_op(op_code codigo_de_operacion, int socket) // What's it used for?
{
    void *buffer = malloc(sizeof(int));
    memcpy(buffer, &(codigo_de_operacion), sizeof(int));
    send(socket, buffer, sizeof(int), 0);
    free(buffer);
}

void enviar_mensaje(char *mensaje, int socket) // What's it used for?
{
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = MENSAJE;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

    int bytes = paquete->buffer->size + 2 * sizeof(int);

    void *a_enviar = serializar_paquete(paquete, bytes);

    send(socket, a_enviar, bytes, 0);

    free(a_enviar);
    eliminar_paquete(paquete);
}

void eliminar_paquete(t_paquete *paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}
// -------------------------

void buffer_agregar_uint32(t_paquete *paquete, uint32_t data)
{
    agregar_a_paquete(paquete, &data, sizeof(uint32_t));
}

void buffer_agregar_uint8(t_paquete *paquete, uint8_t data)
{
    agregar_a_paquete(buffer, &data, sizeof(uint8_t));
}

void buffer_agregar_string(t_paquete *paquete, uint32_t tamanio, char *string)
{
    buffer_agregar_uint32(paquete, tamanio);
    agregar_a_paquete(paquete, string, tamanio);
}

// ------------------------ SERVIDOR ------------------------ //

int recibir_operacion(int socket) // En base a operacion recibida -> efino como deserializar el buffer
{
    int cod_op;
    if (recv(socket, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        return cod_op;
    else
    {
        close(socket);
        return -1;
    }
}

void *recibir_buffer(int socket, int *size) //
{
    void *buffer;

    recv(socket, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket, buffer, *size, MSG_WAITALL);

    // debug("Recibido paquete con tamaño %d", *(size));
    return buffer;
}

char *recibir_mensaje(int socket) // What's it used for?
{
    int size;
    char *buffer = recibir_buffer(socket, &size);
    return buffer;
}

t_list *recibir_paquete(int socket)
{
    int size;
    int desplazamiento = 0;
    void *buffer;
    t_list *valores = list_create();
    int tamanio;

    buffer = recibir_buffer(socket, &size);
    while (desplazamiento < size)
    {
        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        char *valor = malloc(tamanio);
        memcpy(valor, buffer + desplazamiento, tamanio);
        desplazamiento += tamanio;
        list_add(valores, valor);
    }
    free(buffer);
    return valores;
}
