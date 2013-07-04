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
#include "smtpReplies.h"



int port = 587;
int smtpStatus = 0;
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
int envoi (int socket_smtp,char *from,char *to,char *subject){

	char buf[PACKET_SIZE+1];
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
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	strncpy(buf,"",sizeof(buf));
	//printf("AFTER %s \n",gen_body(text));
	//ICI ca bloque
	writen(socket_smtp,gen_body(text,from,to,subject),strlen(gen_body(text,from,to,subject)));
	read(socket_smtp,buf,PACKET_SIZE);
	printf(buf);
	
	//QUIT
	writen(socket_smtp,quit,strlen(quit));
	readn(socket_smtp,buf,PACKET_SIZE);
	printf(buf);

	return_code = 0;
	return(return_code);
}


/*
 * callback :
 * 	Permet le retour pour le client (char par char)
 */

int callback(int sockfd,char *char_received)
{
	int ecrit;
	ecrit = write(sockfd,char_received,sizeof(char));
	if( ecrit < 0){
		perror("CallBack Char Error (client disconnected while writting ?)");
		exit(5);
	}
	return(0);
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
 *   retourne le nombre d'octets lu ou -1 en cas d'erreur
 */
int readn(int sockfd, char *ptr, int taille)
{
	int reste, lu;
	reste = taille;
	while ( reste > 0 )
	{
		lu = read(sockfd,ptr,reste);
		printf("here ?");
		callback(sockfd,ptr);
		if (lu < 0 ){
			printf("erreur");
			return(lu); /*erreur*/
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
int reception(int socket){
	printf("in reception : \n");
	SmtpStatus Status;
 	char *buffer;
 //initialisation de la connexion
 bzero(buffer,BUFFER_SIZE);
 Status = DefineReply(0,buffer);
 printf("REPLY : %s d'un longueur : %d \n",Status.awnser,(int)strlen(Status.awnser));

 writen(socket,Status.awnser,strlen(Status.awnser));
 readn(socket,buffer,BUFFER_SIZE);
	printf("BUFFER recu : %s",buffer);
	Status = DefineReply(Status.statusCode,buffer);
	 switch(Status.statusCode){
	 case 221:
		 exit(0);
	 case 554:
		 perror("The Server is not available for awnser Exiting Connection");
		 exit(122);
		 break;
	 default:
		printf("OK \n");
		printf(buffer);
		 break;



 }
 //Connection OK 220 envoyé
// En attente d'un ehlo/helo
 return(0);

}


