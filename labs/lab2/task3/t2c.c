#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//TO CHECK: my_get, encrypt, cprt



/*************t2c************/

/* Gets a char c,  and if the char is 'q' , ends the program with exit code 0. Otherwise returns c. */
char quit(char c){
	if(c=='q')
		exit(0);
	return c;
}





/*************t2b************/
                
/* Gets a char c and returns its encrypted form by adding 3 to its value. 
			If c is not between 0x20 and 0x7E it is returned unchanged */    
char encrypt(char c){
	int cHex=(int)c;
	//printf("DEBUG: encrypt rcbvd char: %c",c);

	if(cHex>0x20 && cHex<0x7E){
		cHex=cHex+3;
	}
	//printf(" ,and encrypted to: %c\n",(char)cHex);
	return (char)cHex;
}

/* Gets a char c and returns its decrypted form by reducing 3 to its value. 
            If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c){
	int cHex=(int)c;
	//printf("%c\told value: 0x%x\n",c,cHex);

	if(cHex>0x20 && cHex<0x7E){
		cHex=cHex-3;
	}
	//printf("%c\t new value: 0x%x\n",c,cHex);
	return (char)cHex;
}


/* dprt prints the value of c in a decimal representation followed by a 
           new line, and returns c unchanged. */      
char dprt(char c){
	printf("%d\n",c);
	return c;
}


/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed 
                    by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns 
                    the value of c unchanged. */      
char cprt(char c){
	int cHex=(int)c;
	//printf("DEBUG: cprt prints: ");
	if(cHex>0x20 && cHex<0x7E){
		printf("%c\n",cHex);
	}
	else{
		printf("%c\n",'.');
	}
	return c;
}


/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c){
	char res;
	res = fgetc(stdin);
	return res;
} 


/*************t2a************/
 
char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}
 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  int i;
  
  for(i=0; i<array_length; i++)
  {
	  *(mapped_array+i)=f(*(array+i));
  }
  //printf("DEBUG: map func. got the string: %s\n", mapped_array);
  
  
  //char* i;
  //int count=0;
  //for (i = array; *i != '\0'; i++) {
	//*(mapped_array+count)=f(*i);
	//count++;
  //}
  return mapped_array;
}





 
//int main(int argc, char **argv){
	
////char arr1[] = {'H','E','Y','!'};
////char* arr2 = map(arr1, 4, censor);
////printf("Test for t2a:\n");
////printf("%s\n", arr2); 
////free(arr2);
////printf("\n");

//printf("Test for t2b:\n");

//int base_len = 5;
//char arr1[base_len];
//char* arr2 = map(arr1, base_len, my_get);
//char* arr3 = map(arr2, base_len, encrypt);
//char* arr4 = map(arr3, base_len, dprt);
//char* arr5 = map(arr4, base_len, decrypt);
//char* arr6 = map(arr5, base_len, cprt);
//free(arr2);
//free(arr3);
//free(arr4);
//free(arr5);
//free(arr6);



//return 0;

//}
