#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

// buffer overflow warning
/*char memcard_path[256];

char *get_memcard_path(char *file_name)
{
  char *prefix = "mem_card/";
  char *buf = memcard_path;

  uint32_t available = 256-1;

  while (*prefix)
  {
    *buf = *prefix;
    prefix++;buf++;
  }

  while (*file_name)
  {
    *buf = *file_name;
    file_name++;buf++;
    if (available-- <= 0) assert(0);
  }

  *buf = 0;

  return memcard_path;
}


// bit0     1=Read  ;\These bits aren't actually used by the BIOS, however, at
// bit1     1=Write ;/least 1 should be set; won't work when all 32bits are zero
// bit2     1=Exit without waiting for incoming data (when TTY buffer empty)
// bit9     0=Open Existing File, 1=Create New file (memory card only)
// bit15    1=Asynchronous mode (memory card only; don't wait for completion)
// bit16-31 Number of memory card blocks for a new file on the memory card


union mode {
  struct {
    uint32_t read : 1;
    uint32_t write : 1;
    uint32_t exit : 1;
    uint32_t unknown1 : 6;
    uint32_t create : 1;
    uint32_t unknown2 : 5;
    uint32_t async : 1;
    uint32_t blocks : 16;
  };
  uint32_t val;
};

int memcard_open(char *file_name, uint32_t modev, uint32_t *size)
{
  union mode mode = {
    .val = modev
  };

  char *file_path = get_memcard_path(file_name);

  if (mode.create)
  {
    printf("creating %s\n", file_path);
    int fd = creat(file_path, S_IRUSR | S_IWUSR);

    printf("error %m\n");
    if (fd == -1) return -1;

    *size = mode.blocks*0x2000;
    char *buf = calloc(*size, 1);
    write(fd, buf, *size);

    lseek(fd, 0, SEEK_SET);
    return fd;
  }
  else
  {
    printf("opening %s\n", file_path);
    int fd = open(file_path, O_RDWR);
    if (fd == -1) return -1;

    struct stat stats;

    int err = fstat(fd, &stats);

    if (err == -1) return -1;

    *size = stats.st_size;
    return fd;
  }
}

void memcard_close(int fd)
{
    close(fd);
}

int memcard_lseek(int fd, uint32_t offset, int seektype)
{
    return lseek(fd, offset, seektype);
}

ssize_t memcard_write(int fd, void *buf, int size)
{
    return write(fd, buf, size);
}

ssize_t memcard_read(int fd, void *buf, int size)
{
    return read(fd, buf, size);
}*/