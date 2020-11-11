#include <stdio.h>
#include <string.h>
#define	MAX_LEN 34			/* maximal input string size */

					/* enough to get 32-bit string + '\n' + null terminator */
extern int convertor(char* buf);

int main(int argc, char** argv)
{
  char buf[MAX_LEN ];
  while(1){
    fgets(buf, MAX_LEN, stdin);		/* get user input string */

    size_t len = strlen(buf); /*cut '\n'  (if exist)*/
    if(buf[len-1] == '\n'){
       buf[len-1] = '\0';           
    }

    if(strcmp(buf,"q") == 0){
      break; 
    }
    convertor(buf);			/* call your assembly function */
  }
  return 0;
}
