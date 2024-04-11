#include <stdlib.h>
#include <stdio.h>
// #include <utils/hello.h>
#include <utils/hello.c>

int main(int argc, char *argv[])
{
    decir_hola("Kernel");

    char *modulo = "kernel";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Kernel ...");

    return 0;
}
