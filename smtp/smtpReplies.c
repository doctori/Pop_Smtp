/*
 *  SmtpReplies.c
 *
 */
#include <stdlib.h>
#include "SmtpReplies.h"

BEGIN_STRUCT_DECLARATION(SmtpReply)
	int replyCode;
	const char* replyText;
END_STRUCT_DECLARATION(SmtpReply)

SmtpReply smtpReplies[] =
{
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

const char* GetSmtpReplyTextByCode(int replyCode)
{
	for (int i = 0; i < SMTP_REPLIES_COUNT; i++)
	{
		if (smtpReplies[i].replyCode == replyCode)
			return smtpReplies[i].replyText;
	}
	
	return NULL;
}