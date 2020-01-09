			/*email2.c*/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Program to send and receive email through socket programming. 

void sender(char uemail[50],char netid[6],char messag[1024]);
void receiver(char netid[6],char password[20]);
int main()
{
char choice[2], uem[50], neti[6], mess[1024], app[256];
char lineno[5], passwrd[20];
int i=0, line;

while(1){
printf("Welcome to the interface. press 1 to send or 2 to receive: ");
scanf("%s",choice);
if(strcmp(choice,"1")==0)
{
//asks user for email destination and email contents
printf("Enter your email address:");    
scanf("%s", uem);                         
printf("Enter the netID:");
scanf("%s", neti);

//extra fgets to catch the newline scanf leaves behind. 
fgets(mess,sizeof(mess),stdin);
printf("write your data. once you are finished write a period on a single line\n");

while(1){
printf("Line %i:", i);
fgets(app,sizeof(app),stdin);
strcat(mess,app);
if(strcmp(app,".\n")==0)
{
break;
}
i++;
}

//calls sender program. 
sender(uem,neti,mess);
 }
else if(strcmp(choice,"2")==0)  //Receive function
  {
//get username & password
printf("Please enter your netID: ");
scanf("%s",neti);
printf("Now enter your password: ");
scanf("%s",passwrd);

//call receiver
receiver(neti,passwrd);
  }
memset(choice,0,strlen(choice));
printf("Would you like to do another thing: y or n: ");
scanf("%s",choice);
if(strcmp(choice,"n")==0)
{break;}
 }
}

void sender(char uemail[50],char netid[6],char  messag[1024]){
int sock;
struct sockaddr_in source, dest;
struct hostent *hp, *gethostbyname();

//creates a socket
sock = socket(AF_INET, SOCK_STREAM, 0);
if(sock < 0){
perror("Opening stream socket");
exit(1);
}

//get host address
dest.sin_family = AF_INET;
dest.sin_port = htons(25);

hp = gethostbyname("hopper.csueastbay.edu");
if(hp==0)
{
printf("hostname\n");
}

bcopy(hp->h_addr,&dest.sin_addr,hp->h_length);

//declare values
char rec[1024];
char eml[100];
char neti[100];

bzero(eml,sizeof(eml));
bzero(neti,sizeof(neti));

//connects the socket to the server.
if(connect(sock,(struct sockaddr *)&dest,sizeof(dest)) < 0)
{
printf("error connection\n");
}

//repeated user input data and then formats to SMTP command
strcpy(eml,"MAIL FROM:<");
strcat(eml,uemail);
strcat(eml,">\r\n");

//repeat destination data and then format
strcpy(neti,"RCPT TO:<");
strcat(neti,netid);
strcat(neti,">\r\n");

//read connection ack
int n = read(sock,rec,sizeof(rec));
printf("%s\n",rec);
memset(rec,0,strlen(rec));

//send MAILFROM to server
int m = write(sock,eml,strlen(eml));

//receive ack
n = read(sock,rec,sizeof(rec));
printf("recv:%s\n",rec);
memset(rec,0,strlen(rec));

//send RCPTTO to server
m = write(sock,neti,strlen(neti));
//receive ack
n = read(sock,rec,sizeof(rec));
printf("recv:%s\n",rec);
memset(rec,0,strlen(rec));

//data
char dat[] = "DATA\r\n";
m = write(sock,dat,strlen(dat));
n = read(sock,rec,sizeof(rec));
printf("recv:%s\n",rec);
memset(rec,0,strlen(rec));

//email content
printf("email content:%s\n",messag);
m = write(sock,messag,strlen(messag));
n = read(sock,rec,sizeof(rec));
printf("recv:%s\n",rec);
close(sock);
}

void receiver(char netid[6],char password[20])
{
int sock;
struct sockaddr_in source, dest;
struct hostent *hp, *gethostbyname();

//create the socket
sock = socket(AF_INET, SOCK_STREAM, 0);
if(sock < 0){
perror("Opening stream socket");
exit(1);
}

dest.sin_family = AF_INET;
dest.sin_port = htons(110);

hp = gethostbyname("hopper.csueastbay.edu");
if(hp==0)
{
printf("hostname\n");
}
bcopy(hp->h_addr, &dest.sin_addr,hp->h_length);

//declare variables
char u[12];
char pas[30], msg[7] = "RETR ";
char rec[1024], list[6] = "LIST\r\n";
bzero(u,sizeof(u));
bzero(pas,sizeof(pas));

//connect the socket to the server
if(connect(sock,(struct sockaddr *)&dest,sizeof(dest))<0)
{
printf("error connection\n");
}

//format username
strcpy(u,"USER ");
strcat(u,netid);
strcat(u,"\r\n");

//format password
strcpy(pas,"PASS ");
strcat(pas,password);
strcat(pas,"\r\n");

//read connection ack
int n = read(sock,rec,sizeof(rec));
printf("%s\n",rec);
memset(rec,0,strlen(rec));

//enter username
int m = write(sock,u,strlen(u));

//receive ack
n = read(sock,rec,sizeof(rec));
printf("recv:%s\n",rec);
memset(rec,0,strlen(rec));

//enter password
m = write(sock,pas,strlen(pas));

//receive ack
n = read(sock,rec,sizeof(rec));
printf("recv:%s\n",rec);
memset(rec,0,strlen(rec));

//ask for list of messages
m = write(sock,list,strlen(list));

//get list
n = read(sock,rec,sizeof(rec));
printf("recv:%s\n",rec);
memset(rec,0,strlen(rec));

char resp[2];
//prompt user which messag they want to see. 
printf("which email do you wish to read(pick number):");
scanf("%s",resp);
strcat(msg,resp);
strcat(msg,"\r\n");

//enter command
m = write(sock,msg,strlen(msg));

//final stretch
n = read(sock,rec,sizeof(rec));
printf("%s\n",rec);
close(sock);
}
