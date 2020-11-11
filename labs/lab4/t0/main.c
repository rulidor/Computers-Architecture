#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1

int main (int argc , char* argv[], char* envp[])
{
  int i;
  char * str;
  char * fileName; /*for 0b*/
  fileName = argv[1];
  int feedback=0;
  char * x_name = argv[2];

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

  system_call(4,STDOUT,"\n0b:\n", 5);/*sys_write*/
  int fileDesc = system_call(5,fileName,2, 0777);/*sys_open*/
  system_call(4,STDOUT,"fileDesc=", 9);/*sys_write*/
  str = itoa(fileDesc);
  system_call(4,STDOUT,str, strlen(str));/*sys_write*/
  system_call(4,STDOUT,"\n", 1);/*sys_write*/
  if (fileDesc<0)
    system_call(1,0x55,0, 0);/*sys_exit*/

  feedback = system_call(19,fileDesc,0x291, 0);/*sys_LSeek*/
  if(feedback<0)
    system_call(1,0x55,0, 0);/*sys_exit*/

  feedback = system_call(4,fileDesc,x_name, strlen(x_name));/*sys_write*/
  if(feedback<=0)
    system_call(4,STDOUT,"nothing changed\n", 16);/*sys_write*/
  else
    system_call(4,STDOUT,"file changed successfully\n", 26);/*sys_write*/

  system_call(6,fileDesc);/*sys_close*/


  
  return 0;
}
