#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>

int main(int argc, char *argv[])
{
    decir_hola("Memoria");

    char *modulo = "Memoria";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Memoria ...");

    return 0;
}
