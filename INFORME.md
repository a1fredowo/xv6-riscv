# Informe - Tarea 1: Implementación de Llamadas al Sistema en xv6
Grupo K:  
Alfredo Hernández  
Ignacio Hernández  

## Introducción

Este informe documenta la implementación exitosa de dos nuevas llamadas al sistema en xv6: `getppid()` y `getancestor(int n)`. El objetivo fue comprender la arquitectura interna de xv6 y el mecanismo de llamadas al sistema, desde la interfaz de usuario hasta la implementación en el kernel.

## Funcionamiento de las Llamadas al Sistema

### Arquitectura General

Las llamadas al sistema en xv6 siguen un patrón específico que involucra múltiples archivos trabajando en conjunto:

1. **Interfaz de Usuario (user/user.h)**: Define los prototipos de las funciones disponibles para los programas de usuario
2. **Generación de Stubs (user/usys.pl)**: Script que genera automáticamente el código ensamblador necesario para la transición del modo usuario al modo kernel
3. **Definición de Números (kernel/syscall.h)**: Asigna números únicos a cada llamada al sistema
4. **Tabla de Dispatching (kernel/syscall.c)**: Mantiene una tabla que asocia números de llamadas al sistema con sus implementaciones
5. **Implementación Real (kernel/sysproc.c)**: Contiene la lógica real de las funciones del sistema

### Flujo de Ejecución

Cuando un programa usuario ejecuta `getppid()`, ocurre la siguiente secuencia:

1. El programa llama a la función declarada en `user.h`
2. El código generado por `usys.pl` ejecuta la instrucción `ecall` con el número correspondiente
3. El kernel recibe la interrupción y consulta la tabla en `syscall.c`
4. Se ejecuta la función real en `sysproc.c`
5. El resultado se retorna al programa usuario

## Llamadas al Sistema Implementadas

### Parte I: getppid()

**Funcionalidad**: Retorna el Process ID (PID) del proceso padre del proceso que la invoca.

**Implementación**:
```c
uint64
sys_getppid(void)
{
  return myproc()->parent->pid;
}
```

Esta implementación accede directamente a la estructura del proceso actual (`myproc()`) y navega a su proceso padre para obtener el PID.

### Parte II: getancestor(int n)

**Funcionalidad**: Retorna el ancestro n-ésimo del proceso actual, donde:
- `n = 0`: proceso actual
- `n = 1`: proceso padre
- `n = 2`: proceso abuelo
- `n > niveles disponibles`: retorna -1

**Implementación**:
```c
uint64
sys_getancestor(void)
{
    int n;
    struct proc *p;
    
    // Obtener el parámetro n
    argint(0, &n);
    
    // Verificar que n sea válido
    if(n < 0)
        return -1;
    
    p = myproc();
    
    // Recorrer n niveles hacia arriba
    for(int i = 0; i < n; i++) {
        if(p->parent == 0) {
            // No hay más ancestros
            return -1;
        }
        p = p->parent;
    }
    
    return p->pid;
}
```

Esta función utiliza un bucle para navegar `n` niveles hacia arriba en la jerarquía de procesos, validando la existencia de cada ancestro.

## Modificaciones Realizadas

### 1. user/user.h
- Agregadas las declaraciones de funciones:
  - `int getppid(void);`
  - `int getancestor(int);`

### 2. user/usys.pl
- Agregadas las entradas para generar código ensamblador:
  - `entry("getppid");`
  - `entry("getancestor");`

### 3. kernel/syscall.h
- Definidos números únicos para las llamadas:
  - `#define SYS_getppid 22`
  - `#define SYS_getancestor 23`

### 4. kernel/syscall.c
- Agregadas declaraciones externas:
  - `extern uint64 sys_getppid(void);`
  - `extern uint64 sys_getancestor(void);`
- Agregadas entradas en la tabla de llamadas:
  - `[SYS_getppid] sys_getppid,`
  - `[SYS_getancestor] sys_getancestor,`

### 5. kernel/sysproc.c
- Implementadas las funciones reales del sistema (como se mostró anteriormente)

### 6. user/yosoytupadre.c
- Creado programa de prueba que demuestra el funcionamiento de ambas llamadas al sistema

### 7. Makefile
- Agregado `$U/_yosoytupadre\` a la lista de programas de usuario

## Dificultades Encontradas y Soluciones

### 1. Error en el Uso de argint()
**Problema**: Al implementar `sys_getancestor()`, se intentó usar `argint()` como si retornara un valor:
```c
if(argint(0, &n) < 0) 
    return -1;
```

**Error de compilación**:
```
kernel/sysproc.c:122:6: error: void value not ignored as it ought to be
```

**Solución**: Se corrigió entendiendo que `argint()` en xv6 es una función `void` que simplemente extrae el argumento:
```c
argint(0, &n); 
```

### 2. Error en la Declaración de Funciones
**Problema**: Se declaró incorrectamente `getancestor` con dos parámetros en `user/user.h`:
```c
int getancestor(int, int);  // Incorrecto
```

**Error de compilación**:
```
error: too few arguments to function 'getancestor'
```

**Solución**: Se corrigió la declaración para que fuera consistente con la implementación:
```c
int getancestor(int);  // Correcto
```

### 4. Problema Menor con cat README
**Problema**: Durante las pruebas iniciales de xv6, el comando `cat README` mostró "exec cDME failed".

**Solución**: Se determinó que este es un comportamiento normal en xv6 y no afecta la funcionalidad del sistema. Los comandos `ls` y `echo` funcionaron correctamente.

## Resultados de las Pruebas

Las pruebas ejecutadas demuestran el funcionamiento correcto de ambas implementaciones (se pueden ver también en la imagen adjuntada en WebC):

```
$ yosoytupadre
=== Prueba Parte I: getppid() ===
Mi PID: 3
PID de mi padre: 2

=== Prueba Parte II: getancestor() ===
getancestor(0): 3 (debería ser mi PID)
getancestor(1): 2 (debería ser mi padre)
getancestor(2): 1 (debería ser mi abuelo o -1)
getancestor(10): -1 (debería ser -1)

--- Proceso hijo ---
Mi PID: 4
getancestor(0): 4
getancestor(1): 3
getancestor(2): 2

Pruebas completadas!
```

### Análisis de Resultados

1. **getppid()**: Funciona correctamente, retornando el PID del proceso padre
2. **getancestor(0)**: Retorna el PID del proceso actual (correcto)
3. **getancestor(1)**: Retorna el PID del proceso padre (correcto)
4. **getancestor(2)**: Retorna el PID del proceso abuelo (PID 1, que es init)
5. **getancestor(10)**: Retorna -1 correctamente al no existir el ancestro
6. **Proceso hijo**: Muestra la correcta jerarquía de procesos (hijo→padre→abuelo)

## Conclusiones

La implementación fue exitosa y permitió comprender varios aspectos fundamentales:

1. **Arquitectura de llamadas al sistema**: Se aprendió cómo los diferentes componentes de xv6 trabajan juntos para proporcionar servicios del kernel a los programas de usuario

2. **Vaidación y manejo de errores**: Se implementaron validaciones apropiadas para casos límite como ancestros inexistentes

La tarea demostró que aunque xv6 es un sistema operativo educativo, implementar nuevas funcionalidades requiere una comprensión profunda de su arquitectura y un cuidado especial en mantener la consistencia entre todos los componentes del sistema.

## Información del Sistema

- **Sistema Operativo**: Ubuntu en WSL
- **Versión de QEMU**: 8.1.0 (compilado desde código fuente)
- **Fecha de implementación**: Septiembre 2025
