#pragma once
#ifndef __SSLMAILSEND_H
#define __SSLMAILSEND_H

#ifdef __cplusplus
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

#include "CSmtp.h"

EXPORT bool doMain(char **RetErr, const char *Subject, const char *Sender, const char *MailTo,const char *msg,int mailIdx);


/*
EXPORT void SetSecurityType(SMTP_SECURITY_TYPE type);
EXPORT void SetLogin(const char *);
EXPORT void SetPassword(const char *);
EXPORT void SetSenderName(const char *);
EXPORT void SetSenderMail(const char *);
EXPORT void SetReplyTo(const char *);
EXPORT void SetSubject(const char *);
EXPORT void AddRecipient(const char *);
EXPORT void SetXPriority(CSmptXPriority);
EXPORT void SetXMailer(const char *);
EXPORT void AddMsgLine(const char *text);
EXPORT void ModMsgLine(unsigned int line, const char *text);
EXPORT void DelMsgLine(unsigned int line);
EXPORT void Send();
EXPORT void DisconnectRemoteServer();*/

#endif



