#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>

#include "inifile_tool.h"



/****************************** INI file support ******************************/
// This function locate the specified section in the ini file.
static int ini_LocateSection(FILE* fp, const char* pSection, FILE* bak_fp)
{
	char szBuff[INI_MAXLINE + 1];
	char* current;
	char* tail;

	while (1)
	{
		if (!fgets(szBuff, INI_MAXLINE, fp))
		{
			if (feof(fp))
				return INI_SECTIONNOTFOUND;
			else
				return INI_FILEIOFAILED;
		}
		else if (bak_fp && fputs(szBuff, bak_fp) == EOF)
			return INI_FILEIOFAILED;

		current = szBuff;

		while (*current == ' ' || *current == '\t')
			current++; 

		if (*current == ';' || *current == '#')
			continue;

		if (*current++ == '[')
			while (*current == ' ' || *current == '\t')
				current ++;
		else
			continue;

		// Locate the tail.
		tail = current;
		while (*tail != ']' &&
			*tail != '\n' &&
			*tail != ';' &&
			*tail != '#' &&
			*tail != '\0')
			tail++;
		*tail = '\0';
		while (*tail == ' ' || *tail == '\t')
		{
			*tail = '\0';
			tail--;
		}

		if (strcmp(current, pSection) == 0)
			return INI_OK;
	}

	return INI_SECTIONNOTFOUND;
}

// This function locate the specified key in the ini file.
static int ini_LocateKeyValue(FILE* fp, const char* pKey, BOOL bCurSection,
	char* pValue, int iLen, FILE* bak_fp, char* nextSection)
{
	char szBuff[INI_MAXLINE + 1 + 1];
	char* current;
	char* tail;
	char* value;

	while (1)
	{
		if (!fgets(szBuff, INI_MAXLINE, fp))
			return INI_FILEIOFAILED;
		if (szBuff[strlen(szBuff) - 1] == '\n')
			szBuff[strlen(szBuff) - 1] = '\0';
		current = szBuff;

		while (*current == ' ' || *current == '\t')
			current++; 

		if (*current == ';' || *current == '#')
			continue;

		if (*current == '[')
		{
			if (bCurSection)
			{
				strcpy(nextSection, current);
				return INI_KEYNOTFOUND;
			}
			else
				continue;
		}

		tail = current;
		while (*tail != '=' &&
			*tail != '\n' &&
			*tail != ';' &&
			*tail != '#' &&
			*tail != '\0')
			tail++;

		value = tail + 1;
		if (*tail != '=')
			*value = '\0'; 

		*tail-- = '\0';
		while (*tail == ' ' || *tail == '\t')
		{
			*tail = '\0';
			tail--;
		}

		if (strcmp(current, pKey) == 0)
		{
			tail = value;
			while (*tail != '\n' && *tail != '\0')
				tail++;
			*tail = '\0'; 

			if (pValue)
				strncpy(pValue, value, iLen);

			return INI_OK;
		}
		else if (bak_fp && *current != '\0')
			fprintf(bak_fp, "%s=%s\n", current, value);
	}

	return INI_KEYNOTFOUND;
}

/* Function: GetValueFromIniFile(const char* pIniFile, const char* pSection,
 *  							 const char* pKey, char* pValue, int iLen);
 * Parameter:
 *     pIniFile: ini file path name.
 *     pSection: Section name.
 *     pKey:	 Key name.
 *     pValue:   The buffer will store the value of the key.
 *     iLen:	 The max length of value string.
 * Return:
 *     int  			 meaning
 *     INI_FILENOTFOUND 		  The ini file not found. 
 *     INI_SECTIONNOTFOUND  	  The section is not found. 
 *     INI_EKYNOTFOUND  	  The Key is not found.
 *     INI_OK   		 OK.
 */
int GetValueFromIniFile(const char* pIniFile, const char* pSection,
	const char* pKey, char* pValue, int iLen)
{
	if (pValue)
		*pValue = 0;
	FILE* fp;
	char tempSection[INI_MAXLINE + 2];

	if (!(fp = fopen(pIniFile, "r")))
		return INI_FILENOTFOUND;

	if (pSection)
		if (ini_LocateSection(fp, pSection, NULL) != INI_OK)
		{
			fclose(fp);
			return INI_SECTIONNOTFOUND;
		}

	if (ini_LocateKeyValue(fp, pKey, pSection != NULL, pValue, iLen, NULL,
			tempSection) != INI_OK)
	{
		fclose(fp);
		return INI_KEYNOTFOUND;
	}

	fclose(fp);
	return INI_OK;
}

/* Function: GetIntValueFromIniFile(const char* pIniFile, const char* pSection,
 *  							 const char* pKey);
 * Parameter:
 *     pIniFile: Ini file path name.
 *     pSection: Section name.
 *     pKey:	 Key name.
 * Return:
 *     int  					meaning
 *     INI_FILENOTFOUND 			The ini file not found. 
 *     INI_SECTIONNOTFOUND  		The section is not found. 
 *     INI_EKYNOTFOUND  			The Key is not found.
 *     INI_OK   					OK.
 */
int GetIntValueFromIniFile(const char* pIniFile, const char* pSection,
	const char* pKey, int* value)
{
	int ret;
	char szBuff[51];

	ret = GetValueFromIniFile(pIniFile, pSection, pKey, szBuff, 50);
	if (ret < 0)
		return ret;

	*value = strtol(szBuff, NULL, 0);
	if ((*value == LONG_MIN || *value == LONG_MAX) && errno == ERANGE)
		return INI_INTCONV;

	return INI_OK;
}

static int ini_CopyAndLocate(FILE* ini_fp, FILE* tmp_fp, const char* pSection,
	const char* pKey, char* tempSection)
{
	if (pSection && ini_LocateSection(ini_fp, pSection, tmp_fp) != INI_OK)
		return INI_SECTIONNOTFOUND;

	if (ini_LocateKeyValue(ini_fp, pKey, pSection != NULL, NULL, 0, tmp_fp,
			tempSection) != INI_OK)
		return INI_KEYNOTFOUND;

	return INI_OK;
}

static int ini_FileCopy(FILE* sf, FILE* df)
{
	char line[INI_MAXLINE + 1];

	while (fgets(line, INI_MAXLINE + 1, sf) != NULL)
		if (fputs(line, df) == EOF)
		{
			return INI_FILEIOFAILED;
		}

	return INI_OK;
}

/* Function: SetValueToIniFile(const char* pIniFile, const char* pSection,
 *  							 const char* pKey, char* pValue);
 * Parameter:
 *     pIniFile: ini file path name.
 *     pSection: Section name.
 *     pKey:	 Key name.
 *     pValue:   Value.
 * Return:
 *     int  					meaning
 *     INI_FILENOTFOUND 		The ini file not found.
 *     INI_TMPFILEFAILED		Create tmp file failure.
 *     INI_OK   				OK.
 */

int SetValueToIniFile(const char* pIniFile, const char* pSection,
	const char* pKey, const char* pValue)
{
	FILE* ini_fp;
	FILE* tmp_fp;
	int rc;
	char tempSection[INI_MAXLINE + 2]={0};

	if ((tmp_fp = tmpfile()) == NULL)
		return INI_TMPFILEFAILED;

	if (!(ini_fp = fopen(pIniFile, "r+")))
	{
		fclose(tmp_fp);
		if (!(ini_fp = fopen(pIniFile, "w")))
			return INI_FILEIOFAILED;
		fprintf(ini_fp, "[%s]\n", pSection);
		fprintf(ini_fp, "%s=%s\n", pKey, pValue);
		fclose(ini_fp);
		return INI_OK;
	}

	switch (ini_CopyAndLocate(ini_fp, tmp_fp, pSection, pKey, tempSection))
	{
	case INI_SECTIONNOTFOUND:
		fprintf(tmp_fp, "\n[%s]\n", pSection);
		fprintf(tmp_fp, "%s=%s\n", pKey, pValue);
		break;

	case INI_KEYNOTFOUND:
		fprintf(tmp_fp, "%s=%s\n\n", pKey, pValue);
		fprintf(tmp_fp, "%s\n", tempSection);
		break;

	default:
		fprintf(tmp_fp, "%s=%s\n", pKey, pValue);
		break;
	}

	if ((rc = ini_FileCopy(ini_fp, tmp_fp)) != INI_OK)
		goto error;

	// replace ini content with tmp file content
	// truncate ini content first
	fclose(ini_fp);
	if (!(ini_fp = fopen(pIniFile, "w")))
		return INI_FILEIOFAILED;

	rewind(tmp_fp);
	rc = ini_FileCopy(tmp_fp, ini_fp);

	error : fclose(ini_fp);
	fclose(tmp_fp);
	return rc;
}

