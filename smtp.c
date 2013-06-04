#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#define HELO "HELO 192.168.1.1\r\n"
#define DATA "DATA\r\n"
#define QUIT "QUIT\r\n"

int sock;
struct sockaddr_in server;
struct hostent *hp, *gethostbyname();
char buf[BUFSIZ+1];
int len;
char *host_id="XX.XX.XX.XX";
char *from_id="langlais.christophe.co@gmail.com";
char *to_id="langlais.christophe.co@gmail.com";
char *sub="testmail\r\n";
char wkstr[100]="hello\r\n";

/*=====Send a string to the socket=====*/

void send_socket(char *s)
{
	write(sock,s,strlen(s));
    write(1,s,strlen(s));
}

//=====Read a string from the socket=====*/

void read_socket()
{
	len = read(sock,buf,BUFSIZ);
    write(1,buf,len);
}

/*=====MAIN=====*/
int main(int argc, char* argv[])
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock==-1)
    {
		perror("opening stream socket");
		exit(1);
    }
    //TODO LE RESTE !!!
}
