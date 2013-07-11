#ifndef SMTP_REPLIES
#define SMTP_REPLIES
#define BUFFER_SIZE 2048
#define ADR_SIZE 256
#define SMTP_REPLIES_COUNT 16
typedef struct SmtpAddress{
	char* user;
	char* domain;
}SmtpAddress;
typedef struct SmtpStatus{
	int statusCode;
	char* awnser;
	SmtpAddress FROM;
	SmtpAddress TO[7];
	char *DATA;
}SmtpStatus;

typedef enum {
        TRUE  = (1==1),
        FALSE = (1==0),
} bool_t;

typedef struct SmtpReply{
	int replyCode;
	char* replyText;

	}SmtpReply;
bool_t isAddress(SmtpAddress SmtpAddress);
char* GetSmtpReplyTextByCode(int replyCode);
char* ConstructSmtpReply(int replyCode);
void SmtpStatusAddTO(SmtpStatus* Status, SmtpAddress TO);
void DefineReply(SmtpStatus *Status,char *clientAwnser);
SmtpStatus SmtpStatusClone(SmtpStatus SmtpStatusSource);
SmtpAddress SmtpAddressClone(SmtpAddress SmtpAddressSource);
void SmtpAdressToString(char* buff,SmtpAddress Adress);
#endif
