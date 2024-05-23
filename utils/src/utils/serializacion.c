#include "serializacion.h"

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

    return magic; // paquete serializado: OpCode + tamaño + stream
}

t_paquete *crear_paquete(op_code codigo_operacion)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = codigo_operacion;
    crear_buffer(paquete);
    return paquete;
}

void crear_buffer(t_paquete *paquete)
{
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio)
{
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(int) + tamanio); // Tamaño previo + int que contendrá tamañoDeDatoNuevo + tamanioDatoNuevo

    memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));        // Agrego tamaño del dato nuevo al paquete
    memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio); // Agrego dato nuevo al paquete

    paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete *paquete, int socket)
{
    int bytes = paquete->buffer->size + 2 * sizeof(int); // tamaño del stream + int con streamSize + opCode
    void *a_enviar = serializar_paquete(paquete, bytes);

    send(socket, a_enviar, bytes, 0);

    free(a_enviar);
}

void enviar_cod_op(op_code codigo_de_operacion, int socket)
{
    void *buffer = malloc(sizeof(int));
    memcpy(buffer, &(codigo_de_operacion), sizeof(int));
    send(socket, buffer, sizeof(int), 0);
    free(buffer);
}
// Crea buffer y crear paquete previo al envio
void enviar_mensaje(char *mensaje, int socket) // ?
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
// ------------------------ Funciones por tipo de dato ------------------------ //

void agregar_a_paquete_uint32(t_paquete *paquete, uint32_t data)
{
    agregar_a_paquete(paquete, &data, sizeof(uint32_t));
}

void agregar_a_paquete_uint8(t_paquete *paquete, uint8_t data)
{
    agregar_a_paquete(paquete, &data, sizeof(uint8_t));
}

// ------------------------ SERVIDOR ------------------------ //

int recibir_operacion(int socket) // En base a operacion recibida -> defino como deserializar el buffer
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

// Paso puntero a entero (size) para guardar tamaño del buffer (usado para deserializar)
void *recibir_buffer(int socket, int *size) //
{
    void *buffer;

    recv(socket, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket, buffer, *size, MSG_WAITALL);

    return buffer;
}

char *recibir_mensaje(int socket) // ?
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
