/*
 * mypipe.c
 * 
 * Copyright 2020 caspl202 <caspl202@caspl202-lubuntu>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	char * argForChild1[] = {"ls","-l",'\0'};
	char * argForChild2[] = {"tail","-n","2",'\0'};
	int fd[2];
	int debugMode = 0;
	for (int i = 1; i<argc; i++){
		if (!strcmp(argv[i],"-d")){
			debugMode = 1;
		}
	}
	if (pipe(fd) == -1){
		fprintf(stderr,"pipe faild\n");
		exit(0);
	}
	if (debugMode) fprintf(stderr,"parent_process>forking…\n");
	int childPID1 = fork();
	if (debugMode) fprintf(stderr,"parent_process>created process with id: %d\n", childPID1);
	
	if (!childPID1){//==0 then we are in the child process
		close(1);//close to write
		
		if (debugMode) fprintf(stderr,"child1>redirecting stdout to the write end of the pipe…\n");
		dup(fd[1]);//duplicate write-end
		close(fd[1]);//closing fileDesc after dup
		
		if (debugMode) fprintf(stderr,"child1>going to execute cmd: ls -l\n");
		execvp(argForChild1[0],argForChild1);
		perror("Return not expected. Must be an execv error\n");
		_exit(1);
	}
	else{//we're in the parent
		if (debugMode) fprintf(stderr,"parent_process>waiting for child processes to terminate…");
		int status1;
		waitpid(childPID1,&status1,0);
		
		int status2;
		if (debugMode) fprintf(stderr,"parent_process>closing the write end of the pipe…\n");
		close(fd[1]);//step 4 - close to write, closing standart output
		if (debugMode) fprintf(stderr,"parent_process>forking…\n");
		int childPID2 = fork();
		if (debugMode) fprintf(stderr,"parent_process>created process with id: %d\n", childPID2);
		
		if (!childPID2){//==0 then we are in the child process
			close(0);//close to read, closing standart input
			
			if (debugMode) fprintf(stderr,"child2>redirecting stdout to the read end of the pipe…\n");
			dup(fd[0]);//duplicate read-end
			close(fd[0]);//closing fileDesc after dup
			
			if (debugMode) fprintf(stderr,"child2>going to execute cmd: tail -n 2\n");
			execvp(argForChild2[0],argForChild2);
			perror("Return not expected. Must be an execv error\n");
			_exit(1);
		}
		
		else{//childPID2>0
			
			if (debugMode) fprintf(stderr,"parent_process>closing the read end of the pipe…\n");
			close(fd[0]);//step 7 - close to read, closing standart input
			
			if (debugMode) fprintf(stderr,"parent_process>waiting for child processes to terminate…\n");
			waitpid(childPID2,&status2,0);
			
			
			if (debugMode) fprintf(stderr,"parent_process>exiting…\n");
		}
	}
	return 0;
}

