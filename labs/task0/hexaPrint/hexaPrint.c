#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void PrintHex(char* buffer, int length) {
	int i;
	for (i=0; i<length-1; i++){
		printf("%X ",(unsigned char)buffer[i]);
	}
	printf("%X\n",(unsigned char)buffer[i]);
}






int main(int argc, char **argv) {

		fork();
		if (fork()%2)
			printf("Spoon ");
		else
			printf("Fork ");



	return 0;
















    char* fileName=argv[1];
    char buffer[200];
    FILE * stream = fopen(fileName, "r");
    int count = fread(&buffer, sizeof(char), 200, stream);
	
	PrintHex(buffer,strlen(buffer)); 
	
	fclose(stream);    

    return 0;
}
