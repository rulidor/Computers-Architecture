#include <stdio.h>
#include <string.h>

extern void assFunc(int,int);

int main(int argc, char** argv){
  int x,y;
  printf("Enter two numbers:\n");

  scanf("%d",&x);// input is valid so we dont have to check it
  scanf("%d",&y);
  assFunc(x,y);

  return 0;
}

char c_checkValidity(int x, int y){
  if(x>=y){
    return '1';
    //printf("%dfrom C\n",1);
  }
  // printf("%dfrom C\n",0);
  return '0';
}