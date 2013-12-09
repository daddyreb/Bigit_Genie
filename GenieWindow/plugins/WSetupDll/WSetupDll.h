#ifndef WSETUPDLL_H
#define WSETUPDLL_H
extern "C" bool  ex_get_export_information(wchar_t **ppnicguid,wchar_t **ppssid,wchar_t **ppprofilename);
extern "C" void  ex_free_export_information(wchar_t *pnicguid,wchar_t *pssid,wchar_t *pprofilename);

extern "C" bool  ex_export_to_file(const wchar_t *pnicguid,const wchar_t *pssid,const wchar_t *pprofilename,const wchar_t *pfilename);
extern "C" bool  ex_export_to_file2(const wchar_t *pssid,const wchar_t *ppassword,const wchar_t *pfilename);
extern "C" bool  ex_export_to_file2_v2(const wchar_t *pssid,const wchar_t *ppassword,
												const wchar_t *auth,const wchar_t *cipher,const wchar_t *pfilename);

extern "C" bool  ex_get_logicaldriver_letters(wchar_t **ppletters);
extern "C" void  ex_free_logicaldriver_letters(wchar_t *pletters);

extern "C" bool  ex_is_logicaldriver_removable(const wchar_t* ldriver);
extern "C" bool  ex_is_logicaldriver_cdrom(const wchar_t* ldriver);
extern "C" bool  ex_is_logicaldriver_system(const wchar_t* ldriver);
#endif
