/*				Usend4.c
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>		/* for gettimeofday() */
#include <stdlib.h>
#include <strings.h>

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
  char buf[1024];
  int seqno, rval;

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
  seqno = 1;
  sprintf(buf, "%d\0", seqno);

  if ((rval=sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&dest,sizeof(dest))) < 0) {
    perror("writing on datagram socket");
  }
  system("date");
  printf("sent packet with seqno %d\n", seqno);

  close(sock);
}
