#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct fun_desc {
  char *name;
  char (*fun)(char);
};

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

/*Set File Name queries the user for a file name, and store it in file_name.
You may assume that the file name is no longer than 100 characters. If debug mode is on,
the function should also print: "Debug: file name set to 'file_name' "
(obviously, replacing 'file_name' with the actual name). 
*/
void setFileName(state* s){
    char userIn[100];
    printf("Enter a file name: ");
    fgets (userIn, 100, stdin);
    strcpy(s->file_name, userIn);
    if(strcmp(s->debug_mode, '1')  == 0)
        printf("Debug: file name set to '%s'\n", s->file_name);
}

void setUnitSize(state* s){
    printf("Enter a number: ");
    char userIn[20]; //for getting user input
    int newSize = 0;
    fgets (userIn, 20, stdin);
    if(strcmp(userIn, '1')  == 0 || strcmp(userIn, '2')  == 0 || strcmp(userIn, '4')  == 0){
        sscanf(userIn, "%d", &newSize);
        s->unit_size = newSize;
        if(strcmp(s->debug_mode, '1') == 0)
            printf("Debug: set size to %d\n", s->unit_size);
    }
    else if (strcmp(s->debug_mode, '1') == 0)
        printf("Error - entered invalid size. No changes have been made\n");
}

int main(int argc, char **argv) {
    struct fun_desc menu[] = { {"Toggle Debug Mode" , NULL}, { "Set File Name", NULL}, { "Set Unit Size", NULL},{ "Quit", NULL}, { NULL, NULL } };
    char userIn[50]; //for getting user input
    int choice; //convert user input into int
    int bound=sizeof(menu)/sizeof(struct fun_desc); //for checking if choice within bounds
    struct fun_desc* funcIndex; //to iterate through menu array
    int debugFlag = 0;

    printf("bound is: %d\n", bound);

    do{
        int i=0;
        printf("Choose action:\n");
        for(funcIndex=menu; funcIndex[i].name!=NULL; i++)
            printf("%d- %s\n",i ,funcIndex[i].name);
        printf("Option: ");
        fgets (userIn, 50, stdin);
        sscanf(userIn, "%d", &choice);
        
        if(!(choice>=0 && choice<=bound-2)) //case NOT in the bounds
        {
            printf("Not within bounds\n");
            exit(0);
        }
        else
            printf("Within bounds\n");


        switch (choice)
        {
        case 0:
            if(debugFlag == 0){
                debugFlag = 1;
                printf("Debug flag now on\n");
            }
            else{
                debugFlag = 0;
                printf("Debug flag now off\n");
            }
            break;
        
        case 1:
            //setFileName(s);
            break;

        case 2:
            //setUnitSize(s);
            break;
        
        default:
            printf("quitting\n");
			exit(0);
            break;
        }

    }while(1);


    return 0;
}

/*
When the debug mode is on, you should print the value of the variables:
unit_size, file_name, and mem_count, every time just before the menu is printed. 
*/