#include "WSetupDll.h"
#include "CoreImpl.h"

extern "C" bool  ex_get_export_information(wchar_t **ppnicguid,wchar_t **ppssid,wchar_t **ppprofilename)
{
	bool bret = false;

	*ppnicguid		= NULL;
	*ppssid			= NULL;
	*ppprofilename	= NULL;


	if (NULL==ppnicguid || NULL==ppssid || NULL==ppprofilename)
	{
		return bret;
	}
	
	std::list<std::wstring> inf_list;
	if (!enum_interfaces(inf_list))
	{
		return bret;
	}

	std::list<std::wstring>::iterator iter;
	std::wstring ssid,profilename;

	for (iter=inf_list.begin();iter!=inf_list.end();++iter)
	{
		if (get_connected_ssid_profilename(iter->c_str(),ssid,profilename))
		{
			*ppnicguid		= new wchar_t[iter->length()+1];
			*ppssid			= new wchar_t[ssid.length()+1];
			*ppprofilename	= new wchar_t[profilename.length()+1];
			if (*ppnicguid && *ppssid && *ppprofilename)
			{
				wcscpy(*ppnicguid,iter->c_str());
				wcscpy(*ppssid,ssid.c_str());
				wcscpy(*ppprofilename,profilename.c_str());
				bret = true;
			}

			break;
		}
	}
	return bret;
}

extern "C" void  ex_free_export_information(wchar_t *pnicguid,wchar_t *pssid,wchar_t *pprofilename)
{
	delete [] pnicguid;
	delete [] pssid;
	delete [] pprofilename;
}

extern "C" bool  ex_export_to_file(const wchar_t *pnicguid,const wchar_t *pssid,
								 const wchar_t *pprofilename,const wchar_t *pfilename)
{
	bool bret = false;

	if(NULL==pnicguid || NULL==pssid || NULL==pprofilename || NULL==pfilename)
	{
		return bret;
	}

	DWORD			os_version					= GetOsVersion(NULL);
	std::wstring	str_password;
	bool			bnetworkkey_or_passphrase	= false;
	bool			bprotect					= false;

	if (os_version>=6)//vista and later
	{
		if (get_password_fromprofile_new(pprofilename,str_password,bnetworkkey_or_passphrase,&bprotect))
		{
			if (!bprotect)
			{
				bret = true;
			}
		}
	}
	else
	{
		wchar_t *pxml_profile = NULL;
		if (get_profile(pnicguid,pprofilename,&pxml_profile))
		{
			if (get_password_fromprofile(pxml_profile,str_password,bnetworkkey_or_passphrase,&bprotect))
			{
				if (!bprotect)
				{
					bret = true;
				}
			}
			free_profilexml_buffer(pxml_profile);
		}
	}

	if (!bret)
	{
		return bret;
	}

	//reset flag
	bret = false;
	//ssid and password encrypted and save to file
	std::wstring cfg_str;
	if (build_config_string(pssid,str_password.c_str(),bnetworkkey_or_passphrase,cfg_str))
	{
		if (save_to_file(pfilename,cfg_str.c_str()))
		{
			bret = true;
		}
	}

	return bret;
}

extern "C" bool  ex_export_to_file2(const wchar_t *pssid,const wchar_t *ppassword,const wchar_t *pfilename)
{
	bool bret = false;
	std::wstring cfg_str;

	if(NULL==pssid || NULL==ppassword || NULL==pfilename)
	{
		return bret;
	}

	if (build_config_string(pssid,ppassword,true,cfg_str))
	{
		if (save_to_file(pfilename,cfg_str.c_str()))
		{
			bret = true;
		}
	}

	return bret;
}

//---------------------------增加认证和加密方式---------------------------------------
extern "C" bool  ex_export_to_file2_v2(const wchar_t *pssid,const wchar_t *ppassword,
												const wchar_t *auth,const wchar_t *cipher,const wchar_t *pfilename)
{
	bool bret = false;
	std::wstring cfg_str;

	if(NULL==pssid || NULL==ppassword || 
		NULL==auth || NULL==cipher || NULL==pfilename)
	{
		return bret;
	}

	if (build_config_string_v2(pssid,ppassword,true,auth,cipher,cfg_str))
	{
		if (save_to_file(pfilename,cfg_str.c_str()))
		{
			bret = true;
		}
	}

	return bret;
}

//--------------------------------结尾---------------------------------------------

extern "C" bool  ex_get_logicaldriver_letters(wchar_t **ppletters)
{
	bool bret = false;
	if (NULL==ppletters)
	{
		return bret;
	}
	*ppletters = NULL;

	DWORD size=GetLogicalDriveStrings(0,NULL);
	if (0==size)
	{
		return bret;
	}

	wchar_t *buffer = new wchar_t[size];
	if (GetLogicalDriveStrings(size,buffer))
	{
		*ppletters	= buffer;
		bret		= true;
	}

	return bret;
}

extern "C" void  ex_free_logicaldriver_letters(wchar_t *pletters)
{
	delete [] pletters;
}

extern "C" bool  ex_is_logicaldriver_removable(const wchar_t* ldriver)
{
	return is_logicaldriver_removable(ldriver);
}

extern "C" bool  ex_is_logicaldriver_cdrom(const wchar_t* ldriver)
{
	return is_logicaldriver_cdrom(ldriver);
}

extern "C" bool  ex_is_logicaldriver_system(const wchar_t* ldriver)
{
	return is_logicaldriver_system(ldriver);
}
