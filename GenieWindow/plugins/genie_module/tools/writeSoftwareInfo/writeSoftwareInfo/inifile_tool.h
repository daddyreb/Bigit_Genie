#ifndef BOOL
typedef	int BOOL;
#endif
/*
#ifndef	FALSE
#define		FALSE		0
#endif

#ifndef	TRUE
#define		TRUE		1
#endif
*/

#define		INI_MAXLINE		512
#define 	_NAME_MAX 256


#define		INI_OK			0
#define		INI_SECTIONNOTFOUND	10
#define		INI_FILEIOFAILED	20
#define		INI_KEYNOTFOUND		30
#define		INI_INTCONV		40
#define		INI_FILENOTFOUND	50
#define		INI_TMPFILEFAILED	60  					


int GetIntValueFromIniFile(const char* pIniFile, const char* pSection,
	const char* pKey, int* value);
int GetValueFromIniFile(const char* pIniFile, const char* pSection,
	const char* pKey, char* pValue, int iLen); 

int SetValueToIniFile(const char* pIniFile, const char* pSection,
	const char* pKey, const char* pValue);

