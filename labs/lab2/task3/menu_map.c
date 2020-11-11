#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Headers to include functions from task2-c */

char quit(char c);
char encrypt(char c);
char decrypt(char c);
char dprt(char c);
char cprt(char c);
char my_get(char c);
char censor(char c);
char* map(char *array, int array_length, char (*f) (char));

/******************************/
/* end of including functions */


struct fun_desc {
  char *name;
  char (*fun)(char);
};
 
 
 
int main(int argc, char **argv){

int choice; //for debugging

//char (*carray)[5];//FIX ME!
char* carray=(char*)(malloc(5*sizeof(char)));
char* arrAdrs;



struct fun_desc menu[] = { {"Censor" , censor}, { "Encrypt", encrypt}, { "Decrypt", decrypt }, { "Print dec", dprt }, { "Print string",cprt}, { "Get string", my_get}, { "Quit", quit}, { NULL, NULL } };

int bound=sizeof(menu)/sizeof(struct fun_desc);
//printf("***DEBUG: size of carray is: %d\n",bound-2);


struct fun_desc* funcIndex; //to iterate through menu array


do {

	int i=0;
	printf("Please choose a function:\n");
	for(funcIndex=menu; funcIndex[i].name!=NULL; i++){
		printf("%d) %s\n",i ,funcIndex[i].name);
	} 
	printf("Option: ");
	scanf("%d", &choice);
	if(!(choice>=0 && choice<=bound-2)) //case NOT in the bounds
	{
		printf("Not within bounds\n");
		quit('q');
	}
	else
		printf("Within bounds\n");
	//printf("choice is: %d\n", choice);
	//printf("%s\n",menu[choice].name);
	if(choice==6)
		quit('q');
	arrAdrs=carray;
	char* carray = map(carray, 5, menu[choice].fun);
	
	printf("DONE.\n\n");
	//free(arrAdrs);
	
	//printf("DEBUG: the string received is: %s\n",carray);

} while(1);

//TO CHECK: my_get, encrypt, cprt

return 0;

}
