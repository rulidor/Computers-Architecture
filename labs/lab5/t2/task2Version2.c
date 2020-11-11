#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "LineParser.h"

#include <sys/wait.h>

#define PATH_MAX 4096 /* # chars in a path name including nul */

//process status field:
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0



typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;


process* proces_list;
int debugFlag = 0;

void updateProcessStatus(process* process_list, int pid, int status){
	process* current = proces_list;
	while(current!=NULL){
		if (current->pid==pid){
			current->status = status;
			break;
		}
		current = current->next;
	}
}

void updateProcessList(process **process_list){
	process* listHead = *process_list;
	//if(*process_list == NULL){
//		printf("process_list is empty\n");
		//return;
	//}
	while(listHead!=NULL){
		pid_t cpid = listHead->pid;
		pid_t w;
		int wstatus = 0;
		w = waitpid(cpid, &wstatus, WNOHANG);//in general: sucess returns childPID that changed
											//in WNOHANG: 0 - means the childPID did not change
											//// positive - change status
		printf("w is: %d cpis is: %d wstatus is: %d\n",w,cpid,wstatus);
		if (w < 0)
		{
			perror("*******error: waitpid");
			//exit(EXIT_FAILURE);
		}
		if (w>0){
			if (WIFEXITED(wstatus)|WIFSIGNALED(wstatus))
			{
				printf("exited, status=%d\n", WEXITSTATUS(wstatus));
				listHead->status = TERMINATED;
			}
			else if (WIFSTOPPED(wstatus))
			{
				printf("stopped by signal %d\n", WSTOPSIG(wstatus));
				listHead->status = SUSPENDED;
			}
			else if (WIFCONTINUED(wstatus))
			{
				printf("continued\n");
				listHead->status = RUNNING;
			}
		}
		if(listHead->next == NULL)
			break;
		listHead = listHead->next;
	}
}

void freeProcess(process *proce){
	if(!proce)
		return;
		//while list!=null
	freeCmdLines(proce->cmd);
	if (proce->next)
		freeProcess(proce->next);
	free(proce);
}

//Receive a process list (process_list), a command (cmd), and the
//process id (pid) of the process running the command. 
//***adding to beginning of the list***
void addProcess(process** process_list, cmdLine* cmd, pid_t pid){

	// process** listHead = process_list;
	process *newProce=(process*)malloc(sizeof(process));
	newProce->cmd = cmd;
	newProce->pid = pid;
	newProce->status = RUNNING;
	newProce->next = *process_list;
	//deleted: *process_list = newProce;
	proces_list = newProce; //added
	printf("in addProcess: cmd: %s\n",newProce->cmd->arguments[0]);
	printf("in addProcess, list updated to: cmd: %s\n",(*process_list)->cmd->arguments[0]);

}

void removeProcessFromList(process** process_list, int pid){
	if(process_list==NULL){
		return;
	}
	process * current = (*process_list)->next;
	process * prev = (*process_list);
	if(current==NULL){//edge case: list has 1 node
		if(prev==NULL || prev->pid != pid){
			return;
		}
		freeCmdLines(prev->cmd);
		free(prev);
		return;
	}
	//edge case: pid fount at 1st element
	if(prev!=NULL && prev->pid == pid){
		proces_list = prev->next;
		freeCmdLines(prev->cmd);
		free(prev);
		return;
	}
	while(current!=NULL){
		if(current->pid==pid){
			prev->next = current->next;
			freeCmdLines(current->cmd);
			free(current);
			break;
		}
		prev = current;
		current=current->next;

	}
		
}
		


	

//print processes 
void printProcessList(process** process_list){
	int i=0;
	process* listHead = *process_list;
	process* temp = listHead;
	if(*process_list == NULL){
		printf("process_list is empty\n");
		return;
	}
	updateProcessList(process_list);
	while(1){
		switch(listHead->status){
			case RUNNING:
				printf("Index in process list %d\tPID %d\tStatus %s\tCommand: ", i,listHead->pid, "RUNNING");
				break;
			case TERMINATED:
				printf("Index in process list %d\tPID %d\tStatus %s\tCommand: ", i,listHead->pid, "TERMINATED");
				removeProcessFromList(&listHead,listHead->pid);//freshly terminated
				break;
			case SUSPENDED:
				printf("Index in process list %d\tPID %d\tStatus %s\tCommand: ", i,listHead->pid, "SUSPENDED");
				break;
		}
		for(int j=0; j<listHead->cmd->argCount; j++)
			printf("%s ", listHead->cmd->arguments[j]);
		printf("\n");
		i++;
		if(listHead->next == NULL)
			break;
		temp = listHead;
		listHead = listHead->next;
	}

}

void executeBlocking(cmdLine *pCmdLine, int debugFlag)
{
	printf("**Blocking is executed!\n");
	pid_t cpid, w;
	int wstatus = 0;
	cpid = fork();
	if (cpid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (cpid == 0)
	{ /* Code executed by child */
		printf("****in the child process!\n");
		if (debugFlag == 1){
			fprintf(stderr, "child process ID: %ld\n", (long)getpid());
		}
		// printf("pid in fork: %d\n",proces_list->pid);
		execvp(pCmdLine->arguments[0], pCmdLine->arguments);
		perror("Return not expected. Must be an execv error\n"); //A successful call to execv does not have a return
																 //value because the new process image overlays the calling process image
		// proces_list->status = TERMINATED; --- parent
		_exit(1);
	}
	else
	{ /* Code executed by parent */
/*
			if (WIFEXITED(wstatus)|WIFSIGNALED(wstatus))
			{
				printf("exited, status=%d\n", WEXITSTATUS(wstatus));
				listHead->status = TERMINATED;
			}
			else if (WIFSTOPPED(wstatus))
			{
				printf("stopped by signal %d\n", WSTOPSIG(wstatus));
				listHead->status = SUSPENDED;
			}
			else if (WIFCONTINUED(wstatus))
			{
				printf("continued\n");
				listHead->status = RUNNING;
			}
*/
		addProcess(&proces_list,pCmdLine,cpid);
		do
		{
			w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
			if (w == -1)
			{
				printf("finished 2!\n");
				perror("error: waitpid");
				//exit(EXIT_FAILURE);
			}
			if (WIFEXITED(wstatus))
			{
				printf("exited, status=%d\n", WEXITSTATUS(wstatus));
				// proces_list->status = TERMINATED; //added
			}
			else if (WIFSIGNALED(wstatus))
			{
				printf("killed by signal %d\n", WTERMSIG(wstatus));
				// proces_list->status = TERMINATED; //added
			}
			else if (WIFSTOPPED(wstatus))
			{
				printf("stopped by signal %d\n", WSTOPSIG(wstatus));
				//proces_list->status = SUSPENDED; //added
			}
			else if (WIFCONTINUED(wstatus))
			{
				printf("continued\n");
				//proces_list->status = RUNNING; //added
			}
		} while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
		
	}
}

//****this is the NON blocking method****
void execute(cmdLine *pCmdLine)
{
	
	if (debugFlag == 1)
		fprintf(stderr, "Executing command: %s, parent process ID: %d\n", pCmdLine->arguments[0], getpid());
	if (pCmdLine->blocking == 1){
		executeBlocking(pCmdLine, debugFlag);
		proces_list->status = TERMINATED;
		return;
	}
	//execv(pCmdLine->arguments[0], pCmdLine->arguments);
	int childPID = fork();
	
	if (childPID == 0)
	{ //case we're at the child process
		
		printf("NON blocking is executed!\n");
		fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
		execvp(pCmdLine->arguments[0], pCmdLine->arguments);

		perror("Return not expected. Must be an execv error\n"); //A successful call to execv does not have a return
																 //value because the new process image overlays the calling process image
		_exit(1);
	}

	else{// we're at the parent process
		if (debugFlag == 1)
			fprintf(stderr, "child process ID: %d\n", childPID);
		addProcess(&proces_list, pCmdLine, childPID);
		//proces_list->status = TERMINATED;
	}

}


int main(int argc, char **argv)
{
	char buffer[PATH_MAX];
	char userIn[2048];
	cmdLine *line;

	//checking debug flag
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
		if(strcmp(line->arguments[0], "procs") == 0){
			printProcessList(&proces_list);
			freeCmdLines(line);
			continue;
		}
		else if (strcmp(line->arguments[0], "cd") == 0){
			if (chdir(line->arguments[1]) != 0)
				perror("error while trying to change directory\n");
			freeCmdLines(line);
			continue;
		}
		else if (strcmp(line->arguments[0], "quit") == 0){
			freeCmdLines(line);
			freeProcess(proces_list);
			exit(0);
		}
		execute(line);
	}

	return 0;
}
