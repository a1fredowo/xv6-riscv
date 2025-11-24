#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char *addr;
  
  printf("Test de escritura en página protegida\n");
  
  // Asignar memoria
  addr = sbrk(4096);
  if(addr == (char*)-1){
    printf("ERROR: sbrk falló\n");
    exit(1);
  }
  
  // Escribir valor inicial
  addr[0] = 'Z';
  printf("Valor inicial: %c\n", addr[0]);
  
  // Aplicar protección
  printf("Aplicando mrdprotect...\n");
  if(mrdprotect(addr, 4096) < 0){
    printf("ERROR: mrdprotect falló\n");
    exit(1);
  }
  
  // TEST: Intentar ESCRIBIR (debería funcionar)
  printf("TEST: Intentando escribir (debe ser permitido)...\n");
  addr[0] = 'A';
  printf("✓ Escritura exitosa\n");
  
  // Revertir protección
  printf("Revirtiendo protección...\n");
  if(munrdprotect(addr, 4096) < 0){
    printf("ERROR: munrdprotect falló\n");
    exit(1);
  }
  
  // Ahora SÍ podemos leer
  printf("Valor final después de remover protección: %c\n", addr[0]);
  
  printf("✓ Test completado exitosamente\n");
  exit(0);
}