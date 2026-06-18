#include <unistd.h>
#include <assert.h>
#include <stdio.h>

#include "from_cd.h"
#include "psx_exe.h"

FILE *graph_fd = 0;

int main(int argc, char *argv[])
{
  // keep this in mind
  assert(!chdir(".."));

  graph_fd = fopen("graph.dot", "w");

  int err = decompile_everything();
  if (err) printf("error code: %d\n", err);
  return err;
}