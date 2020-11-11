#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "LineParser.h"

#include <sys/wait.h>

#define PATH_MAX 4096 /* # chars in a path name including nul */

int debugFlag = 0;

void executeBlocking(cmdLine *pCmdLine, int debugFlag)
{
	printf("Blocking is executed!\n");
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
		printf("cmd is: %s", *pCmdLine->arguments); //for debugging
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
	if (strcmp(pCmdLine->arguments[0], "quit") == 0)
		exit(0);

	//checking debug flag


	//handling "cd" cmd
	if (strcmp(pCmdLine->arguments[0], "cd") == 0)
	{
		if (chdir(pCmdLine->arguments[1]) != 0)
			perror("error while trying to change directory\n");
		return;
	}

	if (debugFlag == 1)
		fprintf(stderr, "Executing command: %s, parent process ID: %d\n", pCmdLine->arguments[0], getpid());
	if (pCmdLine->blocking == 1)
		return executeBlocking(pCmdLine, debugFlag);
	//execv(pCmdLine->arguments[0], pCmdLine->arguments);
	if (fork() == 0)
	{ //case we're at the child process
		if (debugFlag == 1)
			fprintf(stderr, "child process ID: %d\n", getpid());
		printf("NON blocking is executed!\n");
		fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
		execvp(pCmdLine->arguments[0], pCmdLine->arguments);
		perror("Return not expected. Must be an execv error\n"); //A successful call to execv does not have a return
																 //value because the new process image overlays the calling process image
		_exit(1);
	}

	//else - we're at the parent process
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
		execute(line);
		freeCmdLines(line);
	}

	return 0;
}
