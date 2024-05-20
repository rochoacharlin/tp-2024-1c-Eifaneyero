#include "interface_cpu.h"

void solicitar_lectura_de_instruccion(int conexion_memoria, uint32_t desplazamiento) // TODO: faltan implementar bien las fuciones
{
    t_solicitud_de_instruccion solicitud = crear_solicitud_de_instruccion();
    solicitud->desplazamiento;

    t_paquete paquete = crear_paquete();
    paquete->codigo_operacion = SOLICITUD_INSTRUCCION;
    // TODO: falta hacer tamanio_solicitud_de_instruccion, procesar_solicitud_instruccion
    paquete->buffer = serializar_estructura(solicitud, tamanio_solicitud_instruccion, serealizar_solicitud_instruccion);

    t_buffer heap = serializar_paquete(paquete);

    enviar_mensaje(conexion_memoria, paquete);

    destruir_solicitud_de_instruccion(solicitud);
    destruir_paquete(paquete);
}
