#ifndef SMTP_REPLIES
#define SMTP_REPLIES
#define BUFFER_SIZE 2048
#define SMTP_REPLIES_COUNT 16
typedef struct SmtpStatus{
	int statusCode;
	char* awnser;
}SmtpStatus;
typedef struct SmtpReply{
	int replyCode;
	char* replyText;
}SmtpReply;
char* GetSmtpReplyTextByCode(int replyCode);
char* ConstructSmtpReply(int replyCode);
SmtpStatus DefineReply(int smtpStatus,char *clientAwnser);

#endif
