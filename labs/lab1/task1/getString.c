#include <stdio.h>
#include <stdlib.h>

void getString(){
	char c;
	int charAsci=0;
	
	while((c=fgetc(stdin))!=EOF)  
		{	
			charAsci=(int)c;
			if(charAsci>96 && charAsci<123)
			{
					charAsci=charAsci-32;
			}
			printf("%c",(char)charAsci);
		}

  
}
