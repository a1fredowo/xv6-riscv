#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Implementación de la syscall getppid y getancestor para comprobar su funcionamiento.

int
main(int argc, char *argv[])
{
    // Pruebas para getppid
    printf("=== Prueba Parte I: getppid() ===\n");
    printf("Mi PID: %d\n", getpid());
    printf("PID de mi padre: %d\n", getppid());
    
    // Pruebas para getancestor
    printf("\n=== Prueba Parte II: getancestor() ===\n");
    printf("getancestor(0): %d (debería ser mi PID)\n", getancestor(0));
    printf("getancestor(1): %d (debería ser mi padre)\n", getancestor(1));
    printf("getancestor(2): %d (debería ser mi abuelo o -1)\n", getancestor(2));
    printf("getancestor(10): %d (debería ser -1)\n", getancestor(10));
    
    // Crear un proceso hijo para probar getancestor en un contexto diferente
    if(fork() == 0) {
        // Proceso hijo
        printf("\n--- Proceso hijo ---\n");
        printf("Mi PID: %d\n", getpid());
        printf("getancestor(0): %d\n", getancestor(0));
        printf("getancestor(1): %d\n", getancestor(1));
        printf("getancestor(2): %d\n", getancestor(2));
        exit(0);
    } else {
        // Proceso padre
        wait(0);
        printf("\nPruebas completadas!\n");
    }
    
    exit(0);
}