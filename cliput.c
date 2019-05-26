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
#include<stdbool.h>
#include<pthread.h>

//#define Filename "a.jpg"

void putfiledata(int sockfd,char *filename)
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
      //  return 0; end of file send functionn
}

bool sendfileoversocket(int sockfd,char *filename)
{
   struct stat ob;
   int filedesc,filesize,size;
   
   stat(filename,&ob);
   filedesc=open(filename,O_RDONLY);
   filesize=ob.st_size;
  // send(sockfd,filedesc,NULL,filesize);

   putfiledata(sockfd,filename);//working line of function
   
   return true;
}

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
      char replymsgpwd[BUFSIZ];
         
    int filedesc;
    unsigned long filesize;

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


            memset( replymsg, '\0', sizeof( replymsg ));
            memset( requestmsg, '\0', sizeof( requestmsg ));
 	      printf("Give command \n");
	      scanf("%s",requestmsg);
	      requestmsg[strlen(requestmsg)]='\0';
	      printf("%lu\n",strlen(requestmsg));
	     
	      printf("our request msg is %s\n",requestmsg);
	      replymsg[0]='\0';
	      printf("first reply msg %s\n",replymsg);
	      
	      if(strcmp(requestmsg,"Get") == 0)   
	      {
	            char Filename[256];
		      strcpy(requestmsg,"Get ");
		      //write(sockfd,requestmsg,strlen(requestmsg));
		      printf("Give file name \n");
		      scanf("%s",Filename);
		      strcat(requestmsg,Filename);
		      Filename[strlen(Filename)]='\0';
		      printf("%s\n",requestmsg);
		      write(sockfd,requestmsg,strlen(requestmsg));
		      // strcat(requestmsg,Filename);
		      
		      printf("%lu\n",strlen(Filename));
		     
		      printf("our reply msg before receiving reply msg %s\n",replymsg);
		      recv(sockfd,replymsg,2,0);
		      replymsg[2]='\0';
		     
		      printf("OUR REPLY MSG IS %s\n",replymsg); 
		      printf("Result of string compare %d\n",strcmp(replymsg,"OK")); 
		      
		      
		      if(strcmp(replymsg,"OK") == 0)
		      {
				printf("ok");
				recv(sockfd,&filesize,sizeof(unsigned long),0);
				char *buffer=(char *)malloc(filesize);
				//filedesc=open(Filename, O_CREAT | O_EXCL | O_WRONLY ,0666);
				FILE *getfile;
				getfile=fopen(Filename,"ab+");

				
				////////////////////////////
				 int loop=filesize/100;
                         int remainfile=filesize%100;
                         
				 int i=0;
        			 char chunk[100];
        			 for(int j=0;j<loop;j++)
        			 {
           				recv(sockfd,chunk,100,0);
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
                     		recv(sockfd,chunk,remainfile,0);
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
				     if( ! (s % 16) )
				     {
				     		printf( "\n" );
				     }
				 }
				fwrite(buffer,1,filesize,getfile);
				//close(filedesc);
				//return 0;
		      }
		      else
		      {
			  printf("bad request");
			  fprintf(stderr,"Bad request\n");
		      }
		      
		      memset( Filename, '\0', sizeof( Filename ));
		      memset( replymsg, '\0', sizeof( replymsg ));
	            memset( requestmsg, '\0', sizeof( requestmsg ));
	     } 
	     
	     else if(strcmp(requestmsg,"pwd") == 0)
	     {
			write(sockfd,requestmsg,strlen(requestmsg));
		      recv(sockfd,replymsgpwd,BUFSIZ,0);
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
	                recv(sockfd, &size, sizeof(int), 0);
	                printf("for ls %lu\n",strlen(requestmsg));
	               	
	                f=malloc(size);
	                filehandle = open(lsfilename, O_CREAT | O_EXCL | O_WRONLY ,0666);
	                recv(sockfd, f, size, 0);
	              
	                write(filehandle, f, size);
	              
	                printf("The remote directory listing is as follows:\n");
                      //printf("\n");
	                system("cat t.txt");
	                remove("t.txt");
	        
	     }
	     
	     else if(strcmp(requestmsg,"cd") == 0)
	     {
			send(sockfd,requestmsg,256,0);
			printf("enter your choiceable path\n");
			char path[256];
			int status;
			scanf("%s",path);
			path[strlen(path)]='\0';
			printf("%lu\n",strlen(path));
			send(sockfd, path,256,0);
			recv(sockfd, &status, sizeof(int), 0);
			
			if(status==1) 
			{
			      printf("Changed successfully\n");
			}
			else 
			{
				printf("problem;\n");
			}
	     }
	     
	    else if(strcmp(requestmsg,"put")==0)
	    {
	         //   write(sockfd,requestmsg,strlen(requestmsg));
	         //   char putfilename[256];
	         /*   printf("Enter your file name");
	              scanf("%s",putfilename);
	              putfilename[strlen(putfilename)]='\0';
	         */
	            char putfilename[256];
		      strcpy(requestmsg,"put ");
		      //write(sockfd,requestmsg,strlen(requestmsg));
		      printf("Give file name \n");
		      scanf("%s",putfilename);
		      strcat(requestmsg,putfilename);
		      putfilename[strlen(putfilename)]='\0';
		      printf("%s\n",requestmsg);
		      write(sockfd,requestmsg,strlen(requestmsg));
	           
	         
		      sendfileoversocket(sockfd,putfilename);
		      printf("file has been sent;");
	        //}
	         
                  memset( putfilename, '\0', sizeof( putfilename ));
	    }
	    
	    else if(strcmp(requestmsg,"help")==0)
	    {
	       
			printf("Hello,\n");
			printf("You have the following options in this ftp session\n");
			printf("\n");
			printf("   1.Get  --------------- this helps you to fetch the file form the server\n");
			printf("   2.put  --------------- this command is to put any item into the server\n");
			printf("   3.pwd  --------------- this command is for present working directory of the server\n");
			printf("   4.ls   --------------- this command shows you all the files of the current directory in the server\n");
			printf("   5.cd   --------------- this command is for changing the directory of the server\n");
			printf("   6.quit --------------- in order to quit  the existing ftp session \n");
	    
	    }
	    
	    else if(strcmp(requestmsg,"!pwd")==0)
	    {
	                
	                printf("This is the current directory of the client \n");
	                
	                char pwd[256];
			    getcwd(pwd,sizeof(pwd));
			    pwd[strlen(pwd)]='\0';
			    printf("%s",pwd);
                memset( pwd, '\0', sizeof( pwd ));
                memset( requestmsg, '\0', sizeof( requestmsg ));
			    
	                
	    }
	    
	    else if(strcmp(requestmsg,"!ls")==0)
	    {
	                system("ls > t.txt");
	                system("cat t.txt");
	                remove("t.txt");
	    }
	    
	    else if(strcmp(requestmsg,"!cd")==0)
	    {
	                 printf("give full directory\n");
	                 char path[256];
	                 scanf("%s",path);
	                 if(chdir(path) == 0)
                       {
		               printf("the directory has been changed\n");
		           }
		           else  printf("sorry to change the directory\n");           
		       
                       memset( path, '\0', sizeof( path ));
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
	       
	     memset( replymsg, '\0', sizeof( replymsg )); 
           memset( requestmsg, '\0', sizeof( requestmsg ));
	       // Filename[0]='\0';
	   
	  
   }
   
close(sockfd);
printf("socket closed\n");
return 0;

}


//previously tried put
 /* int putFilehandle,putfilesize;
	           struct stat obj;
	           stat(putfilename,&obj);
	           putFilehandle=open(putfilename, O_RDONLY);
	          
	           if(putFilehandle == -1)
	           {
	              printf("No such file here");
	           }
	           else
	           {
	           	write(sockfd,putfilename,256);
		   	putfilesize=obj.st_size;
	          	  send(sockfd, &putfilesize, sizeof(int), 0);
	           	  sendfile(sockfd, putFilehandle, NULL, putfilesize);
			  //recv(sockfd, &status, sizeof(int), 0);
			  /*if(status)
			  {
			    printf("File stored successfully\n");
			  }
			  else printf("File failed to be stored to remote machine\n");*/
