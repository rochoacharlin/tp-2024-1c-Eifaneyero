#ifndef UTILS_HELLO_H_
#define UTILS_HELLO_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

/**
 * @fn    decir_hola
 * @brief Imprime un saludo al nombre que se pase por parámetro por consola.
 */

// Podriamos usar variables para la ip, puerto, etc.

void decir_hola(char *quien);
t_log *crear_logger(char *modulo);
#endif
