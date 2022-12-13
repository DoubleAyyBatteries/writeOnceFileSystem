
#include "writeonceFS.h"

int wo_mount(char *filename, void *memoryAddress);
// int wo_unmount(void* memoryAddress);
// int wo_open(char* filename, int flags);
// int wo_open(char* filename, int flags, int mode);
// int wo_read(int fd,  void* buffer, int bytes);
// int wo_write(int fd,  void* buffer, int bytes);
// int wo_close(int fd);

int wo_mount(char *filename, void *memoryAddress)
{
    // check if file exists
    if (access(filename, F_OK) != 0)
    {
        printf("Error: File Not Found\n");
        return -1;
    }

    FILE *fptr;
    if ((fptr = fopen(filename, "r")) == NULL)
    {
        printf("Error: Unable to Open File");
        return -1;
    }

    // check if filesize = 0
    struct stat buffer;
    if (stat(filename, &buffer) == 0)
    {

        printf("Size = %ld\n", buffer.st_size);
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int x;
    wo_mount(argv[1], &x);
}
