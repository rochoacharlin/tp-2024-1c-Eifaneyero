#!/bin/bash

# Encontrar todos los archivos .config en el directorio actual y sus subdirectorios
archivos=($(find ./ -name '*.config'))

# Definir los módulos
modulos=("MEMORIA" "CPU" "KERNEL")

# Inicializar arrays para IPs y Puertos
declare -A IPs
declare -A Puertos

# Capturar IPs y Puertos del usuario
for modulo in "${modulos[@]}"; do
    echo "Ingresar IP de la ${modulo}: "
    read IPs["${modulo}"]

    if [ "$modulo" == "CPU" ]; then
        echo "Ingresar Puerto INTERRUPT de la ${modulo}: "
        read Puertos["${modulo}_INTERRUPT"]

        echo "Ingresar Puerto DISPATCH de la ${modulo}: "
        read Puertos["${modulo}_DISPATCH"]
    else
        echo "Ingresar Puerto de la ${modulo}: "
        read Puertos["${modulo}"]
    fi
done

# Reemplazar IPs y Puertos en los archivos .config
for archivo in "${archivos[@]}"; do
    for modulo in "${modulos[@]}"; do
        sed -i "s/IP_${modulo}=[a-z0-9\.]*/IP_${modulo}=${IPs[${modulo}]}/" "$archivo"
        if [ "$modulo" == "CPU" ]; then
            sed -i "s/PUERTO_${modulo}_INTERRUPT=[a-z0-9\.]*/PUERTO_${modulo}_INTERRUPT=${Puertos[${modulo}_INTERRUPT]}/" "$archivo"
            sed -i "s/PUERTO_${modulo}_DISPATCH=[a-z0-9\.]*/PUERTO_${modulo}_DISPATCH=${Puertos[${modulo}_DISPATCH]}/" "$archivo"
        else
            sed -i "s/PUERTO_${modulo}=[a-z0-9\.]*/PUERTO_${modulo}=${Puertos[${modulo}]}/" "$archivo"
        fi
    done
done