#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "LineParser.h"

#define PATH_MAX 4096	/* # chars in a path name including nul */

int main(int argc, char **argv){
    char buffer[PATH_MAX];
    char userIn[2048];
    cmdLine *line = NULL;
    
    while(1){
        getcwd(buffer, PATH_MAX);
        printf("current working direcoty: %s\n", buffer);
        fgets (userIn, 2048, stdin);
        line = parseCmdLines(userIn);
        execute(line);
    }

    return 0;
}

void execute(cmdLine *pCmdLine){
    if (strcmp(pCmdLine->arguments[0], "quit") == 0)
        exit(0);
    //execv(pCmdLine->arguments[0], pCmdLine->arguments);
    execvp(pCmdLine->arguments[0], pCmdLine->arguments);
    perror("Return not expected. Must be an execv error\n"); //A successful call to execv does not have a return
                                                            //value because the new process image overlays the calling process image
    freeCmdLines(pCmdLine);
}
