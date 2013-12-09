#ifndef _WINDOWS_TOOLS_
#define _WINDOWS_TOOLS_

#include <windows.h>
#include <QDebug>

extern "C" {
        typedef BOOL (*_IsRunAsAdmin)();
        typedef BOOL (*_IsUserInAdminGroup)();
        typedef BOOL (*_IsProcessElevated)();
        typedef DWORD (*_GetProcessIntegrityLevel)();
};

bool elevate()
{
       bool ret=false;
    _IsRunAsAdmin IsRunAsAdmin;
    _IsUserInAdminGroup IsUserInAdminGroup;
    _IsProcessElevated IsProcessElevated;
    _GetProcessIntegrityLevel GetProcessIntegrityLevel	;
		
    HINSTANCE hinstDLL=NULL;
    hinstDLL=LoadLibraryA("windowstool.dll");
    if (hinstDLL)
    {
        IsRunAsAdmin=(_IsRunAsAdmin)GetProcAddress(hinstDLL,"IsRunAsAdmin");
        IsUserInAdminGroup=(_IsUserInAdminGroup)GetProcAddress(hinstDLL,"IsUserInAdminGroup");
        IsProcessElevated=(_IsProcessElevated)GetProcAddress(hinstDLL,"IsProcessElevated");
        GetProcessIntegrityLevel=(_GetProcessIntegrityLevel)GetProcAddress(hinstDLL,"GetProcessIntegrityLevel");

//code start
			
        BOOL fIsRunAsAdmin;
        try
        {
            fIsRunAsAdmin = IsRunAsAdmin();
        }
        catch (DWORD dwError)
        {
            qDebug("IsRunAsAdmin");
        }

            // Elevate the process if it is not run as administrator.
        if (!fIsRunAsAdmin)
        {
            wchar_t szPath[MAX_PATH];
            if (GetModuleFileName(NULL, szPath, sizeof(szPath)))
            {
                    // Launch itself as administrator.
                    SHELLEXECUTEINFO sei = { sizeof(sei) };
                    sei.lpVerb = L"runas";
                    sei.lpFile = szPath;
                    sei.hwnd = NULL;
                    sei.nShow = SW_NORMAL;

                    if (!ShellExecuteEx(&sei))
                    {
                        DWORD dwError = GetLastError();
                        if (dwError == ERROR_CANCELLED)
                        {
                            // The user refused to allow privileges elevation.
                            // Do nothing ...
                        }
                    }
                    else
                    {
                        qDebug()<<"evevate error!!!!";
                    }
                }
            }
        try
        {
            ret = IsRunAsAdmin();
        }
        catch (DWORD dwError)
        {
            qDebug("catched exception when IsRunAsAdmin()");
        }

            FreeLibrary(hinstDLL);
//code end			
        }//if (hinstDLL)
        else
        {
            qDebug()<<"can not find windowstool.dll";
        }

        return ret;

		
		
		
}
#endif
