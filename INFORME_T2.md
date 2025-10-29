# Tarea 2: Lottery Scheduling en xv6

## Integrantes
- Grupo K

## Descripción
Implementación de un planificador de procesos basado en Lottery Scheduling que reemplaza el Round-Robin original de xv6. Cada proceso recibe un número de "tickets" y la probabilidad de ser elegido por el scheduler es proporcional a su cantidad de tickets.

---

## Funcionamiento y Lógica de la Implementación

### 1. Estructura del Proceso (`kernel/proc.h`)
Se agregaron dos campos al `struct proc`:
- `int tickets`: Número de tickets asignados al proceso (mínimo 1)
- `int run_slices`: Contador de veces que el proceso ha sido planificado

### 2. Inicialización (`kernel/proc.c - allocproc()`)
Todos los procesos nuevos se inicializan con:
- `tickets = 100` (valor por defecto)
- `run_slices = 0`

### 3. Algoritmo de Lotería (`kernel/proc.c - scheduler()`)
El scheduler implementa el siguiente algoritmo:

1. **Calcular tickets totales**: Suma los tickets de todos los procesos RUNNABLE
2. **Generar ticket ganador**: Número aleatorio entre 1 y total_tickets
3. **Seleccionar ganador**: Acumula tickets proceso por proceso hasta alcanzar el número ganador
4. **Ejecutar proceso**: El proceso seleccionado entra en estado RUNNING y se incrementa su `run_slices`
```c
// Pseudocódigo simplificado
total = suma(tickets de procesos RUNNABLE)
ganador = random(1, total)
acumulado = 0
para cada proceso RUNNABLE:
    acumulado += proceso.tickets
    si acumulado >= ganador:
        ejecutar proceso
        break
```

### 4. Generador de Números Aleatorios
Se implementó un generador de números pseudoaleatorios (Linear Congruential Generator) para la selección de procesos.

### 5. System Call `settickets(int n)`
Permite que un proceso modifique su cantidad de tickets en tiempo de ejecución.

**Archivos modificados:**
- `kernel/sysproc.c`: Implementación de `sys_settickets()`
- `kernel/syscall.h`: Definición `#define SYS_settickets 22`
- `kernel/syscall.c`: Registro de la syscall
- `user/usys.pl`: Generación de stub
- `user/user.h`: Prototipo para espacio de usuario

---

## Modificaciones Realizadas

### Archivos del Kernel
1. **kernel/proc.h**: Agregados campos `tickets` y `run_slices` al struct proc
2. **kernel/proc.c**: 
   - Función `random()` para números aleatorios
   - `allocproc()`: Inicialización de campos
   - `scheduler()`: Implementación completa de Lottery Scheduling
3. **kernel/sysproc.c**: Implementación de `sys_settickets()`
4. **kernel/syscall.h**: Definición de número de syscall
5. **kernel/syscall.c**: Registro de la syscall en el kernel

### Archivos de Usuario
6. **user/usys.pl**: Entry para la syscall
7. **user/user.h**: Prototipo de `settickets()`
8. **user/demo.c**: Programa de prueba con 10 procesos

### Configuración
9. **Makefile**: Agregado `$U/_demo\` a UPROGS

---

## Dificultades y Soluciones

### 1. Error en `argint()`
**Problema**: La firma de `argint()` en esta versión de xv6 no retorna valor.

**Solución**: Remover la verificación `if(argint(0, &n) < 0)` y usar `argint(0, &n)` directamente.

### 2. Deadlock en el scheduler
**Problema**: Si no hay procesos RUNNABLE, el scheduler se queda en loop infinito.

**Solución**: Verificar `total_tickets == 0` y ejecutar `wfi` (Wait For Interrupt) para evitar consumo innecesario de CPU.

### 3. Sincronización con locks
**Problema**: Riesgo de condiciones de carrera al acceder a `p->tickets` y `p->state`.

**Solución**: Usar `acquire(&p->lock)` y `release(&p->lock)` correctamente en cada acceso.

---

## Problemas del Lottery Scheduling

### 1. **Falta de Garantías de Tiempo**
Un proceso con pocos tickets puede sufrir de inanición (starvation) si hay muchos procesos con más tickets. No hay garantía de cuándo será ejecutado.

### 2. **Predicción Imprecisa**
La distribución de CPU es probabilística, no determinística. Un proceso con 10% de tickets podría obtener 5% o 15% del CPU en la práctica debido a la aleatoriedad.

### 3. **Overhead del Random**
La generación de números aleatorios y el cálculo de tickets totales en cada ciclo del scheduler agrega overhead computacional comparado con Round-Robin.

### 4. **Dificultad para Procesos Interactivos**
Los procesos que requieren respuesta rápida (ej: interfaz gráfica) pueden experimentar latencia variable, afectando la experiencia del usuario.

### 5. **Problema de Granularidad**
Si hay pocos procesos, las diferencias en tickets se notan mucho. Con muchos procesos, las diferencias se diluyen y el comportamiento se acerca a Round-Robin.

### 6. **No Considera Prioridad Real-Time**
No hay forma de garantizar que un proceso crítico siempre tenga prioridad, solo mayor probabilidad. Esto es problemático para sistemas de tiempo real.

---

## Compilación y Ejecución
```bash
# Compilar
make clean
make

# Ejecutar xv6
make qemu

# Dentro de xv6, ejecutar demo
$ demo
```

---

## Resultados

El programa `demo` crea 10 procesos con tickets incrementales (50, 100, 150, ..., 500). La salida muestra que todos los procesos se ejecutan y completan, demostrando que el Lottery Scheduler funciona correctamente.

La distribución probabilística de CPU se observa en el orden variable de finalización de los procesos, aunque los procesos con más tickets tienden a obtener más tiempo de CPU en promedio.