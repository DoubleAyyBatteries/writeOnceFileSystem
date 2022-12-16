#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>



#define default_size 4194304
#define quanta 1024

typedef struct fileblock
{
    char filename[128];
    int fd;        // file descriptor
    int allperm;   // all permissions
    int groupperm; // group permissions
    int usrperm;   // all permissions

    size_t dataSize; // internal data size. Always >= user specified data size
    void *data;
    // struct fileblock next;
} fb;

struct fileblock *next(struct fileblock *block);
void initialize();
void printFiles();

// int wo_mount(char* filename, void* memoryAddress);
// int wo_unmount(void* memoryAddress);
// int wo_open(char* filename, int flags);
// int wo_open(char* filename, int flags, int mode);
// int wo_read(int fd,  void* buffer, int bytes);
// int wo_write(int fd,  void* buffer, int bytes);
// int wo_close(int fd);
