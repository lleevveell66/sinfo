/*** sinfo_server.c                                  ***/
/*** simple socket stream info sharing - server side ***/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// Port server will listen on:
#define PORT 1411
// Number of pending connections the queue will hold:
#define BACKLOG 10
// Set this to 1 to print debugging information:
#define DEBUG 0

void sigchld_handler(int s)
{
 while(wait(NULL)>0);
}

int main(int argc,char *argv[ ])
{
 // We will listen on sock_fd, with new connections on new_fd:
 int sockfd,new_fd,numbytes;
 // Server address information:
 struct sockaddr_in my_addr;
 // Client address information:
 struct sockaddr_in their_addr;
 int sin_size;
 struct sigaction sa;
 int yes=1;

 // Set up our network socket:
 if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
 {
  perror("sinfo server: socket() error");
  exit(1);
 }
 else
  if(DEBUG) { printf("sinfo server: socket() sockfd is OK...\n"); }

 // Set some options on our end:
 if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
 {
  perror("sinfo server: setsockopt() error");
  exit(1);
 }
 else
  if(DEBUG) { printf("sinfo server: setsockopt is OK...\n"); }

 // Prepare our socket info:
 my_addr.sin_family=AF_INET;
 my_addr.sin_port=htons(PORT);
 my_addr.sin_addr.s_addr=INADDR_ANY;
 // Zero out the rest of the struct:
 memset(&(my_addr.sin_zero),'\0',8);

 if(DEBUG) { printf("sinfo server: Using %s and port %d...\n",inet_ntoa(my_addr.sin_addr),PORT); }

 // Bind our end of the socket:
 if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==-1)
 {
  perror("sinfo server: bind() error");
  exit(1);
 }
 else
  if(DEBUG) { printf("sinfo server: bind() is OK...\n"); }
  
 // Listen on our socket:
 if(listen(sockfd,BACKLOG)==-1)
 {
  perror("sinfo server:listen() error");
  exit(1);
 }
 if(DEBUG) { printf("sinfo server: listen() is OK...Listening...\n"); }

 // Clean any dead processes:
 sa.sa_handler=sigchld_handler;
 sigemptyset(&sa.sa_mask);
 sa.sa_flags=SA_RESTART;

 // Set up a child signaling action:
 if(sigaction(SIGCHLD,&sa,NULL)==-1)
 {
  perror("sinfo server: sigaction() error");
  exit(1);
 }
 else
  if(DEBUG) { printf("sinfo server: sigaction() is OK...\n"); }

 // accept() loop
 while(1)
 {
  sin_size=sizeof(struct sockaddr_in);

  if((new_fd=accept(sockfd,(struct sockaddr *)&their_addr,&sin_size))==-1)
  {
   perror("sinfo server: accept() error");
   continue;
  }
  else
   if(DEBUG) { printf("sinfo server: accept() is OK...\n"); }

  if(DEBUG) { printf("sinfo server: new socket, new_fd is OK...\n"); }
  if(DEBUG) { printf("sinfo server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr)); }

  // Child process:
  if(!fork())
  {
   // Child does not require the listener, so close the socket:
   if(DEBUG) { printf("sinfo server, forked child: closing sockfd\n"); }
   close(sockfd);

   if(DEBUG) { printf("sinfo server, forked child: reading stats.txt\n"); }

   char buffer[1024];
   char * line = NULL;
   FILE *f;
   int len; 
   size_t l=0;
   ssize_t read;

   // Read from the local file stats.txt and send it on the connection:
   f=fopen("stats.txt","r");
   while((read=getline(&line,&l,f))!=-1)
   {
    if(DEBUG) { printf("sinfo server, forked child: Trying to send %s\n",line); }

    len=strlen(line);
    if(sendall(new_fd,line,&len)==-1) 
    {
     perror("sinfo server, forked child: sendall()");
     printf("We only sent %d bytes because of the error!\n",len);
    }
   }
   close(new_fd);
   fclose(f);
   if(DEBUG) { printf("sinfo server, forked child: exiting forked child\n"); }
   exit(0);
  }
  else
   if(DEBUG) { printf("sinfo server: send is OK...!\n"); }

  // The parent does not need the new socket:
  close(new_fd);
  if(DEBUG) { printf("sinfo server: new socket, new_fd closed successfully...\n"); }
 }
 return 0;
}


int sendall(int s,char *buf,int *len)
{
 int total=0;            // Total bytes we've sent
 int bytesleft=*len;     // Number of bytes left to send
 int n;

 while(total<*len) 
 {
  n=send(s,buf+total,bytesleft,0);
  if(n==-1) { break; }
  total+=n;
  bytesleft-=n;
 }

 *len=total;             // Return the number actually sent

 return n==-1?-1:0;      // Return -1 on failure, 0 on success
}

