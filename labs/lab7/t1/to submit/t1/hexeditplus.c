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
    sscanf(userIn, "%s", userIn);
    strcpy(s->file_name, userIn);
    if(s->debug_mode == '1')
        printf("Debug: file name set to '%s'\n", s->file_name);
}

void setUnitSize(state* s){
    printf("Enter a number: ");
    char userIn[20]; //for getting user input
    int newSize = 0;
    fgets (userIn, 20, stdin);
    if(strcmp(userIn, "1")  == 0 || strcmp(userIn, "2")  == 0 || strcmp(userIn, "4")  == 0){
        sscanf(userIn, "%d", &newSize);
        s->unit_size = newSize;
        if(s->debug_mode == '1')
            printf("Debug: set size to %d\n", s->unit_size);
    }
    else if (s->debug_mode == '1')
        printf("Error - entered invalid size. No changes have been made\n");
}

void loadToMemory(state* s){
    if(strlen(s->file_name) == 0){
        printf("Error - file name is empty.\n");
        return;
    }
    FILE * fp;    
    fp = fopen (s->file_name, "r");

    if(fp == NULL){
        printf("Error - could not open file.\n");
        perror("fopen");
        return;
    }
    printf("Please enter <location> <length>\n"); //location (in hexadecimal) and length (in decimal).
    char userIn[100]; //for getting user input
    fgets (userIn, 100, stdin);
    //char * splited = strtok(userIn, " ");
    unsigned int location=-1;
    int len=-1;
    sscanf(userIn, "%X %d", &location, &len);
    //splited = strtok(NULL, " ");
    //sscanf(splited, "%d", &len);
    printf("location: %d, length: %d\n", location, len);
    fseek(fp, location, SEEK_SET);
    fread(s->mem_buf, len, s->unit_size, fp);
    printf("Loaded %d units into memory\n", len);

    fclose(fp);
}

void memoryDisplay(state* s, int displayFlag){
    printf("Enter 'u' - number of units (in decimal): ");
    char userIn[100]; //for getting user input
    int numUnits=-1; //num of units
    unsigned int addr=-1; //starting address
    fgets (userIn, 100, stdin);
    sscanf(userIn, "%d", &numUnits);
    printf("Enter starting address (in hexa): ");
    fgets (userIn, 100, stdin);
    sscanf(userIn, "%X", &addr);

    char* buffer = (char*) s->mem_buf + addr;
    char* end = buffer+addr + s->unit_size*numUnits;
    while (buffer < end) {
        //print ints
        int var = *((int*)(buffer));
        if(displayFlag == 0)
            fprintf(stdout, "%hhd\n", var);
        else
            fprintf(stdout, "%hhX\n", var);
        buffer += s->unit_size;
    }
}

void saveToFile(state* s){
    printf("Please enter: <source-address> <target-location> <length>\n");
    char userIn[100]; //for getting user input
    fgets (userIn, 100, stdin);
    unsigned int sourceAddr = -1;
    unsigned int targetLoc = -1;
    int len = -1;
    sscanf(userIn, "%X %X %d", &sourceAddr, &targetLoc, &len);
    printf("entered: sourceAddr: %X, targetLoc: %X, len: %d\n", sourceAddr, targetLoc, len);

    FILE* fp = fopen(s->file_name, "r+");
    if (fp == NULL){
        printf("Error - could not open file.\n");
        perror("fopen");
        return;
    }
    
    //checking that target is not greater than size of file
    /*Move file point at the end of file.*/
    fseek(fp,0,SEEK_END); 
    /*Get the current position of the file pointer.*/
    int size=ftell(fp);
    if(targetLoc > size){
        printf("Error - target is greater than size of file\n");
        return;
    }
    fseek(fp, targetLoc, SEEK_SET);

    if(sourceAddr == 0)
        fwrite(s->mem_buf, s->unit_size, len, fp);
    else
        fwrite((void*)s->mem_buf, s->unit_size, len, fp);

    fclose(fp);
}

void memoryModify(state* s, int debugFlag){
    printf("Please enter <location> <val>\n"); //in hexa
    char userIn[100]; //for getting user input
    fgets (userIn, 100, stdin);
    unsigned int location = -1;
    unsigned int val = -1;
    sscanf(userIn, "%X %X", &location, &val);
    if(debugFlag == 1)
        printf("location: %X, val: %X\n", location, val);
    
    printf("before modifying:\n");
    char* buffer = (char*) s->mem_buf;
    char* end = buffer + s->unit_size*10;
    while (buffer < end) {
        //print ints
        int var = *((int*)(buffer));
        fprintf(stdout, "%hhX\n", var);
        buffer += s->unit_size;
    }

    for(int i=0; i<s->unit_size; i++){

        s->mem_buf[location + i] = val;
        val = val / 256;
    }
     //********check for correctness

    printf("after modifying:\n");
    char* buffer2 = (char*) s->mem_buf;
    char* end2 = buffer2 + s->unit_size*10;
    while (buffer2 < end2) {
        //print ints
        int var = *((int*)(buffer2));
        fprintf(stdout, "%hhX\n", var);
        buffer2 += s->unit_size;
    }

}


int main(int argc, char **argv) {
    state* s = (state*)malloc(sizeof(state));
    s->unit_size = 1; //default
    s->mem_count = 0;
    struct fun_desc menu[] = { {"Toggle Debug Mode" , NULL}, { "Set File Name", NULL}, { "Set Unit Size", NULL}, {"Load Into Memory", NULL},
    {"Toggle Display Mode", NULL}, {"Memory Display", NULL},
    {"Save Into File", NULL}, {"Memory Modify", NULL}, { "Quit", NULL}, { NULL, NULL } };
    char userIn[50]; //for getting user input
    int choice; //convert user input into int
    int bound=sizeof(menu)/sizeof(struct fun_desc); //for checking if choice within bounds
    struct fun_desc* funcIndex; //to iterate through menu array
    int debugFlag = 0;
    int displayFlag = 0; // 0 - show in decimal. 1 - show in hexa

    printf("bound is: %d\n", bound);

    do{
        if(debugFlag == 1)
            printf("Debug: unit_size=%d, file_name=%s, mem_count=%d\n", s->unit_size, s->file_name, s->mem_count);
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
            setFileName(s);
            break;

        case 2:
            setUnitSize(s);
            break;

        case 3:
            loadToMemory(s);
            break;
        
        case 4:
            if(displayFlag == 0){
                displayFlag = 1;
                printf("Display flag now on, hexadecimal representation\n");
            }
            else{
                displayFlag = 0;
                printf("Display flag now off, decimal representation\n");
            }
            break;

        case 5:
            memoryDisplay(s, displayFlag);
            break;

        case 6:
            saveToFile(s);
            break;

        case 7:
            memoryModify(s, debugFlag);
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