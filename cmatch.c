#include<stdio.h>
#include<string.h>
int main()
{
   char *file="hellotxt.c";
   char *f1=".txt";
   char *f2=".";
   
   if(strstr(file,f1) || !(strstr(file,f2)))
   {
     printf("it works\n");
   }
}

