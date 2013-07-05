#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#include "server.h"
char server_name[] = "smtp.laposte.net"; // nom du serveur SMTP pour faire le relay
int socket_smtp = -1;


int main (int argc, char *argv[]){

	//traitement parametres entree
	int listenPort = 25;
	int listenfd,connfd,pid;
	socklen_t len;
	struct sockaddr_in servaddr,cliaddr;
	char adresseIP[16];
	struct sigaction;
//On ignore (SIG_IGN) le signal que chaque fils qui se termine envoie Ã  son pÃ¨re (SIGCHLD)
//Ainsi, les fils ne passent pas par l'Etat zombie
signal(SIGCHLD,SIG_IGN);
listenfd=socket(AF_INET,SOCK_STREAM,0);//Creation du socket
if(listenfd < 0){
	perror("Error While Opening The Sotcket");
	exit(1);
}
bzero(&servaddr,sizeof(servaddr));//mise a zero de la structure

servaddr.sin_family=AF_INET;//IPv4
servaddr.sin_addr.s_addr=htonl(INADDR_ANY);//connexion acceptÃ©e pour toute adresse(adresse de l'hote convertit   en adresse reseau)
servaddr.sin_port=htons(listenPort);//port sur lequel ecoute le serveur

if(bind(listenfd,(struct sockaddr *)& servaddr,sizeof(servaddr))<0){//on relie le descripteur à la structure de socket
	perror("ERROR Binding of Isaac (or the socket maybe ?");
	exit(7);
}
listen(listenfd,10);//convertit la socket en socket passive,attendant les connexions.   10=nombre maximal de clients mis en attente de connexion par le noyau
len=sizeof(cliaddr);
for(;;)
{

	connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&len);
	if(connfd < 0){
		perror("ERROR Accepting the Connection (no more connection available ? ");
		exit(4);
	}
	printf("connexion : port %d\n",listenPort);
	pid=fork();
	if(pid<0){
		perror("Cannot create child process to treat the new connexion");
		exit(6);
	}
	if(pid==0)
	{
		/* Inside client process */
		close(listenfd);
		reception(connfd);//fonction chargée de travailler avec le client
		close(connfd);
		}

	close(connfd);
}
return(0);
}
