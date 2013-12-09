// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "SslMailSend.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


EXPORT bool doMain(char **RetErr, const char *Subject, const char *Sender, const char *MailTo, const char *msg,int mailIdx)
{
	bool bError = false;

	try
	{
		CSmtp mail;

		mail.SetSMTPServer("smtp.gmail.com",587);
		mail.SetSecurityType(USE_TLS);
		mail.SetLogin(Sender);
		mail.SetPassword("Siteview@NETGEAR@888");
  		mail.SetSenderName("netgeargenie");
  		mail.SetSenderMail(Sender);
  		mail.SetReplyTo(MailTo);
  		mail.SetSubject(Subject);
  		mail.AddRecipient(MailTo);
  		mail.SetXPriority(XPRIORITY_NORMAL);
  		mail.SetXMailer("The Bat! (v3.02) Professional");
  		mail.AddMsgLine(msg);
		mail.Send();
	}
	catch(ECSmtp e)
	{
		strcpy(*RetErr, e.GetErrorText().c_str());
		bError = true;
	}
	if(!bError)
	{
		strcpy(*RetErr, "Mail was send successfully.\n");
		return true;
	}
ret_flag:

	return false;
}
