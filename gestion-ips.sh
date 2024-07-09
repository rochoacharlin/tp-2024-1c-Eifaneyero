#!/bin/bash

archivos=($(find ./ -name '*.config'))

modulos=(MEMORIA CPU KERNEL)

echo "Ingresar IP de la memoria: "
read IP[0]

echo "Ingresar IP del CPU: "
read IP[1]

echo "Ingresar IP del Kernel: "
read IP[2]

for i in ${archivos[@]};
    do for j in {0..2};
        do sed -i "s/IP_${modulos[$j]}=[a-z0-9\.]*/IP_${modulos[$j]}=${IP[$j]}/" "$i";
    done;
done;
