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
	SmtpAddress* FROM;
	SmtpAddress* TO[8];
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
bool_t isAddress(SmtpAddress* SmtpAddress);
char* GetSmtpReplyTextByCode(int replyCode);
char* ConstructSmtpReply(int replyCode);
void DefineReply(SmtpStatus *Status,char *clientAwnser);

SmtpAddress* SmtpAddressClone(SmtpAddress* SmtpAddressSource);
void AddSmtpTO(SmtpStatus *SmtpStatusSource,SmtpAddress* SmtpAddressToAdd);
SmtpAddress* NewSmtpAddress(void);
SmtpStatus* NewSmtpStatus(void);
void SmtpAdressToString(char* buff,SmtpAddress* Adress);
#endif
