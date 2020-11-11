#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void sighandler(int signum)
{
    printf("Looper handling %s\n", strsignal(signum));
    signal(signum, SIG_DFL);
    raise(signum);
    if(signum==SIGCONT){
		signal(SIGTSTP,sighandler);
	}
	else if (signum==SIGTSTP){
		signal(SIGCONT,sighandler);
	}
}

int main(int argc, char **argv)
{
    printf("Starting the program\n");
    signal(SIGINT, sighandler);
    signal(SIGCONT, sighandler);
    signal(SIGTSTP, sighandler);

    while (1)
    {
        sleep(2);
    }

    return 0;
}

