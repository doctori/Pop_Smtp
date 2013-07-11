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


int writen(int sockfd, char *ptr, int n);
int readn(int sockfd, char *ptr, int n);
int isOk(char *buf, int do_exit);

int socket_pop = -1;
// nom du serveur POP
char *server_pop = "pop.laposte.net";
int port = 110;

char *user= "esgi.prog";
char *password= "Password1";

int main (int argc, char *argv[]){

// Addresse de la socket
struct sockaddr_in socketServerAddr;

// Host serveur
struct hostent *serverEntity;

// Addresse du serveur
unsigned long serverAddr;

//Buffer pour la lecture/écriture/reception de la cmd bufferList
unsigned char bufferEcriture[PACKET_SIZE+1];
unsigned char bufferLecture[PACKET_SIZE+1];
unsigned char bufferList[PACKET_SIZE+1];

//Initialisation des variables
int msg, n, index = 0;
//
int retry = 4;

//remplissage de zero
bzero(&socketServerAddr,sizeof(socketServerAddr));
serverAddr = inet_addr(server_pop);
if ( (long) serverAddr != (long) -1){
	//copie les octets de serverAddr vers le socket de la taille de serverAddr
	bcopy(&serverAddr,&socketServerAddr.sin_addr,sizeof(serverAddr));
}else{
	serverEntity = gethostbyname(server_pop);
	bcopy(serverEntity->h_addr,&socketServerAddr.sin_addr,serverEntity->h_length);
}
//traduction de l'endian
socketServerAddr.sin_port = htons(port);
//IPV4
socketServerAddr.sin_family = AF_INET;
//socket ouvert en TCP avec IPV4
socket_pop = socket(AF_INET,SOCK_STREAM,0);
connect(socket_pop,(struct sockaddr *)&socketServerAddr,sizeof(socketServerAddr));
bzero(bufferEcriture, PACKET_SIZE+1);
bzero(bufferLecture, PACKET_SIZE+1);
do {
	n=readn(socket_pop,bufferLecture,PACKET_SIZE);
	retry--;
} while(isOk(bufferLecture,retry==0));

//Envoi de l'utilisateur
sprintf(bufferEcriture,"USER %s\r\n",user);
writen(socket_pop,bufferEcriture,strlen(bufferEcriture));
n=readn(socket_pop,bufferLecture,PACKET_SIZE);
//test si le serveur accepte la commande
isOk(bufferLecture,1);

//Envoi du mot de passe
sprintf(bufferEcriture,"PASS %s\r\n",password);
writen(socket_pop,bufferEcriture,strlen(bufferEcriture));
n=readn(socket_pop,bufferLecture,PACKET_SIZE);
isOk(bufferLecture,1);

//Listing des messages sur le serveur
sprintf(bufferEcriture,"LIST\r\n");
writen(socket_pop,bufferEcriture,strlen(bufferEcriture));
n=readn(socket_pop,bufferLecture,PACKET_SIZE);
isOk(bufferLecture,1);

if (strlen(bufferLecture) == 0 ){
	n=readn(socket_pop,bufferList,PACKET_SIZE);
}else{
	bcopy(bufferLecture, bufferList, strlen(bufferLecture) + 1);
}
//On incremente jusqu'a ce qu'on lise un "."
while (bufferList[index]!='.' ){
	sscanf(&bufferList[index], "%d", &msg);
	//On incrémente l'index pour lire le message
	while(bufferList[index++]!='\n');
	//On prend le contenu du message indexé par la valeur de msg
	sprintf(bufferEcriture,"RETR %d\r\n",msg);
	writen(socket_pop,bufferEcriture,strlen(bufferEcriture));
	do {
		n=readn(socket_pop,bufferLecture, PACKET_SIZE);
		printf("%s",bufferLecture);
		//comparaison de \r\n.\r\n avec le contenu de bufferLecture, si différent on 
		if ( ! strncmp("\r\n.\r\n",&bufferLecture[n-5],5))break;
		bzero(bufferLecture, PACKET_SIZE+1);
	} while ( 1 );
}

shutdown(socket_pop,2);
close(socket_pop);
return 0;
}

int writen(int sockfd, char *ptr, int taille){
int reste, ecrit;
reste = taille;
while ( reste > 0 ){
	ecrit = write(sockfd, ptr, reste);
	if ( ecrit <= 0 ){
		return ecrit;
	}
	reste -= ecrit;
	ptr += ecrit;
}
return (taille-reste);
}

int readn(int sockfd, char *ptr, int taille){
int reste, lu; 
reste = taille;
while ( reste > 0 ){
	lu = read(sockfd,ptr,reste);
	if (lu < 0 ){
		return lu;
	}else{
		if ( lu == 0 ){
			break;
		}
	}
	reste -= lu;
	ptr += lu;
	if ( *(ptr-2) == '\r'&& *(ptr-1)=='\n' )break;
}
*ptr = 0x00;
return (taille-reste);
}

int isOk(char *buf, int do_exit)
{
char *ptr, tmp[PACKET_SIZE+1];
bzero(tmp, PACKET_SIZE+1);
if ((ptr=strstr(buf, "+OK")) == NULL){
	if ( strstr(buf, "-ERR") ){
		printf("ERROR: -->%s<--\n", buf);
		exit(1);
	}
if (do_exit)exit(1);
else return 1;
} else {
	while(*ptr != '\n')ptr++ ;
	bcopy(ptr+1,tmp,strlen(ptr));
	bzero(buf,PACKET_SIZE+1);
	bcopy(tmp, buf, strlen(tmp));
}
return 0;
}
