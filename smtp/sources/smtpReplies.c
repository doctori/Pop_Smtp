/*
 *  SmtpReplies.c
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "smtpReplies.h"
#define MAX_TOKEN 4


SmtpReply smtpReplies[] = {
	{211, "System status\n"},
	{214, "Help message\n"},
	{220, "Service ready\n"},
	{221, "Service closing transmission channel\n"},
	{250, "Requested mail action okay, completed\n"},
	{354, "Start mail input; end with <CRLF>.<CRLF>\n"},
	{421, "Service not available, closing transmission channel\n"},
	{450, "Requested mail action not taken: mailbox unavailable\n"},
	{451, "Requested action aborted: local error in processing\n"},
	{452, "Requested action not taken: insufficient system storage\n"},
	{500, "Syntax error, command unrecognized\n"},
	{501, "Syntax error in parameters or arguments\n"},
	{503, "Bad sequence of commands\n"},
	{552, "Requested mail action aborted: exceeded storage allocation\n"},
	{553, "Requested action not taken: mailbox name not allowed\n"},
	{554, "Transaction failed\n"}
};
char *SmtpAdressToString(SmtpAddress SmtpAddress){
	char Address[256];
	printf("Concatenation de %s@%s",SmtpAddress.user,SmtpAddress.domain);
	strcat(Address,SmtpAddress.user);
	strcat(Address,"@");
	strcat(Address,SmtpAddress.domain);
	return(Address);
}
SmtpAddress SmtpAddressClone(SmtpAddress SmtpAddressSource){
	SmtpAddress SmtpAddressDest;
	SmtpAddressDest.user = SmtpAddressSource.user;
	SmtpAddressDest.domain=SmtpAddressSource.domain;
	return(SmtpAddressDest);
}
SmtpStatus SmtpStatusClone(SmtpStatus SmtpStatusSource){
	SmtpStatus SmtpStatusDest;
	SmtpStatusDest.DATA=SmtpStatusSource.DATA;
	SmtpStatusDest.awnser = SmtpStatusSource.awnser;
	SmtpStatusDest.statusCode = SmtpStatusSource.statusCode;
	SmtpStatusDest.FROM=SmtpAddressClone(SmtpStatusSource.FROM);
	SmtpStatusDest.TO=SmtpAddressClone(SmtpStatusSource.TO);
	return(SmtpStatusDest);

}
char* GetSmtpReplyTextByCode(int replyCode)
{
	int i;
	for(i = 0; i < SMTP_REPLIES_COUNT; i++)
	{
		if (smtpReplies[i].replyCode == replyCode)
			return(smtpReplies[i].replyText);
	}
	
	return NULL;
}
char* ConstructSmtpReply(int replyCode){
	char* replyString=malloc(sizeof(char)*BUFFER_SIZE);
	memset(replyString,0x00,BUFFER_SIZE);
	sprintf(replyString,"%d %s",replyCode,GetSmtpReplyTextByCode(replyCode));
	return(replyString);
}
void DefineReply(SmtpStatus *Status,char *clientAwnser){
	size_t startStr;
	char *buff = malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(buff,clientAwnser);
	char **pBuff = &buff;
	char *currentToken = malloc(sizeof(char)*BUFFER_SIZE);
	char *Token[MAX_TOKEN];

	static char delimiters[] = " :<>@";


	startStr = sizeof(char)*4;
	int i=0,replyCode=0;
	//découpage de la reponse en Token (separation des espace et <>)
	if((*Status).statusCode!=354){
		while ((currentToken = strsep(pBuff,delimiters))){
			if (strcmp(currentToken, "") && strcmp(currentToken, "from") &&
						strcmp(currentToken, "to"))
					{
						Token[i] = currentToken;
						i++;
					}
		}
	}
	//Verifie que l'instruction n'est pas quit
	if(strncmp("QUIT\n\r",clientAwnser,startStr)==0){
		replyCode=221;
	}else{
		switch((*Status).statusCode){
		// Smtp Initialisaiton
		case 0 :
			// Si pas de reponse client (initialisation de la connexion)
			replyCode=220;
			break;
		// smtp waiting ehlo
		case 220 :
			if(strncmp("EHLO",clientAwnser,startStr)==0 || strncmp("HELO",clientAwnser,startStr) == 0){
				replyCode=250;
			}else{
				replyCode=500;
			}
			break;
		// Smtp Heloed Waiting for MAIL FROM
		case 250 :
			if(strncmp("MAIL",Token[0],startStr)==0){
				//The Message Started with MAIL
				//Check if contains a "FROM" part
				printf("\nFROM ? : %s@%s \n",Token[2],Token[3]);
				//On alimente Le status avec l'adresse source
				(*Status).FROM.user=strdup(Token[2]);
				(*Status).FROM.domain=strdup(Token[3]);
				replyCode=250;
			}else if(strncmp("RCPT",Token[0],startStr)==0){
				printf("\n TO ? : %s@%s \n",Token[2],Token[3]);
				replyCode=250;
				// On alimente le status avec laddresse de dest.
				(*Status).TO.user=strdup(Token[2]);
				(*Status).TO.domain=strdup(Token[3]);
			}else if(strncmp("RSET",Token[0],startStr)==0){
				//Reset Received
				printf("\n RESET !");
				memset((*Status).FROM.domain,0x00,strlen((*Status).FROM.domain));
				memset((*Status).FROM.user,0x00,strlen((*Status).FROM.user));
				memset((*Status).TO.domain,0x00,strlen((*Status).TO.domain));
				memset((*Status).TO.user,0x00,strlen((*Status).TO.user));
				replyCode=250;
			}else if(strncmp("DATA",Token[0],startStr)==0){
				printf("\n DATA !");
				replyCode=354;
			}else{
				replyCode=500;
			}
		break;
		//Fin Data
		case 354:
			//On pourra ajouter du check de DATA
			printf("Ajout de la data : %s/n",clientAwnser);
			memmove((*Status).DATA,buff,strlen(buff));
			replyCode=250;
			break;
		default:
			replyCode=-1;
			break;
		//après avoir défini le replyCode on construit la string

		}
	}
	(*Status).statusCode= replyCode;
	(*Status).awnser=ConstructSmtpReply(replyCode);

}
