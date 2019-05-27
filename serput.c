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


char* getfilenamefromrequest(char *request)
{
  printf("%s\n",request);
  char *filename=strchr(request,' ');
  return filename+1;
}

void recvtxtfile(int sockfd,char *Filename)
{
       int filedesc,filesize,size;
       char *data;
       recv(sockfd,&filesize,sizeof(int),0);
	 data = malloc(filesize);
	 filedesc=open(Filename, O_CREAT | O_EXCL | O_WRONLY ,0666);
	 recv(sockfd,data,filesize,0);
       write(filedesc,data,filesize);
}

void  sendtxtfile(int newsockfd,char *filename)
{
                struct stat object;
                int filedesc,filesize,size;
   
                stat(filename,&object);
                filedesc=open(filename,O_RDONLY);
                filesize=object.st_size;
                // send(sockfd,filedesc,NULL,filesize);
                send(newsockfd, &filesize, sizeof(int), 0);
                sendfile(newsockfd, filedesc, NULL, filesize);

	          printf("the file has been sent\n");
}

void sendfiledata(int sockfd,char *filename)
{
        FILE *file;
        char *buffer;
        unsigned long fileLen;

        //Open file
        file = fopen(filename, "rb");
        if (!file)
        {
                fprintf(stderr, "Unable to open file %s", filename);
              //  return 0;
        }

        //Get file length
        fseek(file, 0, SEEK_END);
        fileLen=ftell(file);
        fseek(file, 0, SEEK_SET);

        long int loop=fileLen/100;
        long int remainfile=fileLen%100;
       
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

    
        send(sockfd, &fileLen, sizeof(unsigned long), 0);
        int i=0;
        char chunk[100];
        for(int j=0;j<loop;j++)
        {
            int k=0;
            while (i < fileLen && k<100)
            {
            	chunk[k]=buffer[i];
            	i++;
            	k++;
            }
            send(sockfd, chunk,100,0);
        }
        if(remainfile>0)
        {
            int k=0;
            while (i < fileLen && k<remainfile)
            {
            	chunk[k]=buffer[i];
           		i++;
           		k++;
            }
            send(sockfd, chunk,remainfile,0);
        }
    //   write(sockfd,buffer,fileLen);
       
         int s=0;
         while (s < fileLen)
         {
           printf("%02X ",(buffer[s]));
           s++;
           if( ! (s % 16) )
           {
           		 printf( "\n" );
           }
         }
       
       printf("\n\n\n%lu\n",fileLen);
       memset( filename, '\0', sizeof( filename ));
      //  return 0;

}

bool sendfileoversocket(int sockfd,char *filename)
{
  sendfiledata(sockfd,filename);
   
   return true;
}


void error (const char *msg)
{
    perror(msg);
    exit(1);
}

bool operations(int newsockfd)
{
             char filename[BUFSIZ];     
             char serverresponse[BUFSIZ];
             char clientrequest[BUFSIZ];
             char command[10];
             int size;
             memset( serverresponse, '\0', sizeof( serverresponse ));
             memset( clientrequest, '\0', sizeof( clientrequest ));
      		  
            printf("at first our command%s\n",clientrequest);
	      recv(newsockfd,clientrequest,BUFSIZ,0);
	       // recv(newsockfd,clientrequest,strlen(clientrequest),0);
	       clientrequest[strlen(clientrequest)]='\0';
	       printf("now our client is %s\n",clientrequest);
	       printf(" %lu\n",strlen(clientrequest));
	       printf("at first our server is %s\n",serverresponse);
	       
	             sscanf(clientrequest, "%s", command);
	      	       
	       if(strcmp(command,"Get") == 0)   
	       {
	             memset( filename, '\0', sizeof( filename ));
		       strcpy(filename,getfilenamefromrequest(clientrequest));
		       //recv(newsockfd,filename,BUFSIZ,0);
		       printf("%s\n",filename);
		       printf("%d\n",access(filename,F_OK) ); 
		       if(access(filename,F_OK) != -1)
		       {
			  	printf("it is ok\n");  
			  	strcpy(serverresponse, "OK");
			  	printf("response msg is %s\n",serverresponse);
			  	write(newsockfd,serverresponse, strlen(serverresponse));
			  	printf("starting sending\n");
			  	char *f1=".txt";
                        char *f2=".";
			  	if(strstr(filename,f1) || !(strstr(filename,f2)))
				{
                              sendtxtfile(newsockfd,filename);
			      }
			      else
			      {
			         	sendfileoversocket(newsockfd,filename);
			      }

		       
		       }
		       else
		       {
			 	strcpy(serverresponse,"NO");
			 	write(newsockfd,serverresponse,strlen(serverresponse));
		       }
		       memset( serverresponse, '\0', sizeof( serverresponse ));
                   memset( clientrequest, '\0', sizeof( clientrequest ));
		       
		       return false;
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
		         memset( serverresponse, '\0', sizeof( serverresponse ));
                   memset( clientrequest, '\0', sizeof( clientrequest ));
		      
		        return false;

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
			    if(x==NULL)
			    {
				printf("Error opening file");
				//return(-1);
			    }
			    while(fgets (str, 60,stdin))
			    {
				puts(str);
			    }
	          sendfile(newsockfd,filedesc,NULL,size);
	          printf("the file has been sent\n");
	          remove("t.txt");
	         // return 0;
	          memset( serverresponse, '\0', sizeof( serverresponse ));
                   memset( clientrequest, '\0', sizeof( clientrequest ));
		      
	         return false;
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
		  	 memset( path, '\0', sizeof( path ));
		  	  memset( serverresponse, '\0', sizeof( serverresponse ));
                   memset( clientrequest, '\0', sizeof( clientrequest ));
		      
		  	 return false;
             }
        
                
	       else if(strcmp(command,"put") == 0)
	       {
			    unsigned long filesize;
			    char putfilename[BUFSIZ];
			    memset( putfilename, '\0', sizeof( putfilename ));
			    strcpy(putfilename,getfilenamefromrequest(clientrequest));
				 //recv(newsockfd,filename,BUFSIZ,0);
				 printf("%s\n",putfilename);
			      char *f3=".txt";
                        char *f4=".";
			  	if(strstr(putfilename,f3) || !(strstr(putfilename,f4)))
				{
                              recvtxtfile(newsockfd,putfilename);
			      }
			      else
			      {
			             recv(newsockfd,&filesize,sizeof(unsigned long),0);
					 printf("size of file is %lu \n",filesize);
			    		 char *buffer=(char *)malloc(filesize);
					//filedesc=open(Filename, O_CREAT | O_EXCL | O_WRONLY ,0666);
			    		FILE *getfile;
			    		getfile=fopen(putfilename,"ab+");

					
					////////////////////////////
					 int loop=filesize/100;
		                   int remainfile=filesize%100;
		                   
					    int i=0;
					    char chunk[100];
					    for(int j=0;j<loop;j++)
					    {
				     	             recv(newsockfd,chunk,100,0);
						       int k=0;
						       while (i < filesize && k<100)
						       {
								buffer[i]=chunk[k];
								i++;
								k++;
						       }
					    }
					    
					    if(remainfile>0)
					    {
						   	recv(newsockfd,chunk,remainfile,0);
							int k=0;
							while (i < filesize && k<remainfile)
							{
								buffer[i]=chunk[k];
				     				i++;
				     				k++;
				     			}
					    }
							/////////////////////////////
							int s=0;
							while (s < filesize)
							{
							     printf("%02X ",(buffer[s]));
							     s++;
							     if( ! (s % 16) ) printf( "\n" );
							 }
							fwrite(buffer,1,filesize,getfile);
							//close(filedesc);
							//return 0;
                                          memset( putfilename, '\0', sizeof( putfilename ));
						
			      }			

				           // memset( replymsg, '\0', sizeof( replymsg ));
			                  //memset( requestmsg, '\0', sizeof( requestmsg ));
			  
			  printf("file has been saved");
			 memset( serverresponse, '\0', sizeof( serverresponse ));
                   memset( clientrequest, '\0', sizeof( clientrequest ));
		      
			  return false;
	       }
        
	       else  if(strcmp(clientrequest,"quit") == 0 || strcmp(clientrequest," ") == 0) 
	       {
                              return true;
	       }  

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
              bool stop=false;
              stop=operations(newsockfd);
              if(stop==true)
              {
                break;
              }
              printf("transfer closed in loop\n");
                             
     }
  
  
     printf("transfer closed\n");
     close(newsockfd);
     close(sockfd);
return 0;
}

//previous put for server

 /* recv(newsockfd,putfilename,BUFSIZ,0);
	          printf("our file name is %s\n",putfilename);
	          int putfilesize,putfiledesc;
	          char *d;
	          recv(newsockfd,&putfilesize,sizeof(int),0);
	          printf("%d\n",putfilesize);
	          d = malloc(putfilesize);
	          putfiledesc=open(putfilename, O_CREAT | O_EXCL | O_WRONLY ,0666);
		  recv(newsockfd,d,putfilesize,0);
		  write(putfiledesc,d,putfilesize);
		  close(putfiledesc);*/
		  
//initial codes for sending files
 /*struct stat ob;
   int filedesc,filesize,size;
   
   stat(filename,&ob);
   filedesc=open(filename,O_RDONLY);
   filesize=ob.st_size;*/
  // send(sockfd,filedesc,NULL,filesize);
