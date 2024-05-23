#include "interface_cpu.h"

void solicitar_lectura_de_instruccion(int conexion_memoria, uint32_t desplazamiento) // TODO: faltan implementar bien las fuciones
{
    t_solicitud_de_instruccion *solicitud = crear_solicitud_de_instruccion();
    t_paquete *paquete = crear_paquete(SOLICITUD_INSTRUCCION);
    // TODO: falta hacer tamanio_solicitud_de_instruccion, procesar_solicitud_instruccion
    // paquete->buffer = serializar_estructura(solicitud, tamanio_solicitud_instruccion, serealizar_solicitud_instruccion)

    serializar_solicitud_de_instruccion(solicitud, paquete);

    int tamanio_paquete = sizeof(op_code) + paquete->buffer->size;

    serializar_paquete(paquete, tamanio_paquete);

    enviar_paquete(paquete, conexion_memoria);

    destruir_solicitud_de_instruccion(solicitud);
    destruir_paquete(paquete);
}