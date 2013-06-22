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
#define PACKET_SIZE 1024 
#define MAX_ADDR_SIZE 256
#define SOCKET_ERROR -1
int writen(int sockfd, char *ptr, int taille);
int readn(int sockfd, char *ptr, int taille);
char *gen_from(char *from);
char *gen_to(char *to);
char *gen_body(char *fichier,char *from,char *to,char *subject);

int socket_smtp = -1;
char server_name[] = "smtp.laposte.net"; // nom du serveur SMTP pour faire le relay
int port = 587;
char helo[] = "EHLO esgi.prog\n";
char auth[] = "AUTH PLAIN\n";
char base64[] = "AGVzZ2kucHJvZwBQYXNzd29yZDE=\n";
char data[] = "DATA\n";
char * header ;
char * text = "message.txt";
char quit[] = "QUIT\n";
int return_code = -1;
char * gen_from(char * from){
	char * new_from; 
	sprintf(new_from,"MAIL FROM: <%s>\n",from);
	return new_from;
}
char * gen_to(char * to){
	char * new_to;
	sprintf(new_to,"RCPT TO: <%s>\n",to);
	return new_to;
}
char * gen_body(char * fichier,char *from,char *to,char *subject){
	int i,lu,j,current_length = 0;
	int ft;
	int nbr_pts = 0;
	int BUF_SIZE = 1024;
	int message_length = 0;
	size_t n = BUF_SIZE ;
	char *header = malloc(sizeof(char)*512);
	char *buf = malloc(sizeof(char)*n);
	char *message = malloc((sizeof(char)*n)*20);
	//construction d'une part des headers
	sprintf(header,"Delivered-To: pedro  <%s> \r\nFrom: nico  <%s> \r\nReply-To: nicO <%s> \r\nTo: Pedro <%s> \r\nSubject: %s \r\n\r\n",to,from,from,to,subject);
	for(i=0;i<strlen(header);i++){
		message[i]=header[i];
		current_length++;
	}
	free(header);
	ft=open(fichier,O_RDONLY);
	if(ft<0){
		perror("Erreur pour l'ouverture du fichier de message\n");
		exit(2);
	}
	//on charge le fichier en mem
	do{
		lu=read(ft,buf,BUF_SIZE);
		if(lu<0){
				perror("Erreur Lecteur fichier message\n");
				exit(2);
			}
		for(j=0;j<lu;j++){
			message[current_length+j]=buf[j];
		}
		current_length=current_length+lu;
	}while(lu>0);
	/* 4.5.2.  TRANSPARENCY

	           Without some provision for data transparency the character
	           sequence "<CRLF>.<CRLF>" ends the mail text and cannot be sent
	           by the user.  In general, users are not aware of such
	           "forbidden" sequences.  To allow all user composed text to be
	           transmitted transparently the following procedures are used.
               1. Before sending a line of mail text the sender-SMTP checks
	           the first character of the line.  If it is a period, one
                   additional period is inserted at the beginning of the line.
               2. When a line of mail text is received by the receiver-SMTP
                  it checks the line.  If the line is composed of a single
	          period it is the end of mail.  If the first character is a
	          period and there are other characters on the line, the first
		 character is deleted.
		 */
	// On ajoute 128 a la longueur du message initiale pour la gestion des points
	message_length = strlen(message) + 128;
	char * new_body = malloc(message_length * sizeof(char));
	char * body_ended = malloc((message_length) * sizeof(char));
	// Parcours et compte le nimbre de points
	for(i=0;i<strlen(message)+nbr_pts;i++){
		new_body[i+nbr_pts] = message[i];
		if((i>2) && (message[i] == '.')){
			if(((message[i-1] == '\n') || (message[i-1] == '\r')) && ((message[i+1] == '\r') ||(message[i+1] =='\n'))){
				nbr_pts++;
				new_body[i+nbr_pts]='.';
			}	
		}
		printf("%c",new_body[i+nbr_pts]);
	}
	body_ended=strcat(new_body,"\r\n.\r\n");
	return body_ended;
}
int envoi (char *from,char *to,char *subject){

	char buf[PACKET_SIZE+1], *ptr,tmp;
	FILE *bulk;
	int nb,rc,length = sizeof(int);
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
	 * Le message finissant par "." (-> 250 ok 1371479777 qp 11841
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
	//vidage du buffer :
	strncpy(buf,"",sizeof(buf));
	//Champ DATA
	writen(socket_smtp,data,strlen(data));
	nb = 0;
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	strncpy(buf,"",sizeof(buf));
	//printf("AFTER %s \n",gen_body(text));
	//ICI ca bloque
	rc = writen(socket_smtp,gen_body(text,from,to,subject),strlen(gen_body(text,from,to,subject)));
	read(socket_smtp,buf,PACKET_SIZE);
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
	char tmp;  
	int reste, ecrit, length = sizeof(int);
	reste = taille;
	// lire tant que n ocets restant (reste) non nul
	while ( reste > 0 )
	{
		ecrit = write(sockfd, ptr, reste);
		printf("writting : %s",ptr);
		if ( ecrit < 0 ){
			perror("write-error");
	                 ecrit = getsockopt(sockfd,SOL_SOCKET, SO_ERROR,&tmp,&length);
	                if(ecrit == 0){
	                        // Erreur asynchrone
					errno = tmp;
					perror("SO_ERROR was");
				}
				close(sockfd);
				exit(-1);
			}
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
			printf("Server is closing the connection\n");
			close(sockfd);
			exit(-1);
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
	char *subject = malloc(sizeof(char)*256);
	//traitement parametres entree
	char *to = malloc(sizeof(char)*256);
	char * from = "esgi.prog@laposte.net";

	if(argc >1){
		to = argv[1];
	}else{
		printf("TO ? \n\r");
		fgets(to,MAX_ADDR_SIZE,stdin);
	}
	printf("Subject ?\n\r");
	fgets(subject,MAX_ADDR_SIZE,stdin);
	//suppression des retour a la ligne
	 if ((strlen(to)>0) && (to[strlen (to) - 1] == '\n'))
		         to[strlen (to) - 1] = '\0';
	  if ((strlen(subject)>0) && (to[strlen (subject) - 1] == '\n'))
		         to[strlen (subject) - 1] = '\0';

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
	if(connect(socket_smtp,(struct sockaddr *)&socketServerAddr, sizeof(socketServerAddr)) == SOCKET_ERROR){
	perror("Connect ERROR");
	exit(2);
}
	envoi(from,to,subject);
	// fermeture de la connection
	shutdown(socket_smtp,2);
	close(socket_smtp);
																	return 0;
}

