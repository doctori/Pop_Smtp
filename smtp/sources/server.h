/*
 * server.h
 *
 *  Created on: 4 juil. 2013
 *      Author: root
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "smtpReplies.h"
#define PACKET_SIZE 1024
#define BUFFER_SIZE 2048
#define MAX_ADDR_SIZE 256
#define SOCKET_ERROR -1
int writen(int sockfd, char *ptr, int taille);
int readn(int sockfd, char *ptr, int taille);
char *gen_from(char *from);
char *gen_to(char *to);
char *gen_body(char *fichier,char *from,char *to,char *subject);
int connectToSmtpRelay();
SmtpStatus* reception(int socket);
int envoi (SmtpStatus* StatusToSend);
int callback(int sockfd,char *char_received);

#endif /* SERVER_H_ */
