#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main ( )
{
        FILE *file;
        /*unsigned*/ char *buffer;//to get byte or char
          //unsigned char *buffer;//to get as unsigned hex
        char filename[20];
        unsigned long fileLen;
        scanf("%s",filename);
        printf("%s \n",filename);

        //Open file
        file = fopen(filename, "rb");
        if (!file)
        {
                fprintf(stderr, "Unable to open file %s", filename);
                return 0;
        }

        //Get file length
        fseek(file, 0, SEEK_END);
        fileLen=ftell(file);
        fseek(file, 0, SEEK_SET);

        //Allocate memory
        buffer=(char *)malloc(fileLen);
        if (!buffer)
        {
                fprintf(stderr, "Memory error!");
                                fclose(file);
                return 1;
        }

       fread(buffer,fileLen,sizeof(char),file);
       fclose(file);

       int i=0;

       while (i < fileLen){
           printf("%02X ",(buffer[i]));//to get byte
          // printf("%02X ",((unsigned char)buffer[i]));//unsigned hex
           i++;
           if( ! (i % 16) ) printf( "\n" );
       }
       
       printf("\n\n\n%lu\n",fileLen);

        return 0;
}
