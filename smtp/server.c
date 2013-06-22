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

#define PACKET_SIZE 1024 

void contenu(char *f);
int writen(int sockfd, char *ptr, int taille);
int readn(int sockfd, char *ptr, int taille);

int socket_smtp = -1;
char server_name[] = "smtp.laposte.net"; // nom du serveur SMTP pour faire le relay
int port = 587;
char helo[] = "HELO esgi.prog\n";
char auth[] = "AUTH PLAIN\n";
char base64[] = "AGVzZ2kucHJvZwBQYXNzd29yZDE=\n";
char * from = "esgi.prog@laposte.net";
char * to= "langlais.christophe.co@gmail.com";
char data[] = "DATA\n";
char * header ;
char text[] = "To: langlais.christophe.co@gmail.com\nFrom: esgi.prog@laposte.net\nSubject: this is a test message\nDate: Thu, 17 Jun 2013 12:12:12 -0200\nCeci est un message test\n.\n";
char quit[] = "QUIT\n";
int return_code = -1;
char * gen_from(char * from){
	char * new_from; 
	sprintf(new_from,"MAIL FROM: <%s>\n",from);
	return new_from;
}
char * gen_to(char * to){
	char * new_to;
	sprintf(new_to,"RCPT TO: < %s>\n",to);
	return new_to;
}
char * gen_body(char * message){
	// fera d'autres choses
	return message;
}
int envoi (){

	char buf[PACKET_SIZE+1], *ptr;
	FILE *bulk;
	int nb;

	bulk = stdin;
	buf[0]= 0x00;
	
	/* RFC 
	 * Connexion reponse serveur : 220 ....
	 * HELO réponse serveur : 250 You are ...
	 * AUTH PLAIN réponse serveur : 334 
	 * envoi id/mdp en base64 réponse serveur : 235 ok, go ahead (#2.0.0)
	 * MAIL FROM: -> 250 ok
	 * RCPT TO: -> 250 ok
	 * DATA -> 354 go ahead
	 * Le message finissant par "." -> 250 ok 1371479777 qp 11841
	 * QUIT -> 221
	 */
	
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	
	//HELO
	writen(socket_smtp,helo,strlen(helo));
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	
	//AUTH PLAIN
	writen(socket_smtp,auth,strlen(auth));
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	
	//login/mdp base64
	writen(socket_smtp,base64,strlen(base64));
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	
	//Champ FROM
	writen(socket_smtp,gen_from(from),strlen(gen_from(from)));
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	
	//Champ TO
	writen(socket_smtp,gen_to(to),strlen(gen_to(to)));
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	
	//Champ DATA
	writen(socket_smtp,data,strlen(data));
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);

	//ICI ça bloque
	writen(socket_smtp,text,strlen(text));
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	
	//QUIT
	writen(socket_smtp,quit,strlen(quit));
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);

	return_code = 0;
	return return_code;
}


/*
 * writen:
 *   write 'n' octets sur le descripteur du socket
 *   retourne le nombre de octets ecrit ou -1 en cas d'erreur
 */
 
int writen(int sockfd, char *ptr, int taille)
{
	int reste, ecrit;
	reste = taille;
	// lire tant que n ocets restant (reste) non nul
	while ( reste > 0 )
	{
		ecrit = write(sockfd, ptr, reste);
		if ( ecrit <= 0 )
			return ecrit; /*erreur*/
		reste -= ecrit;
		ptr += ecrit;
	} 
return (taille-reste);
} 

/*
 * readn:
 *   read 'n' octets jusqu'a la fin
 *   retourne le nombre de octets lu ou -1 en cas d'erreur
 */
int readn(int sockfd, char *ptr, int taille)
{
	int reste, lu;
	reste = taille;
	while ( reste > 0 )
	{
		lu = read(sockfd,ptr,reste);
		if (lu < 0 ){
			printf("erreur");
			return lu; /*erreur*/
		} else if ( lu == 0 ){
			break;
		}
		reste -= lu;
		ptr += lu;
		if ( *(ptr-2) == '\r' && *(ptr-1) == '\n' ){
			break;
		}
	}
	*ptr = 0x00;
	return (taille-reste);
}
int main (int argc, char *argv[]){
	// Addresse de la socket
 	struct sockaddr_in socketServerAddr;
	// Description du host serveur
	struct hostent *serverEntity;

	// Addresse du serveur
	unsigned long serverAddr;
	
	// Initialisation de l'adresse du socket serveur a zero
	bzero((void *)&socketServerAddr,sizeof(socketServerAddr));
	
	// Convertion de l'adresse ip
	serverAddr = inet_addr(server_name);
	if ( (long)serverAddr != (long)-1)
		bcopy((void *)&serverAddr,(void*)&socketServerAddr.sin_addr,sizeof(serverAddr));
	else{
		serverEntity = gethostbyname(server_name);
		bcopy((void *)serverEntity->h_addr,(void *)&socketServerAddr.sin_addr,serverEntity->h_length);
	}
	// Htons pour utiliser l'endian du reseau (host to network)
	socketServerAddr.sin_port = htons(port);
	// Adresse IPv4
	socketServerAddr.sin_family = AF_INET;
	// Creation de la socket en TCP SOCK_STREAM
	socket_smtp = socket(AF_INET,SOCK_STREAM,0);
	// Connexion du socket
	connect(socket_smtp,(struct sockaddr *)&socketServerAddr, sizeof(socketServerAddr));
	envoi();
	// fermeture de la connection
	shutdown(socket_smtp,2);
	close(socket_smtp);
																	return 0;
}

