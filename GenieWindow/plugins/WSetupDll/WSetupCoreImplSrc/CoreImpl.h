#ifndef WAUTOCONFIG_COREIMPL_H
#define WAUTOCONFIG_COREIMPL_H

#include <Windows.h>
#include <wlanapi.h>
#include <list>
#include <string>

bool get_profile(const wchar_t *nicguid,const wchar_t *profilename,wchar_t **profilexml);
void free_profilexml_buffer(wchar_t *profile_buffer);
bool get_profile_fromstorage(const wchar_t *filename,std::wstring &profilexml);
bool get_password_fromprofile_new(const wchar_t *profilename,std::wstring &pwd,
								  bool &bnetworkkey_or_passphrase,bool *protect);
bool get_ssid_profilenames(const wchar_t *nicguid,const wchar_t *ssid,std::list<std::wstring> &profile_names);
bool get_password_fromprofile(const wchar_t *xmlprofile,std::wstring &pwd
							  ,bool &bnetworkkey_or_passphrase,bool *protect);
bool decrypt_password(const wchar_t *pwd,std::wstring &out_pwd);
bool set_profile(const wchar_t *nicguid,const wchar_t *profilexml);
bool enum_interfaces(std::list<std::wstring> &inf_list);
bool get_connected_ssid_profilename(const wchar_t *nicguid,std::wstring &ssid,std::wstring &profilename);
bool build_config_string(const wchar_t *ssid,const wchar_t *pwd,bool bnetworkKey_or_passPhrase,std::wstring &cfg_str);
bool parse_config_string(const wchar_t *cfg_str,std::wstring &ssid,std::wstring &pwd,bool &bnetworkKey_or_passPhrase);
bool build_config_string_v2(const wchar_t *ssid,const wchar_t *pwd,bool bnetworkKey_or_passPhrase,
							const wchar_t *auth,const wchar_t *cipher,std::wstring &cfg_str);
bool parse_config_string_v2(const wchar_t *cfg_str,std::wstring &ssid,std::wstring &pwd,bool &bnetworkKey_or_passPhrase,
							std::wstring &auth,std::wstring &cipher);
bool save_to_file(const wchar_t *filename,const wchar_t *str_config);
bool load_from_file(const wchar_t *filename,std::wstring &str_config);
wchar_t* ssid2stringw(wchar_t *buf, int count,PDOT11_SSID pSsid);
bool SetInterfaceProfile(const wchar_t *nicguid,const wchar_t *ssid,
						 const wchar_t *auth,const wchar_t *cipher,const wchar_t *pwd,bool bnetworkKey_or_passPhrase);
bool connect2wlan(const wchar_t *nicguid,const wchar_t *ssid,const wchar_t *pwd,bool bnetworkKey_or_passPhrase);
const wchar_t *GetAuthAlgoString(DOT11_AUTH_ALGORITHM dot11AuthAlgo);
const wchar_t *GetCipherAlgoString(DOT11_CIPHER_ALGORITHM dot11CipherAlgo);
DWORD GetOsVersion(DWORD* pdwMinorVersion);
bool get_app_dir(std::wstring &dir_path);
bool get_logicaldriver_letters(std::list<std::wstring> &letters);
bool is_logicaldriver_removable(const wchar_t* ldriver);
bool is_logicaldriver_cdrom(const wchar_t* ldriver);
bool is_logicaldriver_system(const wchar_t* ldriver);
bool import_profile(const wchar_t *nicguid,const wchar_t *ssid,const wchar_t *pwd,bool bnetworkKey_or_passPhrase,
					const wchar_t *auth,const wchar_t *cipher);
bool tryconnect_afterimport(const wchar_t *nicguid,const wchar_t *ssid);
#endif
