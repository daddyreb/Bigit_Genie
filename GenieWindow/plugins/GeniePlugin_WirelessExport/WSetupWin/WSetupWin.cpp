// WSetupWin.cpp : Defines the entry point for the application.
//
#include <Windows.h>
#include "CoreImpl.h"

#define  EXPORT_WIRELESS_CONFIG_FILENAME L"wsetup.ws"

int APIENTRY WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR    lpCmdLine,
                   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	std::wstring							str_cfgfilepath;
	std::wstring							str_cfgstring;
	std::wstring							str_ssid;
	std::wstring							str_password;
	std::wstring							str_auth;
	std::wstring							str_cipher;
	bool									bnetworkkey_or_passphrase = false;

	std::list<std::wstring>					inf_list;
	std::list<std::wstring>::iterator		iter;

	bool						bret = false;

	get_app_dir(str_cfgfilepath);
	str_cfgfilepath+=EXPORT_WIRELESS_CONFIG_FILENAME;
	
	if (load_from_file(str_cfgfilepath.c_str(),str_cfgstring))
	{
		/*if (parse_config_string(str_cfgstring.c_str(),str_ssid,str_password,bnetworkkey_or_passphrase))
		{
			bret = true;
		}*/
		if (parse_config_string_v2(str_cfgstring.c_str(),str_ssid,str_password,bnetworkkey_or_passphrase
			,str_auth,str_cipher))
		{
			bret = true;
		}
	}

	if (bret)
	{
		bret = false;
		if (enum_interfaces(inf_list))
		{
			for (iter=inf_list.begin();iter!=inf_list.end();++iter)
			{
				/*if (connect2wlan(iter->c_str(),str_ssid.c_str(),
					str_password.c_str(),bnetworkkey_or_passphrase))
				{
					bret = true;
					break;
				}*/
				if (import_profile(iter->c_str(),str_ssid.c_str(),str_password.c_str(),
					bnetworkkey_or_passphrase,str_auth.c_str(),str_cipher.c_str()))
				{
					bret = true;
					tryconnect_afterimport(iter->c_str(),str_ssid.c_str());
					break;
				}
			}
		}
	}


	if (bret)
	{
		std::wstring str_prompt = L"Import wireless network \"";
		str_prompt += str_ssid;
		str_prompt += L"\" successfully.";
		MessageBox(NULL,str_prompt.c_str(),L"Information",MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		if(inf_list.size()==0)
		{
			MessageBox(NULL,L"Import wireless config failed! Wireless adapter is not enabled.",L"Error",MB_OK|MB_ICONERROR);
		}
		else
		{
			MessageBox(NULL,L"Import wireless config failed.",L"Error",MB_OK|MB_ICONERROR);
		}
		
	}
	
	return !bret;
}