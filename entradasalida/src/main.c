#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>

int main(int argc, char *argv[])
{
    decir_hola("una Interfaz de Entrada/Salida");

    char *modulo = "E/S";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Interfaz de Entrada/Salida...");

    return 0;
}
