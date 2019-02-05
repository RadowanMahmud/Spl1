#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<stdbool.h>
#include<pthread.h>
#include<sys/sendfile.h>


char* getfilenamefromrequest(char *request)
{
  printf("%s\n",request);
  char *filename=strchr(request,' ');
  return filename+1;

}

bool sendfileoversocket(int sockfd,char *filename)
{
   struct stat ob;
   int filedesc,filesize;
   
   stat(filename,&ob);
   filedesc=open(filename,O_RDONLY);
   filesize=ob.st_size;
  // send(sockfd,filedesc,NULL,filesize);
  send(sockfd, &filesize, sizeof(int), 0);
  sendfile(sockfd, filedesc, NULL, filesize);
   
   return true;
}


void error (const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc,char *argv[])
{
     if(argc<2)
     {
       fprintf(stderr,"port not found");
     }
     int sockfd, newsockfd,portNumber,n,clientLen;
     char buffer[255];
     
     
     char serverresponse[BUFSIZ];
     char clientrequest[BUFSIZ];
     char filename[BUFSIZ];
     
     
     struct sockaddr_in serverAddr, clientAddr;
     //socketlen_t clientLen;

     sockfd=socket(AF_INET, SOCK_STREAM ,0);

     if(sockfd<0)
     {
        error("opening socket");
     }
   
     bzero((char *) & serverAddr , sizeof(serverAddr));
     portNumber = atoi(argv[1]);

     serverAddr.sin_family= AF_INET;
     serverAddr.sin_addr.s_addr = INADDR_ANY;
     serverAddr.sin_port = htons(portNumber);

     if(bind(sockfd , (struct sockaddr *) &serverAddr , sizeof(serverAddr))<0)
     {
      error("binding failed");
     }

     listen (sockfd,5);
     clientLen= sizeof(clientAddr);

     newsockfd= accept(sockfd , (struct sockaddr*) &clientAddr, &clientLen);

     if(newsockfd<0)
     {
       error("didn't accept");
     }
  
    // while(1)
    // {
       recv(newsockfd,clientrequest,BUFSIZ,0);
        printf("%s\n",clientrequest);
       strcpy(filename,getfilenamefromrequest(clientrequest));
        printf("%s\n",getfilenamefromrequest(clientrequest) ); 
         printf("%s\n",filename);
        printf("%d\n",access(filename,F_OK) ); 
       if(access(filename,F_OK) != -1)
       {
       +
          printf("it is ok\n");  
          strcpy(serverresponse, "OK");
          write(newsockfd,serverresponse, strlen(serverresponse));
          printf("starting sending\n");
          sendfileoversocket(newsockfd,filename);
       
       }
       else
       {
         strcpy(serverresponse,"NO");
         write(newsockfd,serverresponse,strlen(serverresponse));
       }
        printf("transfer closed in loop\n");
  //   }
     printf("transfer closed\n");
     close(newsockfd);
     close(sockfd);
return 0;
}
