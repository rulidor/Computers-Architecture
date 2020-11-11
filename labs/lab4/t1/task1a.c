#include "util.h"


#define SYS_EXIT 0
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_LSEEK 19


#define STDIN 0
#define STDOUT 1
#define STDERR 2

int main (int argc , char* argv[], char* envp[])
{
  int i;
  char * str;
  char * fileName; /*for 0b*/
  fileName = argv[1];
  int feedback=0; /*hold ret. values from sys calls*/
  int debugFlag=0;
  int inputFlag=0;/*0 - read from stdIn. 1 - from a file*/
  int outpuFlag=0;/*0 - write to stdOut. 1 - to a file*/


  /*checking modes*/
  for(i=1; i<argc; i++){
    if(strcmp(argv[i],"-D")==0)
      debugFlag=1;
    if(strncmp(argv[i], "-i", 2))
      inputFlag=1;
    if(strncmp(argv[i], "-o", 2))
      outpuFlag=1;
  }
  
  makeUpper(debugFlag);
  
  return 0;
}

int MAX_LENGTH = 128;

void makeUpper(int debugFlag){
  char strRead[] = "system call ID: 3, Return value: ";
  char strWrite[] = "system call ID: 4, Return value: ";

  char input[MAX_LENGTH];
  int feedback;
  char c[1];

  while( ( feedback = system_call(3, 0, c, 1) ) > 0){
    if(debugFlag == 1 && c[0] != '\n'){
      printFeedback(strRead, feedback);
    }
    if (c[0] >= 'a' && c[0] <= 'z'){
      c[0] = c[0] - ('a'-'A');
    }
    feedback = system_call(4,STDOUT,c, 1);/*sys_write*/
    feedback = system_call(4,STDOUT,"\n", 1);/*sys_write*/
    if(debugFlag == 1 && c[0] != '\n'){
      printFeedback(strWrite, feedback);
    }
  }
}


void printFeedback(char sysMsg[], int feedback){
  system_call(SYS_WRITE,STDERR,sysMsg, strlen(sysMsg));/*sys_write*/
  system_call(SYS_WRITE,STDERR,itoa(feedback), strlen(itoa(feedback)));/*sys_write*/
  system_call(SYS_WRITE,STDERR,"\n", 1);/*sys_write*/
}






/*OLD main

  str = itoa(argc);
  system_call(SYS_WRITE,STDOUT, str,strlen(str));
  system_call(SYS_WRITE,STDOUT," arguments \n", 12);
  for (i = 0 ; i < argc ; i++)
    {
      system_call(SYS_WRITE,STDOUT,"argv[", 5);
	    str = itoa(i);
      system_call(SYS_WRITE,STDOUT,str,strlen(str));
      system_call(SYS_WRITE,STDOUT,"] = ",4);
      system_call(SYS_WRITE,STDOUT,argv[i],strlen(argv[i]));
      system_call(SYS_WRITE,STDOUT,"\n",1);
    }


  system_call(4,STDOUT,"\n0b:\n", 5);/*sys_write
  int fileDesc = system_call(5,fileName,2, 0777);/*sys_open
  system_call(4,STDOUT,"fileDesc=", 9);/*sys_write
  str = itoa(fileDesc);
  system_call(4,STDOUT,str, strlen(str));/*sys_write
  system_call(4,STDOUT,"\n", 1);/*sys_write
  if (fileDesc<0)
    system_call(1,0x55,0, 0);/*sys_exit

  feedback = system_call(19,fileDesc,0x291, 0);/*sys_LSeek
  if(feedback<0)
    system_call(1,0x55,0, 0);/*sys_exit

  feedback = system_call(4,fileDesc,x_name, strlen(x_name));/*sys_write
  if(feedback<=0)
    system_call(4,STDOUT,"nothing changed\n", 16);/*sys_write
  else
    system_call(4,STDOUT,"file changed successfully\n", 26);/*sys_write

  system_call(6,fileDesc);/*sys_close


*/