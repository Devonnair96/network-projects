/*				client.c
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>		/* for gettimeofday() */
#include <stdlib.h>
#include <string.h>

/* This is the sender program. It creates a socket and initiates a
 * connection with the socket given in the command line. 
 * 1 packet is sent and then the program exits.
 * The form of the command line is `usend4 hostname portnumber'. 
 * The hostname might be algebra.sci.csueastbay.edu.  The port number
 * must be between 1000 and 64000 and must be the same as the
 * port number the receiver is running on.
 */

main(int argc,char *argv[])
{
  int sock;
  struct sockaddr_in source, extrasa, dest;
  struct hostent *hp, *gethostbyname();
  char buf[1024], message[1024], ack[2] = "1";
  int seqno = 1, rval;
  struct timeval rcvtime;
  struct timezone zone;

  /* Check arguments */
  if (argc != 3) {
    printf("Usage: usend4 hostname portnumber\n");
    exit(0);
  }

/* create socket */

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0) {
    perror("Opening stream socket");
    exit(1);
  }
  
  /* now bind source address. Port is wildcard, doesn't matter. */
  source.sin_family = AF_INET;
  source.sin_addr.s_addr = INADDR_ANY;
  source.sin_port = 0;
  if (bind(sock, (struct sockaddr *)&source, sizeof(source)) < 0) {
    perror("binding socket name");
    exit(1);
  }

  dest.sin_family = AF_INET;
  hp = gethostbyname(argv[1]);
  if(hp == 0) {
    printf("%s: unknown host\n",argv[1]);
    exit(2);
  }

  bcopy(hp->h_addr, &dest.sin_addr, hp->h_length);
  dest.sin_port = htons(atoi(argv[2]));


  /* create a packet */
  int n = 1;
  char str[10] = "request";
  //sends the initial request frame to the server. 
  sendto(sock,str,sizeof(str),0,(struct sockaddr *)&dest, sizeof(dest));
 
  while(1)
  { 
   //receives package from server. 
   rval = recvfrom(sock,message,1024,0,NULL,NULL);
   
   //gets time for received packet
   if(gettimeofday(&rcvtime, &zone)<0){
    perror("getting time");
    exit(1);
   }
   //if the frame is a loss it does not print the package info, emulating frame loss.  
   int rnum = random();
   if(rnum%5 == 0){
   printf("frame discarded... ");
   }
   else{
    printf("Client received packet from server, time = %ld %ld, seqno: %d ", rcvtime.tv_sec, rcvtime.tv_usec, seqno);
    printf("package content: ");
    puts(message);
   
    //sending the acknowledgment to the server.
    sendto(sock,ack,sizeof(ack),0,(struct sockaddr *)&dest,sizeof(dest));
   
    //get time that ack was sent
    if(gettimeofday(&rcvtime, &zone)<0){
     perror("getting time");
     exit(1);
    }
    printf("client sending ack: seqno: %d, time = %ld %ld ", seqno+1,  rcvtime.tv_sec, rcvtime.tv_usec);
   }
   seqno++;
  } 
 close(sock);
}
