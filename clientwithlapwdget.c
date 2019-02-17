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
      char replymsgpwd[256];
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

    while(1)
    {
               char requestmsg[256];
               char replymsg[256];

 	      printf("Give command \n");
	      scanf("%s",requestmsg);
	      requestmsg[strlen(requestmsg)]='\0';
	      printf("%lu\n",strlen(requestmsg));
	     
	      printf("our request msg is %s\n",requestmsg);
	        replymsg[0]='\0';
	        printf("first reply msg %s\n",replymsg);
	      
	      if(strcmp(requestmsg,"Get") == 0)   
	      {
		      //strcpy(requestmsg,"Get ");
		      write(sockfd,requestmsg,strlen(requestmsg));
		      printf("Give file name \n");
		      scanf("%s",Filename);
		      printf("%s\n",Filename);
		      write(sockfd,Filename,256);
		      // strcat(requestmsg,Filename);
		      
		      printf("%lu\n",strlen(requestmsg));
		     
		      printf("our reply msg before receiving reply msg %s\n",replymsg);
		      recv(sockfd,replymsg,2,0);
		      replymsg[2]='\0';
		     
		      printf("OUR REPLY MSG IS %s\n",replymsg); 
		      printf("Result of string compare %d\n",strcmp(replymsg,"OK")); 
		      
		      
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
	     } 
	     
	     else if(strcmp(requestmsg,"pwd") == 0)
	     {
			write(sockfd,requestmsg,strlen(requestmsg));
		        recv(sockfd,replymsgpwd,256,0);
		        printf("%lu\n",strlen(replymsgpwd));
		        printf("%s\n",replymsgpwd);		        
	     }
	     else if(strcmp(requestmsg,"ls")==0)
	     {
	                int size,filehandle;
	                char *f;
	                char lsfilename[10];
	                strcpy(lsfilename,"t.txt");
	               	//write(sockfd,requestmsg,strlen(requestmsg));
	               	send(sockfd,requestmsg,256,0);
	               	 printf("for ls %s\n",requestmsg);
	               	 printf("for ls %lu\n",strlen(requestmsg));
	               	recv(sockfd, &size, sizeof(int), 0);
	               	
	               	f=malloc(size);
	               	filehandle = open(lsfilename, O_CREAT | O_EXCL | O_WRONLY ,0666);
	               	recv(sockfd, f, size, 0);
	              
	                write(filehandle, f, size);
	              
	                printf("The remote directory listing is as follows:\n");
                        //printf("\n");
	                system("cat t.txt");
	        
	     }
	     else if(strcmp(requestmsg,"quit")==0)
	     {
		       	if(write(sockfd,requestmsg,strlen(requestmsg)))
		       	{
		       	  printf("walla");
		       	}
		       	
		       	break;
	     }
	     
	    // printf("Transfer closed\n");
	     printf("loop is breaking \n");
	        replymsg[0]='\0';
	        requestmsg[0]='\0';
	        Filename[0]='\0';
	   
	  
   }
    close(sockfd);
printf("socket closed\n");
return 0;
}

