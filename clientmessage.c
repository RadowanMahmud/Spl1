#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
int main(int argc,char *argv[])
{
    int sockfd,portNumber,n;
    struct sockaddr_in serverAddr;
    struct hostent *server;

    char buffer[256];

    if(argc<3)
    {
      fprintf(stderr,"usage %s hostname port \n",argv[0]);
      exit(0);     
    }

    portNumber=atoi(argv[2]);
    sockfd=socket(AF_INET, SOCK_STREAM , 0);
   
    if(sockfd < 0)
    {
      error("Couln't open socket ");
      exit(1);
    }

    server = gethostbyname(argv[1]);
    if(server == NULL)
    {
      fprintf(stderr,"Error , no such id");
      exit(1);
    }

    bzero((char *) &serverAddr , sizeof(serverAddr));
    serverAddr.sin_family= AF_INET;
    bcopy((char *) server->h_addr,(char *) &serverAddr.sin_addr.s_addr, server->h_length);
    serverAddr.sin_port=htons(portNumber);

    if(connect(sockfd , (struct sockaddr *) &serverAddr, sizeof(serverAddr)) <0)
    {
      error("No onnection");
    }

    while(1)
    {
      bzero(buffer,255);
      fgets(buffer,255,stdin);
      n=write(sockfd,buffer, strlen(buffer));
      if(n<0) 
      {
 	error("can not write");
      }

      bzero(buffer,255);
      n=read(sockfd,buffer,255);
      if(n<0)
      {
	error("can not read");
      }
       printf("server : %s\n", buffer);

      int i=strncmp("ses",buffer,3);
      if(i==0)
      {
	break;
      }
    }
    close(sockfd);

return 0;
}
