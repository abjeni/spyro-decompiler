#pragma once

#include <stdint.h>

#include "functions.h"

typedef struct {
  char type_name[8];
  char zerofilled[8];
  uint32_t initial_pc;
  uint32_t initial_gp;
  uint32_t destination_address;
  uint32_t filesize;
  uint32_t data_start;
  uint32_t data_size;
  uint32_t bss_start;
  uint32_t bss_size;
  uint32_t initial_sp_base;
  uint32_t initial_sp_offs;
  char reserved[20];
  char ascii_marker[1972];
/*
  000h-007h ASCII ID "PS-X EXE"
  008h-00Fh Zerofilled
  010h      Initial PC                   (usually 80010000h, or higher)
  014h      Initial GP/R28               (usually 0)
  018h      Destination Address in RAM   (usually 80010000h, or higher)
  01Ch      Filesize (must be N*800h)    (excluding 800h-byte header)
  020h      Data section Start Address   (usually 0)
  024h      Data Section Size in bytes   (usually 0)
  028h      BSS section Start Address    (usually 0) (when below Size=None)
  02Ch      BSS section Size in bytes    (usually 0) (0=None)
  030h      Initial SP/R29 & FP/R30 Base (usually 801FFFF0h) (or 0=None)
  034h      Initial SP/R29 & FP/R30 Offs (usually 0, added to above Base)
  038h-04Bh Reserved for A(43h) Function (should be zerofilled in exefile)
  04Ch-xxxh ASCII marker
             "Sony Computer Entertainment Inc. for Japan area"
             "Sony Computer Entertainment Inc. for Europe area"
             "Sony Computer Entertainment Inc. for North America area"
             (or often zerofilled in some homebrew files)
             (the BIOS doesn't verify this string, and boots fine without it)
  xxxh-7FFh Zerofilled
  800h...   Code/Data                  (loaded to entry[018h] and up)
*/

} psx_header;

int read_psx_file(addr_list *external_calls);