#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<stdbool.h>
#include<pthread.h>
#include<sys/sendfile.h>


/*char* getfilenamefromrequest(char *request)
{
  printf("%s\n",request);
  char *filename=strchr(request,' ');
  return filename+1;
}*/

void sendfiledata(int sockfd,char *filename)
{
        FILE *file;
        /*unsigned*/ char *buffer;
        unsigned long fileLen;

        //Open file
        file = fopen(filename, "rb");
        if (!file)
        {
                fprintf(stderr, "Unable to open file %s", filename);
             //   return 0;
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
              //  return 1;
        }

       fread(buffer,fileLen,sizeof(char),file);
       fclose(file);

       int i=0;

     /*  while (i < fileLen){
           printf("%02X ",(buffer[i]));
           i++;
           if( ! (i % 16) ) printf( "\n" );
       }*/
       write(sockfd,buffer,fileLen);
       
       printf("\n\n\n%lu\n",fileLen);

      //  return 0;

}
bool sendfileoversocket(int sockfd,char *filename)
{
   struct stat ob;
   int filedesc,filesize,size;
   
   stat(filename,&ob);
   filedesc=open(filename,O_RDONLY);
   filesize=ob.st_size;
  // send(sockfd,filedesc,NULL,filesize);
  send(sockfd, &filesize, sizeof(int), 0);
  sendfiledata(sockfd,filename);
   
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
     int sockfd, newsockfd,portNumber,n,clientLen,size;
     char buffer[255];
     
      
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
  
     while(1)
     {
                  char serverresponse[BUFSIZ];
                  char clientrequest[BUFSIZ];
                              memset( serverresponse, '\0', sizeof( serverresponse ));
                              memset( clientrequest, '\0', sizeof( clientrequest ));
      		  
               printf("at first our command%s\n",clientrequest);
	      recv(newsockfd,clientrequest,BUFSIZ,0);
	       // recv(newsockfd,clientrequest,strlen(clientrequest),0);
	        clientrequest[strlen(clientrequest)]='\0';
	       printf("now our client is %s\n",clientrequest);
	       printf(" %lu\n",strlen(clientrequest));
	       printf("at first our server is %s\n",serverresponse);
	      	       
	       if(strcmp(clientrequest,"Get") == 0)   
	       {
		      // strcpy(filename,getfilenamefromrequest(clientrequest));
		       recv(newsockfd,filename,BUFSIZ,0);
		       printf("%s\n",filename);
		       printf("%d\n",access(filename,F_OK) ); 
		       if(access(filename,F_OK) != -1)
		       {
			  printf("it is ok\n");  
			  strcpy(serverresponse, "OK");
			  printf("response msg is %s\n",serverresponse);
			  write(newsockfd,serverresponse, strlen(serverresponse));
			  printf("starting sending\n");
			  sendfileoversocket(newsockfd,filename);
		       
		       }
		       else
		       {
			 strcpy(serverresponse,"NO");
			 write(newsockfd,serverresponse,strlen(serverresponse));
		       }
	       }
	       else if(strcmp(clientrequest,"pwd") == 0)
	       {         
		  char pwd[256];
		  strcpy(serverresponse,getcwd(pwd,sizeof(pwd)));
		  pwd[strlen(pwd)]='\0';
		  printf("%s\n",serverresponse);
		  serverresponse[strlen(serverresponse)]='\0';
		  send(newsockfd, pwd/*serverresponse*/,256,0);
		  printf("%lu\n",strlen(serverresponse));
                              memset( pwd, '\0', sizeof( pwd ));
                              memset( serverresponse, '\0', sizeof( serverresponse ));

	       }
	       else if(strcmp(clientrequest,"ls") == 0)
	       {
	          system("ls > t.txt");
	            struct stat obj;
	          stat("t.txt",&obj);
	          size= obj.st_size;
	          send(newsockfd,&size,sizeof(int),0);
	          int filedesc;
	          filedesc = open("t.txt",O_RDONLY);
	          char str[256];
		        FILE* x = freopen("t.txt","r",stdin);
			    if(x==NULL){
				printf("Error opening file");
				//return(-1);
			    }
			    while(fgets (str, 60,stdin)){
				puts(str);
			    }
	          sendfile(newsockfd,filedesc,NULL,size);
	          printf("the file has been sent\n");
	          remove("t.txt");
	         // return 0;
	       }

               else if(strcmp(clientrequest,"cd") == 0)
               {
                   printf("ready\n");
                   char path[BUFSIZ];
                   int c;
                   recv(newsockfd, path, BUFSIZ,0);
                   printf("%s\n",path);
                   if(chdir(path) == 0)
                   {
		  		  c = 1;
		  	 }
		 	 else c = 0;
		  	 send(newsockfd, &c, sizeof(int), 0);
		  	 path[0]='0';
               }
        
                
	       else if(strcmp(clientrequest,"put") == 0)
	       {
	          char putfilename[BUFSIZ];
	          recv(newsockfd,putfilename,BUFSIZ,0);
	          printf("our file name is %s\n",putfilename);
	          int putfilesize,putfiledesc;
	          char *d;
	          recv(newsockfd,&putfilesize,sizeof(int),0);
	          printf("%d\n",putfilesize);
	          d = malloc(putfilesize);
	          putfiledesc=open(putfilename, O_CREAT | O_EXCL | O_WRONLY ,0666);
		  recv(newsockfd,d,putfilesize,0);
		  write(putfiledesc,d,putfilesize);
		  close(putfiledesc);
		  printf("file has been saved");
	       }
        
	       else  if(strcmp(clientrequest,"quit") == 0 || strcmp(clientrequest," ") == 0) 
	       {
		break;
	       }  
		printf("transfer closed in loop\n");
                              memset( serverresponse, '\0', sizeof( serverresponse ));
                              memset( clientrequest, '\0', sizeof( clientrequest ));
     }
  
  
     printf("transfer closed\n");
     close(newsockfd);
     close(sockfd);
return 0;
}
