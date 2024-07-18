#!/bin/bash

# Encontrar todos los archivos .config en el directorio actual y sus subdirectorios
archivos=($(find ./ -name '*.config'))

# Definir los módulos
modulos=("MEMORIA" "CPU" "KERNEL")

# Inicializar arrays para IPs y Puertos
declare -A IPs

# Capturar IPs y Puertos del usuario
for modulo in "${modulos[@]}"; do
    echo "Ingresar IP de la ${modulo}: "
    read IPs["${modulo}"]
done

# Reemplazar IPs y Puertos en los archivos .config
for archivo in "${archivos[@]}"; do
    for modulo in "${modulos[@]}"; do
        sed -i "s/IP_${modulo}=[a-z0-9\.]*/IP_${modulo}=${IPs[${modulo}]}/" "$archivo"
    done
done

