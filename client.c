#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>

//#define Filename "a.jpg"

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
    char *data;
    //char buffer[256];
    char requestmsg[256];
    char replymsg[256];
    char Filename[256];
    
    int filesize,filedesc;

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

   // while(1)
    //{
      
      strcpy(requestmsg,"Get ");
      printf("Give file name \n");
      scanf("%s",Filename);
      strcat(requestmsg,Filename);
       printf("%s\n",requestmsg);
       printf("%lu\n",strlen(requestmsg));
      write(sockfd,requestmsg,strlen(requestmsg));
      recv(sockfd,replymsg,2,0);
     
      printf("%s\n",replymsg); 
      printf("%d\n",strcmp(replymsg,"OK")); 
      
      
      if(strcmp(replymsg,"OK") == 0)
      {
        printf("ok");
        recv(sockfd,&filesize,sizeof(int),0);
        data = malloc(filesize);
        filedesc=open(Filename, O_CREAT | O_EXCL | O_WRONLY ,0666);
        recv(sockfd,data,filesize,0);
        write(filedesc,data,filesize);
        close(filedesc);
      }
      else
      {
          printf("bad request");
        fprintf(stderr,"Bad request\n");
      }
      
     printf("Transfer closed\n"); 
   // }
    close(sockfd);

return 0;
}

