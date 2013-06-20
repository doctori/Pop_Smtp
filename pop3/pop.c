#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define PACKET_SIZE 1024
#define DEBUG 1


int writen(int fd, char *ptr, int n);
int readn(int fd, char *ptr, int n);

int socket_pop = -1;
char name[] = "smtp.laposte.net"; // nom du serveur SMTP pour faire le relay
int port = 110;

char *user= "esgi.prog";
char *pass= "Password1";

int main (int argc, char *argv[]){


// Addresse de la socket
struct sockaddr_in socketServerAddr;
	
// Description du host serveur
struct hostent *serverEntity;

// Addresse du serveur
unsigned long serverAddr;


}
