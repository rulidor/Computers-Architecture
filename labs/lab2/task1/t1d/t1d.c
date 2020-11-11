#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main (int argc, char** argv){

int iarray[] = {1,2,3};
char carray[] = {'a','b','c'};
int* iarrayPtr;
char* carrayPtr;
int i=0;
int* p;
iarrayPtr=iarray;
carrayPtr=carray;


printf("iarray REAL address: %p. Its pointer address: %p\n", iarray, iarrayPtr);
printf("values of this array:\n");
printf("[0]: %d\n", *(iarrayPtr));
printf("[1]: %d\n", *(iarrayPtr+1));
printf("[2]: %d\n", *(iarrayPtr+2));

printf("\n");



printf("carray REAL address: %p. Its pointer address: %p\n", carray, carrayPtr);
printf("values of this array:\n");
printf("[0]: %c\n", *(carrayPtr));
printf("[1]: %c\n", *(carrayPtr+1));
printf("[2]: %c\n", *(carrayPtr+2));

printf("\n");

printf("p value is: %p\n",p);
    
    return 0;
}
