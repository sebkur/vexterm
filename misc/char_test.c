#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
	printf("Type something, please:\n");

	int s = 100;
	char * buf = malloc(sizeof(char) * s);
	fgets(buf, s, stdin);
	
	unsigned char c; int x = 0;
	while (1){
		c = buf[x++];
		if (c == '\0') break;
		printf("|%x|", c);
	}
	printf("\n");
}
