#include <execinfo.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>

#include "psx_mem.h"
#include "not_renamed.h"

uint32_t v0, v1, k0, k1, a0, a1, a2, a3;
uint32_t t0, t1, t2, t3, t4, t5, t6, t7, t8, t9;
uint32_t s0, s1, s2, s3, s4, s5, s6, s7;
uint32_t at, gp, sp, fp, ra, pc, hi, lo;

void wad_main(void);

void print_psx_string_array(char *name, uint32_t base, uint32_t n) {
  printf("char *%s[] = {\n", name);
  for (int i = 0; i < n; i++)
  {
    printf("  \"%s\",\n", (char *)addr_to_pointer(lw(base + i*4)));
  }
  printf("};\n");
}

int main(int argc, char *argv[])
{
  int err = chdir("..");
  if (err) return err;
  
  init_psx_mem();

  sw(0x80074E40, 2); // PSY-Q debug level

  if (argc > 1)
  {

    //wad_main();
    
    print_psx_string_array("dragon_names", 0x8006F694, 80);

    //read_disk1(lw(WAD_sector), 0x80100000, 0x800, 0, 0x256);
    return 0;
  }

  function_8005B8E0();
  return 0;
}