#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char *addr;
  
  printf("Test de protección de lectura\n");
  
  // Asignar memoria
  addr = sbrk(4096);
  if(addr == (char*)-1){
    printf("ERROR: sbrk falló\n");
    exit(1);
  }
  
  // Escribir valor inicial
  addr[0] = 'Z';
  printf("Valor inicial escrito: %c\n", addr[0]);
  
  // TEST 1: Aplicar protección
  printf("Aplicando mrdprotect...\n");
  if(mrdprotect(addr, 4096) < 0){
    printf("ERROR: mrdprotect falló\n");
    exit(1);
  }
  printf("mrdprotect aplicado correctamente\n");
  
  // TEST 2: Intentar LEER (debería fallar)
  printf("TEST 1: Intentando leer (esto DEBE causar page fault)...\n");
  char val = addr[0];  // Esta línea debería matar el proceso
  printf("ERROR: Lectura exitosa, valor=%c (NO debería imprimirse)\n", val);
  
  // Si llegamos aquí, la protección NO funcionó
  printf("FALLO: La protección de lectura no está funcionando\n");
  exit(1);
}