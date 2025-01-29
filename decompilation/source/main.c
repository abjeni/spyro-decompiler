#include <unistd.h>
#include <assert.h>
#include <stdio.h>

#include "from_cd.h"

int main(int argc, char *argv[])
{
  // keep this in mind
  assert(!chdir(".."));

  int err = decompile_everything();
  if (err) printf("error code: %d\n", err);
  return err;
}