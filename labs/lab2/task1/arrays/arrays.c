#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main (int argc, char** argv){

int iarray[3];
float farray[3];
double darray[3];
char carray[3];


printf("%s\n","float array");

printf("%p\n",farray);
printf("%p\n",farray+1);
printf("%p\n",farray+2);

printf("\n");



printf("%s\n","double array");

printf("%p\n",darray);
printf("%p\n",darray+1);
printf("%p\n",darray+2);

printf("\n");



printf("%s\n","char array");

printf("%p\n",carray);
printf("%p\n",carray+1);
printf("%p\n",carray+2);

printf("\n");	
    
    return 0;
}
