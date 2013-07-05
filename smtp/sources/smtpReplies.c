/*
 *  SmtpReplies.c
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "smtpReplies.h"




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
SmtpStatus DefineReply(int pastStatus,char *clientAwnser){
	int replyCode=0;
	//Verifie que l'instruction n'est pas quit
	if(strncmp("QUIT\n\r",clientAwnser,sizeof(char)*4)==0){
		replyCode=221;
	}else{
		switch(pastStatus){
		// Smtp Initialisaiton
		case 0 :
			// Si pas de reponse client (initialisation de la connexion)
			replyCode=220;
			break;
		// smtp waiting ehlo
		case 220 :
			if(strcmp("EHLO",clientAwnser)==0 || strcmp("HELO",clientAwnser) == 0){
				replyCode=250;
			}
			break;
		default:
			replyCode=-1;
			break;
		//après avoir défini le replyCode on construit la string

		}
	}
	SmtpStatus Status = {replyCode , ConstructSmtpReply(replyCode) };
	return(Status);
}