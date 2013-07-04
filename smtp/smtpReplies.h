#ifndef SMTP_REPLIES
#define SMTP_REPLIES

#include <KAnsiCWrappers.h>

#define SMTP_REPLIES_COUNT 16

const char* GetSmtpReplyTextByCode(int replyCode);

#endif