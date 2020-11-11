#include "util.h"


#define SYS_EXIT 0
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define O_WRONLY 1 
#define O_CREATE 64


extern int system_call();
int MAX_LENGTH = 128;

void printFeedback(char sysMsg[], int feedback){
  system_call(SYS_WRITE,STDERR,sysMsg, strlen(sysMsg));
  system_call(SYS_WRITE,STDERR,itoa(feedback), strlen(itoa(feedback)));
  system_call(SYS_WRITE,STDERR,"\n", 1);
}

void makeUpper(int debugFlag, int inFileDesc, int outFileDesc){
  char strRead[] = "system call ID: 3, Return value: ";
  char strWrite[] = "system call ID: 4, Return value: ";

  int feedback;
  char c[1];
  
  while( ( feedback = system_call(3, inFileDesc, c, 1) ) > 0){    
    if(debugFlag == 1 && c[0] != '\n'){
      printFeedback(strRead, feedback);
    }
    if (c[0] >= 'a' && c[0] <= 'z'){
      c[0] = c[0] - ('a'-'A');
    }
    feedback = system_call(SYS_WRITE,outFileDesc,c, 1);
    if(debugFlag == 1 && c[0] != '\n'){
      feedback = system_call(SYS_WRITE,outFileDesc,"\n", 1);
      printFeedback(strWrite, feedback);
    }
  }
}


int main (int argc , char* argv[], char* envp[])
{
  int i;
  int debugFlag=0;
  int inFileDesc=0;/*STDIN*/
  int outFileDesc=1;/*STDOUT*/


  /*checking modes*/
  for(i=1; i<argc; i++){
    if(strcmp(argv[i],"-D")==0)
      debugFlag=1;
    if(strncmp(argv[i], "-i", 2) == 0){
      inFileDesc = system_call(SYS_OPEN, argv[i]+2,0, 0777);
    }
    if(strncmp(argv[i], "-o", 2) == 0){
      system_call(SYS_OPEN, argv[i]+2,O_CREATE+O_WRONLY, 0777);
    }
  }
  
  makeUpper(debugFlag, inFileDesc, outFileDesc);

  system_call(SYS_CLOSE, inFileDesc);
  system_call(SYS_CLOSE, outFileDesc);

  
  return 0;
}






