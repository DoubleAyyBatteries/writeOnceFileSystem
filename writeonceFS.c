
// #include <signal.h>
#include "writeonceFS.h"

struct fileblock *head;
int fileListTotalSize = 0;
char OGFile[128];

struct fileblock *next(struct fileblock *block)
{
    char *temp;
    temp = (char *)block;
    temp += sizeof(fb);
    temp += block->dataSize;
    // printf("Location: %ld\n", 0 + sizeof(fb) + block->dataSize);
    return (fb *)temp;
}

int isSignalError = 0;

static void handler(int sig)
{
    printf("SIGSEGV %d\n", sig);
    isSignalError = 1;
    printf("Error: Cannot Read File. Format Mismatch\n");
    // exit(EXIT_FAILURE);
    // abort();
    signal(SIGSEGV, SIG_IGN);
}

// void initialize(void **memoryAddress)
// {
//     head = (struct fileblock *)malloc(sizeof(struct fileblock));
//     head->allperm = 7;
//     head->groupperm = 7;
//     head->usrperm = 7;
// }

int wo_mount(char *filename, void *memoryAddress);
// int wo_unmount(void* memoryAddress);
// int wo_open(char* filename, int flags);
// int wo_open(char* filename, int flags, int mode);
// int wo_read(int fd, void* buffer, int bytes);
// int wo_write(int fd, void* buffer, int bytes);
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
        strcpy(OGFile, filename);
        char filedata[default_size];
        fgets(filedata, sizeof(filedata), fptr);
        if (buffer.st_size == 0)
        {
            printf("Check 1: Size = 0\n");
            char *cma = (char *)memoryAddress;
            // printf("size of memadd = %ld\n", sizeof(*memoryAddress));

            memset(cma, 0, default_size);
            // for(int i = 0; i < 50; i++){
            //     printf("%d, %c\n", i, cma[i]);
            // }

            head = (fb *)&cma[0];
            fb *temp = head;
            // temp->filename[0] = 'y';
            // printf("%s\n", temp->filename);
            temp->dataSize = quanta;
            temp->allperm = 7;
            temp->groupperm = 7;
            temp->usrperm = 7;
            memcpy(&cma[0], temp, sizeof(*head));
            fileListTotalSize = sizeof(*head) + quanta;
            printf("Size of files: %d\n", fileListTotalSize);
            printFiles();
        }
        else if (buffer.st_size > 0)
        {
            printf("Check 2: Size = %ld\n", buffer.st_size);
            // read file to see where to add and check for errors
            if (sizeof(*head) > buffer.st_size)
            {
                printf("Error Parsing FIle: Data size too small\n");
                return -1;
            }
            // char *cma = (char *)memoryAddress;
            head = (fb *)&filedata[0];
            fb *temp = head;

            signal(SIGSEGV, handler);
            int x = 0;
            int currSize = 0;
            while (currSize < default_size)
            {
                // if (strcmp(temp->filename, "") == 0)
                // {
                //     printf("Error Parsing File: Empty Filename\n");
                //     // break;
                //     return -1;
                // }
                if (temp->usrperm < 0 || temp->usrperm > 7 || temp->groupperm < 0 || temp->groupperm > 7 || temp->allperm < 0 || temp->allperm > 7)
                {
                    printf("Error Parsing File: Permissions Invalid\n");
                    // break;
                    return -1;
                }
                if (temp->dataSize < 0 || temp->dataSize > default_size)
                {
                    printf("Error Parsing File: Data Size Cannot be negative or greater than 4MB\n");
                    // break;
                    return -1;
                }

                // printf("Checking signal...\n");
                // if (isSignalError == 1)
                // {
                //     return -1;
                // }
                // printf("Assigning x\n");
                x++;
                // printf("Assigning currSize\n");
                currSize += sizeof(*temp) + temp->dataSize;
                printf("Block %d: Perm = %d%d%d, Datasize = %ld\n", x, temp->usrperm, temp->groupperm, temp->allperm, temp->dataSize);

                temp = next(temp);
            }
            fileListTotalSize = currSize;

            signal(SIGSEGV, SIG_DFL);
            // if (isSignalError)
            // {
            //     return -1;
            // }
            memcpy(memoryAddress, filedata, sizeof(filedata));
            char *cma = (char *)memoryAddress;
            head = (fb *)&cma[0];

            printf("Head: Perm = %d%d%d, Datasize = %ld\n", head->usrperm, head->groupperm, head->allperm, head->dataSize);
        }
    }
    return 0;
}

int wo_unmount(void *memoryAddress)
{
    FILE *fptr;
    fptr = fopen(OGFile, "w");
    if (fptr == NULL)
    {
        printf("Error Unmounting File: Null");
        return -1;
    }

    char *cma = (char *)memoryAddress;
    for (int i = 0; i < default_size; i++)
    {
        if (cma[i] > 0)
        {
            printf("%d", cma[i]);
        }
    }

    printf("\nDone Printing Memory\n");

    fb *temp = head;
    int currSize = 0;

    while (currSize < fileListTotalSize)
    {
        char *ctb;
        ctb = (char *)temp;
        for (int i = currSize; i < currSize + sizeof(*temp); i++)
        {
            fprintf(fptr, "%d", ctb[i]);
        }
        ctb += sizeof(*temp);
        currSize += sizeof(*temp);
        for (int i = currSize; i < currSize + temp->dataSize; i++)
        {
            fprintf(fptr, "%d", ctb[i]);
        }
        ctb += temp->dataSize;
        currSize += temp->dataSize;
        temp = next(temp);
    }
    return 0;
}

void printFiles()
{
    fb *temp = head;
    int x = 0;
    int curSize = 0;
    while (curSize < fileListTotalSize)
    {
        printf("Block %d: Perm = %d%d%d, Datasize = %ld\n", x, temp->usrperm, temp->groupperm, temp->allperm, temp->dataSize);
        x++;
        curSize += sizeof(*temp) + temp->dataSize;
        temp = next(temp);
    }
}

int main(int argc, char *argv[])
{
    int *test = (int *)malloc(default_size);
    int returnCode = wo_mount(argv[1], test);
    printf("Return Code = %d\n", returnCode);

    returnCode = wo_unmount(test);
    printf("Return Code = %d\n", returnCode);
    // printf("%s\n", OGFile);
    free(test);
}
