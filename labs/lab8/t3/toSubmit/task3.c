#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <sys/types.h>
#include <fcntl.h>

#include "elf.h"

int debugFlag = 0;
int Currentfd=-1;
void *map_start; /* will point to the start of the memory mapped file */
struct stat fd_stat; /* this is needed to  the size of the file */
Elf32_Ehdr *header; /* this will point to the header structure */
int num_of_section_headers;
// int symbolTabStr_p=NULL;


struct fun_desc {
  char *name;
  char (*fun)(char);
};

void examineELF(){
    char userIn[100];
    printf("Enter a file name: ");
    fgets (userIn, 100, stdin);
    sscanf(userIn, "%s", userIn);
    if(debugFlag == 1)
        printf("Debug: file name received is '%s'\n", userIn);

    close(Currentfd);
       if( (Currentfd = open(userIn, O_RDWR)) < 0 ) {
      perror("error in open");
      Currentfd=-1;
   }

   if( fstat(Currentfd, &fd_stat) != 0 ) {
      perror("stat failed");
      exit(-1);
   }

   if ( (map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, Currentfd, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      exit(-4);
   }

   /* now, the file is mapped starting at map_start.
    * all we need to do is tell *header to point at the same address:
    */

   header = (Elf32_Ehdr *) map_start;
   /* now we can do whatever we want with header!!!!
    * for example, the number of section header can be obtained like this:
    */

   num_of_section_headers = header->e_shnum;


   /* now, we unmap the file */
//    munmap(map_start, fd_stat.st_size);

    if(((char*)map_start)[0]!=0x7f || ((char*)map_start)[1]!=0x45 || ((char*)map_start)[2]!=0x4c || ((char*)map_start)[3]!=0x46){
        printf("Not an ELF file\n");
        munmap(map_start, fd_stat.st_size);
        close(Currentfd);
        Currentfd=-1;
        return;
    }

    printf("Bytes 1,2,3 of the magic number (in ASCII): ");
    for(int i=0; i<4; i++)
        printf("%c ", ((char*)map_start)[i]);
    printf("\n");

    if(header->e_ident[EI_DATA] == 1)
        printf("Data encoding scheme of the object file: 2's complement, little endian\n");
    else if(header->e_ident[EI_DATA] == 2)
        printf("Data encoding scheme of the object file: 2's complement, big endian\n");

    printf("File offset in which the section header table resides: %d\n", header->e_shoff);

    printf("Number of section header entries: %d\n", header->e_shnum);

    printf("Size of each section header entry: %d\n", header->e_shentsize);

    printf("File offset in which the program header table resides: %d\n", header->e_phoff);

    printf("Number of program header entries: %d\n", header->e_phnum);

    printf("Size of each program header entry: %d\n", header->e_phentsize);

    printf("Entry point (in hexadecimal): 0x%X\n", header->e_entry);

}

void printSectionsNames(){
    if(Currentfd<0){
        printf("Error - Currentfd < 0\n");
        return;
    }
    Elf32_Shdr *shdr = (Elf32_Shdr *)(map_start + header->e_shoff); //points to sections
    int shnum = header->e_shnum;

    Elf32_Shdr *sh_strtab = &shdr[header->e_shstrndx]; //points to section names string table
    const char *const sh_strtab_p = map_start + sh_strtab->sh_offset; //points to string table
    if(debugFlag==1){
        printf("shstrndx: %d\n", header->e_shstrndx);
    }
    printf("<index> <name> <address> <offset> <size> <type>\n");
    for (int i = 0; i < shnum; ++i) {
        printf("[%2d] %s %x %x %x %d", i, sh_strtab_p + shdr[i].sh_name, shdr[i].sh_addr, shdr[i].sh_offset, shdr[i].sh_size,
        shdr[i].sh_type);
        if(debugFlag==1){
            printf(" section name offset: %d", shdr[i].sh_name);
        }
        printf("\n");
        // if(strcmp(sh_strtab_p + shdr[i].sh_name, ".strtab") == 0)
        //     symbolTabStr_p = map_start + shdr[i].sh_offset;

    }

}

void printSymbols(){
    if(Currentfd<0){
        printf("Error - Currentfd < 0\n");
        return;
    }
    printf("<index> <value> <section_index> <section_name> <symbol_name>\n");
    Elf32_Shdr *shdr = (Elf32_Shdr *)(map_start + header->e_shoff); //points to sections
    Elf32_Sym *symTable;
    char *symStrTab_p=NULL;
    int symbolNum=-1;
    int linkToStrTable=-1;
    for (int i = 0; i < header->e_shnum; i++){
        if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM) {
            symTable = (Elf32_Sym *)((char *)map_start + shdr[i].sh_offset);
            symbolNum = shdr[i].sh_size / shdr[i].sh_entsize;
            linkToStrTable=shdr[i].sh_link;
            symStrTab_p = map_start + shdr[linkToStrTable].sh_offset; //points to string table of symbol table
            printf("symbol size: %d\n",symbolNum);
            char *strtab=".symtab";
            if(shdr[i].sh_type == SHT_DYNSYM)
                strtab=".dynsym";
            
            for(int j=0; j<symbolNum; j++){
                printf("[%2d] %x %d %s %s", j, symTable[j].st_value, symTable[j].st_shndx,strtab , symStrTab_p + symTable[j].st_name);
                if(debugFlag==1)
                    printf(" symbol size: %d", symTable[j].st_size);
                printf("\n");
            }

        }
    }

}

void printRelocation(){

    Elf32_Shdr *shdr = (Elf32_Shdr *)(map_start + header->e_shoff); //points to sections
    Elf32_Rel *relocationTable;
    int entryNum=-1;
    int relocNum=-1; //num. of entries in relocation
    int linkToStrTable=-1;
    char *strTab_p=NULL; //pointer for .dynstr
    for (int i = 0; i < header->e_shnum; i++){
        if (shdr[i].sh_type == SHT_REL) {
            relocationTable = (Elf32_Rel *)((char *)map_start + shdr[i].sh_offset);
            entryNum = shdr[i].sh_size / shdr[i].sh_entsize;
            linkToStrTable = shdr[i].sh_link;
            strTab_p = map_start + shdr[linkToStrTable].sh_offset;

            break; 
            }
    }

    // //getting symbol table and strTable of symTable
    // Elf32_Shdr *shdr = (Elf32_Shdr *)(map_start + header->e_shoff); //points to sections
    // Elf32_Sym *symTable;
    // char *symStrTab_p=NULL;
    // int symbolNum=-1;
    // int linkToStrTable=-1;
    // for (int i = 0; i < header->e_shnum; i++){
    //     if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM) {
    //         symTable = (Elf32_Sym *)((char *)map_start + shdr[i].sh_offset);
    //         symbolNum = shdr[i].sh_size / shdr[i].sh_entsize;
    //         linkToStrTable=shdr[i].sh_link;
    //         symStrTab_p = map_start + shdr[linkToStrTable].sh_offset; //points to string table of symbol table
    //         break;
    //     }
    // }


    printf("<offset> <info> <type> <value>\n");
    for(int i=0; i<entryNum; i++){
        printf("%x\t %x\t %x\t", relocationTable[i].r_offset, relocationTable[i].r_info, ELF32_R_TYPE(relocationTable[i].r_info));
        int symTableIndex= ELF32_R_SYM(relocationTable[i].r_info); //index in the symTable

        printf("\n");
    }


}

int main(int argc, char **argv) {
    struct fun_desc menu[] = { {"Toggle Debug Mode" , NULL}, {"Examine ELF File", NULL}, {"Print Section Names", NULL},
    {"Print Symbols"},{"Relocation Tables", NULL}, { "Quit", NULL}, { NULL, NULL } };
    char userIn[50]; //for getting user input
    int choice; //convert user input into int
    int bound=sizeof(menu)/sizeof(struct fun_desc); //for checking if choice within bounds
    struct fun_desc* funcIndex; //to iterate through menu array
    

    do{
        if(debugFlag == 1)
            printf("Debug: Currentfd=%d\n", Currentfd);
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
            examineELF();
            break;

        case 2:
            printSectionsNames();
            break;
        
        case 3:
            printSymbols();
            break;
        
        case 4:
            printRelocation();
            break;

        
        default:
        /*Quit should unmap and close any mapped or open files, and "exit normally"*/
            munmap(map_start, fd_stat.st_size);
            close(Currentfd);
            printf("quitting\n");
			exit(0);
            break;
        }

    }while(1);


    return 0;
}