#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include "LineParser.h"

#include <sys/wait.h>

#define PATH_MAX 4096 /* # chars in a path name including nul */

int debugFlag = 0;

typedef struct map map;

struct map {
	map * next;
	char * name;
	char * value;
};

map * myMap;

void freeMap(){
	if(myMap==NULL){
		if(debugFlag){
			printf("map is clear! exeting program\n");
		}
		return;
	}
	map * toClear = myMap;
	myMap = myMap->next;
	free(toClear->name);
	free(toClear->value);
	free(toClear);
	freeMap();
}

void vars(){
	if (debugFlag){
		printf("vars command recieved\n");
	}
	map * current = myMap;
	while(current!=NULL){
		printf("name: %s, value: %s\n",current->name, current->value);
		current = current->next;
	}
	if(myMap==NULL){
		printf("map is empty\n");
	}
}

void set (char * name, char * value){
	if (debugFlag){
		printf("set command recieved\n");
	}
	map * current = myMap;
	while (current!=NULL){
		if (!strcmp(current->name,name)){//name found
			if (debugFlag){
				printf("name was found and value was added\n");
			}
			free(current->value);
			current->value = (char*)malloc((strlen(value)+1)*sizeof(char));
			strncpy(current->value,value,strlen(value)+1);
			return;
		}
		current = current->next;
	}
	if (debugFlag){
		printf("name was not found and new map was created\n");
	}
	//else myMap is NULL or the name does not exist, than current is NULL
	current = (map*)malloc(sizeof(map));
	current->name = (char*)malloc((strlen(name)+1)*sizeof(char));
	current->value = (char*)malloc((strlen(value)+1)*sizeof(char));
	strncpy(current->name,name,strlen(name)+1);
	strncpy(current->value,value,strlen(value)+1);
	current->next = myMap;//adding to the head of the map
	myMap=current;
}

char * findValue(const char * name){
	if (debugFlag){
		printf("searching for name: %s\n", name);
	}
	map * current = myMap;
	while(current!=NULL){
		if(!strcmp(current->name,name)){
			return current->value;
		}
		current = current->next;
	}
	if(current==NULL){
		fprintf(stderr,"name does not exist in map!\n");
	}
	return NULL;
}

char * checkDollar(cmdLine *pCmdLine){
	char * toReplace = "$";
	for(int i = 1; i< pCmdLine->argCount;i++){
		if (pCmdLine->arguments[i][0]=='$'){
			toReplace = findValue(pCmdLine->arguments[i]+1);
			if(toReplace==NULL)//arg was not found in map
				return NULL;
		replaceCmdArg(pCmdLine,i,toReplace);
		}
	}
	return toReplace;
}

void checkInputRedirect(cmdLine *pCmdLine){
	if (pCmdLine->inputRedirect!=NULL){
		if (debugFlag == 1){
			printf("inputRedirect activated\n");
		}
		int fdR;
		if(pCmdLine->inputRedirect[0]=='$'){
			fdR = open(findValue(pCmdLine->inputRedirect+1), O_RDONLY);
		}
		else{
			fdR = open(pCmdLine->inputRedirect, O_RDONLY);
		}
		if (fdR==-1&&debugFlag){
			fprintf(stderr,"file was not found\n");
		}
		else{
			dup2(fdR, 0);
			close(fdR);
		}
	}
}

void checkOutputRedirect(cmdLine *pCmdLine){
	if (pCmdLine->outputRedirect!=NULL) { //if '>' was found in string inputted by user
		if (debugFlag == 1){
			printf("outputRedirect activated\n");
		}
		int fdW;
		if(pCmdLine->outputRedirect[0]=='$'){
			fdW = creat(findValue(pCmdLine->outputRedirect+1), 0644);
		}
		else{
			fdW = creat(pCmdLine->outputRedirect, 0644);
		}
		dup2(fdW, 1);
		close(fdW);
	}
}

void checkRedirect(cmdLine *pCmdLine){
	checkInputRedirect(pCmdLine);
	checkOutputRedirect(pCmdLine);
}

void doPipe(cmdLine *pCmdLine){
	int fd[2];
	if (pipe(fd) == -1 && debugFlag){
		fprintf(stderr,"pipe faild\n");
		return;
	}
	int childPID1 = fork();
	
	if (!childPID1){//==0 then we are in the child process
		close(1);//close to write
		
		dup(fd[1]);//duplicate write-end
		close(fd[1]);//closing fileDesc after dup
		
		execvp(pCmdLine->arguments[0],pCmdLine->arguments);
		perror("Return not expected. Must be an execv error\n");
		_exit(1);
	}
	else{//we're in the parent
		int status1;
		waitpid(childPID1,&status1,0);
		
		int status2;
		close(fd[1]);//close to write, closing standart output
		int childPID2 = fork();
		
		if (!childPID2){//==0 then we are in the child process
			close(0);//close to read, closing standart input
			dup(fd[0]);//duplicate write-end
			close(fd[0]);//closing fileDesc after dup
			execvp(pCmdLine->next->arguments[0],pCmdLine->next->arguments);
			_exit(1);
		}
		
		else{//childPID2>0
			close(fd[0]);//close to read, closing standart input
			waitpid(childPID2,&status2,0);
		}
	}
}

void executeBlocking(cmdLine *pCmdLine)
{
	printf("Blocking is executed!\n");
	if (pCmdLine->next!=NULL){
		doPipe(pCmdLine);
		return;
	}
	//else
	pid_t cpid, w;
	int wstatus;
	cpid = fork();
	if (cpid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (cpid == 0)
	{ /* Code executed by child */
		
		if (debugFlag == 1)
			fprintf(stderr, "child process ID: %ld\n", (long)getpid());
		printf("cmd is: %s\n", *pCmdLine->arguments); //for debugging
		checkRedirect(pCmdLine);
		execvp(pCmdLine->arguments[0], pCmdLine->arguments);
		perror("Return not expected. Must be an execv error\n"); //A successful call to execv does not have a return
																 //value because the new process image overlays the calling process image
		_exit(1);
	}
	else
	{ /* Code executed by parent */
		do
		{
			w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
			if (w == -1)
			{
				perror("waitpid");
				exit(EXIT_FAILURE);
			}

			if (WIFEXITED(wstatus))
			{
				printf("exited, status=%d\n", WEXITSTATUS(wstatus));
			}
			else if (WIFSIGNALED(wstatus))
			{
				printf("killed by signal %d\n", WTERMSIG(wstatus));
			}
			else if (WIFSTOPPED(wstatus))
			{
				printf("stopped by signal %d\n", WSTOPSIG(wstatus));
			}
			else if (WIFCONTINUED(wstatus))
			{
				printf("continued\n");
			}
		} while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
	}
}

void execute(cmdLine *pCmdLine)
{
	if (debugFlag == 1)
		fprintf(stderr, "Executing command: %s, parent process ID: %d\n", pCmdLine->arguments[0], getpid());
	if (pCmdLine->blocking == 1)
		return executeBlocking(pCmdLine);
	if (pCmdLine->next!=NULL){
		doPipe(pCmdLine);
		return;
	}
	pid_t childPID = fork();
	if (childPID == 0)
	{ //case we're at the child process
		if (debugFlag == 1)
			fprintf(stderr, "child process ID: %d\n", getpid());
		printf("NON blocking is executed!\n");
		fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
		checkRedirect(pCmdLine);
		execvp(pCmdLine->arguments[0], pCmdLine->arguments);
		perror("Return not expected. Must be an execv error\n"); //A successful call to execv does not have a return
																	//value because the new process image overlays the calling process image
		_exit(1);
	}
	//else in perant
}

int main(int argc, char **argv)
{
	char buffer[PATH_MAX];
	char userIn[2048];
	cmdLine *line;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
			debugFlag = 1;
	}
	while (1)
	{
		getcwd(buffer, PATH_MAX);
		printf("current working direcoty: %s\n", buffer);
		fgets(userIn, 2048, stdin);
		line = parseCmdLines(userIn);
		if(!strcmp(line->arguments[0],"quit")){
			freeMap();
			freeCmdLines(line);
			exit(0);
		}

		else if(!strcmp(line->arguments[0],"cd")){
			if (!strncmp(line->arguments[1],"~",1)){
				char * path = line->arguments[1]+1;
				char * tmp = getenv("HOME");
				strcat(tmp,path);
				if (chdir(tmp) != 0 )
					perror("error while trying to change directory\n");
			}
			else if (chdir(line->arguments[1]) != 0 )
				perror("error while trying to change directory\n");
		}
		else if(!strcmp(line->arguments[0],"set")){
			char * value = userIn+strlen(line->arguments[1])+5;//including \n
			value[strlen(value)-1]='\0';//without \n
			set(line->arguments[1],value);
		}
		else if(!strcmp(line->arguments[0],"vars")){
			vars();
		}
		else if(checkDollar(line)!=NULL){//if one one of the argument is not in map, we don't exec
			execute(line);
		}
		freeCmdLines(line);
	}
	return 0;
}

