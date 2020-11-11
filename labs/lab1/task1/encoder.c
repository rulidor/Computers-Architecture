#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void getString();
void debugM();
void eMode();


int main(int argc, char **argv) {
  int i,a,b,c;
  int isD=0;
  int isE=0;
  char *key;

  for(i=1; i<argc; i++){
    if(strcmp(argv[i],"-D")==0)
	{
		fprintf(stderr, "-D\n");
		isD=1;
	}
	else if(strstr(argv[i], "+e") != NULL)
	{
		fprintf(stderr, "We are in +e state!\n");
		isE=1;
		key=argv[i];
	}
    else{
	printf("invalid parameter - %s\n",argv[i]);
	return 1;
    }
  }
  
	if(isD)
		debugM();
	else if(isE)
		eMode(key);
	else
		getString();

  return 0;
}

void debugM(char* key)
{
	char c;
	int cAsci1, cAsci2;
	
	FILE * fpointer=fopen("sample1.txt","r");
	while((c=fgetc(fpointer))!=EOF)  
		{	
			cAsci1=(int)c;
			cAsci2=cAsci1;
			if(cAsci1>96 && cAsci1<123)
			{
				cAsci2=cAsci1-32;
			}
			fprintf(stderr, "%d\t%d\n",cAsci1,cAsci2);
			printf("%c",(char)cAsci2);
		}
		printf("\n");
	fclose(fpointer);
	
}

void eMode(char* key)
{
	char c;
	int cAsci1;
	int cAsci2;
	int digit;
	char toDigit[2];
	int i=2;
	
	while((c=fgetc(stdin))!=EOF)  
	{	
		toDigit[0]=(char)key[i];
		toDigit[1]=0;
		digit=atoi(toDigit);  
		
			cAsci1=(int)c;
			cAsci2=(cAsci1+digit)%127;
			printf("%c",(char)cAsci2);
	}
	
	
	for(i=2;i<strlen(key);i++)
	{
		toDigit[0]=(char)key[i];
		toDigit[1]=0;
		digit=atoi(toDigit);  
		printf("%i\n",digit);
	}
	
}




