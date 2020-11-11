#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

void VirusFree(virus* v){
	if(v != NULL){
		free(v->sig);
		free(v);
	}
}

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};


/* Free the memory allocated by the list. */
void list_free(link* l){
    if (l != NULL) {
        list_free(l->nextVirus);
        VirusFree(l->vir);
        free(l);
    }
}


void PrintHex(char* buffer, int length, FILE* output) {
	int i;
	for (i=0; i<length-1; i++){
		fprintf(output,"%02X ",(unsigned char)buffer[i]);
	}
	fprintf(output,"%02X\n",(unsigned char)buffer[i]);
}

void* readVirus(FILE* file) {
	
	char size[2];

	if(fread(size, 1, 2, file) != 2){
		return NULL;
	}

	virus *v=0;
	int i;
	v = malloc (sizeof (virus));
	if (v == 0)
		printf("No memory!!!");
	char name[16];

	fread(name, 1, 16, file);
	v->SigSize = size[0]+(size[1] *16*16);
	strcpy(v->virusName, name);
	v->sig = malloc(v->SigSize);
	fread(v->sig, 1, v->SigSize, file);
	
	return v;
}

void printVirus(virus* virus, FILE* output){
		
	fprintf(output, "Virus name: %s\n", virus->virusName);

	fprintf(output, "Virus size: %d\n", virus->SigSize);

	fprintf(output, "signature:\n");

	PrintHex(virus->sig,virus->SigSize, output);
	
	fprintf(output, "\n\n");


	
}


/**********1b********/

/* Print the data of every link in list to the given stream. Each item followed by a newline character. */

void list_print(link *virus_list, FILE* output){
	link* temp = virus_list;
	while(temp != NULL)
	{
		if(temp->vir != NULL)
			printVirus(temp->vir, output);
		temp = temp->nextVirus;
	}
}

/* Add a new link with the given data to the list 
        (either at the end or the beginning, depending on what your TA tells you),
        and return a pointer to the list (i.e., the first link in the list).
        If the list is null - create a new entry and return a pointer to the entry. */
link* list_append_start(link* virus_list, virus* data){
	if(virus_list->vir == NULL){
		virus_list->vir = data;
		return virus_list;
	}
	struct link* newLink;
	newLink->vir = data;
	newLink->nextVirus = virus_list;
	return newLink;
} 


link* list_append_end(link* virus_list, virus* data){
	if(virus_list->vir == NULL){
		virus_list->vir = data;
		return virus_list;
	}
	printf("VIRUS NANE: %s\n",virus_list->vir->virusName);
	struct link* newLink=(link*) calloc(1, sizeof(link));
	newLink->vir = data;
	struct link* head = virus_list;
	while(virus_list->nextVirus != NULL)
	{
		virus_list = virus_list->nextVirus;		
	}
	virus_list->nextVirus = newLink;

	return head;
	
} 


struct fun_desc {
  char *name;
  char (*fun)(char);
};


void detect_virus(char *buffer, unsigned int size, link *virus_list){
	printf("******detecting for viruses*******\n");
	int i;
	link* temp = virus_list;	
	while(temp != NULL && temp->vir != NULL){	
			for(i=0; i<size-1; i++){
				if (memcmp(buffer+i, temp->vir->sig, temp->vir->SigSize) == 0){
					printf("Virus was found.\n");
					printf("The starting byte location in the suspected file: %d\n", i);
					printf("Virus name: %s\n", temp->vir->virusName);
					printf("Size of the virus signature: %d\n", temp->vir->SigSize);
				}
			}	
			temp = temp->nextVirus;
	}
}



int main(int argc, char **argv) {
struct fun_desc menu[] = { {"Load signatures" , NULL}, { "Print signatures", NULL}, { "Detect viruses", NULL},{ "Quit", NULL}, { NULL, NULL } };
char userIn[30];
char inPath[30];
char outPath[30];


int choice;
int bound=sizeof(menu)/sizeof(struct fun_desc);


FILE * streamIn;
FILE * streamOut;

virus* vir;
link* list = (link*) calloc(1, sizeof(link));
unsigned char* buffer;



struct fun_desc* funcIndex; //to iterate through menu array


do {

	int i=0;
	printf("Please choose an option:\n");
	for(funcIndex=menu; funcIndex[i].name!=NULL; i++){
		printf("%d) %s\n",i+1 ,funcIndex[i].name);
	} 
	printf("Option: ");
	fgets (userIn, 30, stdin);
	sscanf(userIn, "%d", &choice);
	
	if(!(choice>=1 && choice<=bound-1)) //case NOT in the bounds
	{
		printf("Not within bounds\n");
		exit(0);
	}
	else
		printf("Within bounds\n");
	choice=choice-1;

	switch(choice){
		case 0:
			printf("Please enter name of signatures file:\n");
			fgets (userIn, 30, stdin);
			sscanf(userIn, "%s", inPath);
			streamIn = fopen(inPath, "r");

			printf("streamIn file is: %s\n", inPath);

			while (1){
				vir=readVirus(streamIn);
				if(vir==NULL)
					break;
	
				list=list_append_end(list, vir);
				
			}
			fclose(streamIn);

			break;
		case 1:
			list_print(list, stdout);
			sscanf("output.txt", "%s", outPath);
			printf("streamOut file is: %s\n", outPath);
			streamOut = fopen(outPath, "w");

			list_print(list, streamOut);
			fflush(streamOut);
			fclose(streamIn);

			break;
		case 2:

			printf("Please enter name of suspected file:\n");
			fgets (userIn, 30, stdin);
			sscanf(userIn, "%s", inPath);
			streamIn = fopen(inPath, "r");
			int bytes;
			for(bytes = 0; getc(streamIn) != EOF; ++bytes);
			printf("File size: %d bytes\n",bytes);
			fseek(streamIn, 0, SEEK_SET);
			buffer = malloc(sizeof(char)*bytes);

			fread(buffer, 1, bytes, streamIn);

			

			
			detect_virus(buffer, bytes, list);
			free(buffer);
			fclose(streamIn);

			break;
		
		case 3:
			printf("quitting\n");
			list_free(list);
			exit(0);
			break;
	}
	

} while(1);
    fclose(streamIn);
    fclose(streamOut);


    return 0;
}

