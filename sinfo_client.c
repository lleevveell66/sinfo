/*** sinfo_client.c                                  ***/
/*** simple socket stream info sharing - client side ***/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Port client will connect to:
#define PORT 1411
// Max bytes to get at one time:
#define MAXDATASIZE 1024
// Set this to 1 to print debugging information:
#define DEBUG 0

int main(int argc,char *argv[])
{
 int sockfd,numbytes;
 char buf[MAXDATASIZE];
 struct hostent *he;
 struct sockaddr_in their_addr;

 // Not enough command line args, show syntax and exit
 if(argc!=2)
 {
  fprintf(stderr,"SYNTAX: %s server_name_or_ip\n",argv[0]);
  exit(1);
 }

 // Get the host details:
 if((he=gethostbyname(argv[1]))==NULL)
 {
  perror("sinfo client: gethostbyname()");
  exit(1);
 }
 else
  if(DEBUG) { printf("sinfo client: The remote host is: %s\n",argv[1]); }

 // Set up our network socket:
 if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
 {
  perror("sinfo client: socket()");
  exit(1);
 }
 else
  if(DEBUG) { printf("sinfo client: The socket() sockfd is OK...\n"); }

 if(DEBUG) { printf("sinfo client: Using %s and port %d...\n",argv[1],PORT); }

 // Prepare their socket info:
 their_addr.sin_family=AF_INET;
 their_addr.sin_port=htons(PORT);
 their_addr.sin_addr=*((struct in_addr *)he->h_addr);
 // Zero out the rest of the struct:
 memset(&(their_addr.sin_zero),'\0',8);

 // Connect our end of the socket to the server end:
 if(connect(sockfd,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))==-1)
 {
  perror("sinfo client: connect()");
  exit(1);
 }
 else
  if(DEBUG) { printf("sinfo client: The connect() is OK...\n"); }

 // Read from the connection until there is no more to read, and display it:
 while((numbytes=recv(sockfd,buf,MAXDATASIZE-1,0))!=0)
 {
  buf[numbytes]='\0';
  if(DEBUG) { printf("sinfo client: Received: ***%s***\n", buf); }
  printf("%s", buf);
 }

 // Close our end of the socketed connection:
 if(DEBUG) { printf("sinfo client: Closing sockfd\n"); }
 close(sockfd);
 return 0;
}
