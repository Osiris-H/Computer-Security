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

    for (i = 0; i < 189; i += 4) {
        printf("Value at %p with index %d is: %x\n", (void *)&buf[i], i, *(int *)&arg[i]);
    }

	printf("len: %d\n", len);

	for (i = 0; i <= len; i++) {
		if (i >= 159) {
            printf("b points to: %p, overwrite value is: %x\n", (void *)b, *a);
        }
        *b++ = *a++;
        if (i >= 168) {
            printf("%d\n", i);
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
