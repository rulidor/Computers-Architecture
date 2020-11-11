#include <stdio.h>

//check - is it possible to use string.h?

int digit_cnt(char* input){
    int counter = 0;
    char c;
    int index = 0;
    while( (c = *(input+index) ) != '\0'){
        if(c>='0' && c<='9')
            counter++;
        index++;
    }
    return counter;
}

int main(int argc, char **argv) {
    
    printf("Num. of digits: %d\n", digit_cnt(argv[1]));
    return 0;
}

