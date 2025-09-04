# Informe Tarea 0: Instalación xv6
Alfredo Hernández
Ignacio Hernández
## Pasos Seguidos

1. Clonación del repositorio: Se clonó mi fork desde GitHub
2. Creación de rama: Se creo la rama `a1fredowo`
3. Instalación de dependencias: Se instalaron las dependencias las herramientas necesarias según mi SO (en este caso Ubuntu con WSL)
4. Compilación: Se compiló con `make` sin errores
5. Ejecución: Se ejecutó xv6 con `make qemu`

## Problemas Encontrados y Soluciones
El principal problema encontrado fue un error de compatibilidad de versiones: `ERROR: Need qemu version >= 7.2`, ya que mi sistema WSL tenía instalado QEMU 6.2.0. Se intento corregir utilizando varias soluciones incluyendo actualización de repositorios y uso de snap, pero ninguna funcionó en el entorno WSL. Finalmente, se solucionó el problema compilando QEMU 8.1.0 desde el código fuente, descargando el tarball oficial, instalando las dependencias de desarrollo necesarias (build-essential, zlib1g-dev, libglib2.0-dev, etc.), y configurando la compilación específicamente para el target RISC-V. Después de la compilación e instalación manual, QEMU funcionó correctamente y pude proceder con la compilación y ejecución de xv6. También encontré un error menor con el comando `cat README` que mostró "exec cDME failed", pero tras buscar, pude verificar que esto es normal en xv6 y no afecta el funcionamiento general del sistema.

## Confirmación de funcionamiento
![Consola en Ubuntu](https://cdn.discordapp.com/attachments/779582681193250817/1412919053420593243/image.png?ex=68ba0ae9&is=68b8b969&hm=315fd9fc6d99746baf11d807e73e9fb35ab98348ca21c5c0ef6c6b2dedd34e32)

## Información del Sistema

- SO: Ubuntu 22.04.5 LTS con WSL
- Versión de QEMU: QEMU emulator version 8.1.0
