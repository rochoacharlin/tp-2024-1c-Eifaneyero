#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>

int main(int argc, char *argv[])
{
    decir_hola("CPU");

    char *modulo = "cpu";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando CPU ...");

    return 0;
}
