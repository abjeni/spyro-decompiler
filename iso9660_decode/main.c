#include "iso9660.h"
#include <unistd.h>
#include "../config.h"

char *filename = ROM_NAME;

int main(int argc, char *argv[])
{
  chdir("..");
  return read_iso9660(filename);
}