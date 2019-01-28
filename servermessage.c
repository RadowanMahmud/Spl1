#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<string.h>

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
  
     while(1)
     {
       bzero(buffer,255);
       n= read(newsockfd , buffer ,255);
       if(n<0)
       {
         error("can't read");
       }
       printf("client : %s\n", buffer);
       bzero(buffer,255);
       fgets(buffer,255,stdin);

       n= write(newsockfd,buffer,strlen(buffer));
       if(n<0)
       {
         error(" can;t write");
       }

       int t=strncmp("bye" , buffer ,3);
       if(t==0) break;
     }
     close(newsockfd);
     close(sockfd);
return 0;
}
