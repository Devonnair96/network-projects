/*				Serv2.c
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>           /* for gettimeofday() */
#include <stdlib.h>
#include <strings.h>

#define TRUE 1

/* This is the receiver program. It opens a socket with the given port
 * and then begins an infinite loop. Each time through the loop it 
 * accepts a pkt and prints out the contents and the current time. 
 * Command line is `urecv4 portnumber'.  
 * The port number is a number between 1000 and 64000.
 */

main(int argc,char *argv[])
{
  int sock, length;
  struct sockaddr_in source, dest;
  int msgsock;
  char buf[1024], message[1024];
  int rval, srclen;
  int i, seqno = 0;
  long time;
  struct timeval rcvtime;
  struct timezone zone;
 

  /* check arguments */
  if (argc != 2) {
    printf("Usage: urecv4 localport\n");
    exit(0);
  }  

/* create socket */

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0) {
    perror("opening datagram socket");
    exit(1);
  }

  /* name socket using wildcard for IP address and given port number */

  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = INADDR_ANY;
  dest.sin_port = htons(atoi(argv[1]));
  if(bind(sock, (struct sockaddr *)&dest, sizeof(dest))) {
    perror("binding socket name");
    exit(1);
  }

  /* find out assigned port number and print out */

  length = sizeof(dest);
  if(getsockname(sock, (struct sockaddr *)&dest, &length)) {
    perror("getting socket name");
    exit(1);
  }
  printf("Socket has port #%d\n",ntohs(dest.sin_port));
 
  
  do {
      bzero(buf,sizeof(buf));
      srclen = sizeof(source);
      //initially receives the request from and then receives the acks from packages sent to client. 
      while ((rval = recvfrom(sock,buf,1024,0,(struct sockaddr *)&source,&srclen))<0){
        perror("receiver recvfrom");
      }
      /* get current relative time */
      if (gettimeofday(&rcvtime, &zone) < 0) {
    	perror("getting time");
    	exit(1);
      }
      //if the frame being received is the request frame it wont print this. 
      if(seqno != 0){
       printf("Server Received ACK, time = %ld %ld, seqno: %d ", rcvtime.tv_sec, rcvtime.tv_usec, seqno);
      }
      //sends the package to the client.   
      sendto(sock,message,rval,0,(struct sockaddr *)&source,srclen);
      seqno++;
      sleep(1);
      
  } while (rval != 0);
   close(sock);
}

