#ifndef SMTP_REPLIES
#define SMTP_REPLIES
#define BUFFER_SIZE 2048
#define SMTP_REPLIES_COUNT 16
typedef struct SmtpAddress{
	char* user;
	char* domain;
}SmtpAddress;
typedef struct SmtpStatus{
	int statusCode;
	char* awnser;
	SmtpAddress FROM;
	SmtpAddress TO;
	char *DATA;
}SmtpStatus;

typedef struct SmtpReply{
	int replyCode;
	char* replyText;

	}SmtpReply;
char* GetSmtpReplyTextByCode(int replyCode);
char* ConstructSmtpReply(int replyCode);
void DefineReply(SmtpStatus *Status,char *clientAwnser);
SmtpStatus SmtpStatusClone(SmtpStatus SmtpStatusSource);
SmtpAddress SmtpAddressClone(SmtpAddress SmtpAddressSource);
char * SmtpAdressToString(SmtpAddress Adress);
#endif
