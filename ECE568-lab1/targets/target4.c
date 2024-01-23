#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
foo ( char *arg )
{
	int		len;
	int		i;
	char		buf[156];
	static char *	a;
	static char *	b;
	
	len = strlen(arg);
	if (len > 169) len = 169;

	a = arg;
	b = buf;

    for (i = 0; i < len; i += 4) {
        printf("Value at %p is: %x\n", &arg[i], *(*int) &arg[i]);
    }

	printf("len: %d\n", len);

	for (i = 0; i <= len; i++) {
        if (i >= 167) {
            printf("i before copy: %d\n", i);
            printf("value at arg %d: %x\n", i, arg[i]);
            printf("value at a: %x\n", *a);
            printf("value at b: %x\n", *b);
        }
		*b++ = *a++;
        if (i == 167) {
            printf("Address of i: %p\n", (void *)&i);
            printf("Value at address 0x3021fe97: %x\n", *(unsigned char *)0x3021fe97);
            printf("Value at address 0x3021fe98: %x\n", *(unsigned char *)0x3021fe98);
            printf("Value at address 0x3021fe99: %x\n", *(unsigned char *)0x3021fe99);
        }
        if (i >= 168) {
            printf("i after copy: %d\n", i);
//            printf("value at buf %d: %x\n", i, buf[i]);
            printf("Value at address 0x3021fe98: %x\n", *(unsigned char *)0x3021fe98);
            printf("Value at address 0x3021fe99: %x\n", *(unsigned char *)0x3021fe99);
        }
	}
		
	
	return (0);
}

int
lab_main ( int argc, char *argv[] )
{
	printf ("Target4 running.\n");

	if (argc != 2)
	{
		fprintf(stderr, "target4: argc != 2\n");
		exit(EXIT_FAILURE);
	}
	
	foo ( argv[1] );
	return (0);
}
