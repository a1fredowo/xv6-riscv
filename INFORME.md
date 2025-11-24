# Tarea 3: Protección de Lectura en xv6

**Grupo K**  
**Fecha:** 23 de noviembre de 2025

---

## 1. Implementación

### 1.1 Objetivo
Implementar protección de memoria "solo escritura" (write-only) en xv6, donde ciertas páginas de memoria pueden ser escritas pero no leídas por procesos de usuario.

### 1.2 Enfoque Utilizado

Se utilizó un **bit personalizado en el PTE** (`PTE_RDPROTECT`) para marcar páginas protegidas de lectura, combinado con manejo de page faults en el trap handler.

#### Modificaciones realizadas:

1. **kernel/riscv.h** - Definición del bit personalizado:
```c
   #define PTE_RDPROTECT (1L << 9)  // Bit 9 reservado para software
```

2. **kernel/vm.c** - Funciones de protección:
   - `mrdprotect()`: Marca páginas como protegidas y quita el bit PTE_R
   - `munrdprotect()`: Restaura el bit PTE_R y quita la marca de protección

3. **kernel/trap.c** - Manejo de page faults:
   - Detecta accesos a páginas con `PTE_RDPROTECT`
   - Scause 13 (Load): Deniega y mata el proceso
   - Scause 15 (Store): Permite temporalmente dando permisos R+W

4. **kernel/sysproc.c** - Syscalls:
   - `sys_mrdprotect()`: Wrapper para mrdprotect
   - `sys_munrdprotect()`: Wrapper para munrdprotect

5. **kernel/syscall.h, kernel/syscall.c, user/user.h, user/usys.pl**:
   - Registro de las nuevas syscalls con números 25 y 26

---

## 2. Funcionamiento

### 2.1 Algoritmo de mrdprotect()
```
Para cada página en el rango [va, va+len):
  1. Obtener PTE con walk()
  2. Verificar que la página sea válida
  3. Activar bit PTE_RDPROTECT
  4. Quitar bit PTE_R (mantener PTE_W y PTE_V)
  5. Llamar sfence_vma() para invalidar TLB
```

### 2.2 Manejo de Page Faults

Cuando ocurre un page fault (scause 13 o 15):

1. **Verificar si tiene PTE_RDPROTECT**
   - Si NO: Intentar lazy allocation (vmfault)
   
2. **Si tiene PTE_RDPROTECT:**
   - **Load (lectura)**: Denegar → matar proceso
   - **Store (escritura)**: Dar permisos R+W temporalmente

### 2.3 Algoritmo de munrdprotect()
```
Para cada página en el rango [va, va+len):
  1. Obtener PTE con walk()
  2. Si tiene PTE_RDPROTECT:
     - Quitar bit PTE_RDPROTECT
     - Restaurar bits PTE_R, PTE_W, PTE_U
  3. Llamar sfence_vma()
```

---

## 3. Archivos Modificados

| Archivo | Cambios |
|---------|---------|
| `kernel/riscv.h` | Agregado `#define PTE_RDPROTECT` |
| `kernel/defs.h` | Declaraciones de mrdprotect/munrdprotect |
| `kernel/vm.c` | Implementación de mrdprotect/munrdprotect |
| `kernel/trap.c` | Manejo de page faults para páginas protegidas |
| `kernel/sysproc.c` | Syscalls sys_mrdprotect/sys_munrdprotect |
| `kernel/syscall.h` | Definiciones SYS_mrdprotect (25), SYS_munrdprotect (26) |
| `kernel/syscall.c` | Registro de las syscalls |
| `user/user.h` | Prototipos de mrdprotect/munrdprotect |
| `user/usys.pl` | Entries para las syscalls |
| `user/rdprotect_test.c` | Programa de prueba (bloqueo de lectura) |
| `user/wrtest.c` | Programa de prueba (permitir escritura) |
| `Makefile` | Agregados programas de prueba |

---

## 4. Pruebas

### 4.1 Test de Bloqueo de Lectura (`rdprotect_test`)
```c
char *addr = sbrk(4096);
addr[0] = 'Z';                    // ✓ Funciona (sin protección)
mrdprotect(addr, 4096);           // ✓ Aplicar protección
char val = addr[0];               // ✗ Page fault → proceso muere
```

**Resultado esperado:**
```
usertrap(): intento de lectura en página protegida pid=3
            sepc=0x6c stval=0x4000
```

### 4.2 Test de Escritura (`wrtest`)
```c
char *addr = sbrk(4096);
addr[0] = 'Z';                    // ✓ Escribir inicial
mrdprotect(addr, 4096);           // ✓ Proteger
addr[0] = 'A';                    // ✓ Escritura permitida
munrdprotect(addr, 4096);         // ✓ Remover protección
printf("%c\n", addr[0]);          // ✓ Ahora lectura funciona: 'A'
```

**Resultado esperado:**
```
✓ Escritura exitosa
✓ Test completado exitosamente
```

---

## 5. Limitaciones y Consideraciones

### 5.1 Limitación Arquitectónica de RISC-V

**Problema:** En RISC-V, una página con **solo permiso de escritura** (W sin R) es considerada **inválida** según la especificación.

**Consecuencia:** Después de la primera escritura, debemos dar permisos R+W para que la instrucción se ejecute, lo que deja la página legible.

**Solución implementada:** 
- Bloqueamos lecturas **antes de cualquier escritura**
- La primera escritura activa R+W permanentemente
- Documentamos esta limitación como inherente a RISC-V

### 5.2 Alternativas No Implementadas

1. **Single-stepping**: Usar el bit de debug de RISC-V para quitar R después de cada instrucción
   - ❌ Demasiado complejo
   - ❌ Gran overhead de rendimiento

2. **Emulación de instrucciones**: Decodificar y ejecutar stores en el kernel
   - ❌ Requiere decodificador RISC-V completo
   - ❌ Muy complejo para el alcance de la tarea

### 5.3 Compatibilidad con Lazy Allocation

El código mantiene compatibilidad con el mecanismo de lazy allocation existente:
```c
if((pte = walk(...)) != 0 && (*pte & PTE_RDPROTECT)) {
  // Manejar página protegida
} else if(vmfault(...) != 0) {
  // Lazy allocation
}
```

---

## 6. Casos de Uso

La protección de lectura es útil para:

1. **Claves criptográficas**: Escribir claves en memoria pero prevenir lectura accidental
2. **Write-only logs**: Permitir append pero no lectura no autorizada
3. **Buffers de salida**: Procesos pueden escribir pero no espiar datos de otros

---

## 7. Comandos de Compilación
```bash
make clean
make
make qemu
```

Dentro de xv6:
```bash
rdprotect_test    # Test de bloqueo de lectura
wrtest            # Test de escritura
```

---

## 8. Conclusiones

✅ **Logrado:**
- Implementación funcional de protección de lectura
- Bloqueo exitoso de operaciones de lectura
- Permiso correcto para operaciones de escritura
- Restauración de permisos con munrdprotect

⚠️ **Limitaciones reconocidas:**
- Limitación arquitectónica de RISC-V (W-only no soportado nativamente)
- Páginas quedan legibles después de primera escritura
- Solución completa requeriría hardware específico o emulación costosa

La implementación cumple con los objetivos de la tarea dentro de las restricciones arquitectónicas de RISC-V.