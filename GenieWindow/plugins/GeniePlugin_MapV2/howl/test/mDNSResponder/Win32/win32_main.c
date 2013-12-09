/*
 * Copyright 2003, 2004 Porchdog Software. All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or without modification,
 *	are permitted provided that the following conditions are met:
 *
 *		1. Redistributions of source code must retain the above copyright notice,
 *		   this list of conditions and the following disclaimer.   
 *		2. Redistributions in binary form must reproduce the above copyright notice,
 *		   this list of conditions and the following disclaimer in the documentation
 *		   and/or other materials provided with the distribution.
 *
 *	THIS SOFTWARE IS PROVIDED BY PORCHDOG SOFTWARE ``AS IS'' AND ANY
 *	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *	IN NO EVENT SHALL THE HOWL PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *	OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *	OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	The views and conclusions contained in the software and documentation are those
 *	of the authors and should not be interpreted as representing official policies,
 *	either expressed or implied, of Porchdog Software.
 */

#define	WIN32_WINDOWS		0x0401		// Needed to use waitable timers.
#define	_WIN32_WINDOWS		0x0401		// Needed to use waitable timers.

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"win32_main.h"
#include <mDNSServant.h>
#include <howl.h>

#define SERVICE_NAME "mDNSResponder"

/*
 * Prototypes
 */
static void
install_service(
			int		argc,
			char	*	argv[]);


static void
unregister_service(
			int		argc,
			char	*	argv[]);


static DWORD
start_mDNSResponder();


static DWORD
stop_mDNSResponder();


static BOOL WINAPI
console_ctrl_handler(
			DWORD		type);


/*
 * Event used to hold ServiceMain from completing
 */
static HANDLE terminateEvent	= NULL;

/*
 * Handle used to communicate status info with
 * the SCM. Created by RegisterServiceCtrlHandler
 */
static SERVICE_STATUS_HANDLE serviceStatusHandle;

/*
 * Flags holding current state of service
 */
static HANDLE				g_console_event	=	NULL;
static BOOL					runningService		=	FALSE;
static BOOL					pauseService		=	FALSE;
static sw_mdns_servant	g_servant;


/*
 * Thread for the actual work
 */
HANDLE threadHandle				= 0;


void
service_error_handler(char *s, DWORD err)
{
	sw_debug(SW_LOG_NOTICE, "%s %d\n", s, err);
	ExitProcess(err);
}


// Initializes the service by starting its thread
BOOL
InitService()
{
	DWORD id;

	// Start the service's thread
	threadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) start_mDNSResponder, 0, 0, &id);

	if (threadHandle==0)
		return FALSE;
	else
	{
		runningService = TRUE;
		return TRUE;
	}
}

// Resumes a paused service
VOID ResumeService()
{
	pauseService=FALSE;
	ResumeThread(threadHandle);
}

// Pauses the service
VOID PauseService()
{
	pauseService = TRUE;
	SuspendThread(threadHandle);
}

// Stops the service by allowing ServiceMain to
// complete
VOID StopService() 
{
	stop_mDNSResponder();

	runningService=FALSE;
	// Set the event that is holding ServiceMain
	// so that ServiceMain can return
	SetEvent(terminateEvent);
}

// This function consolidates the activities of 
// updating the service status with
// SetServiceStatus
BOOL SendStatusToSCM (DWORD dwCurrentState,
	DWORD dwWin32ExitCode, 
	DWORD dwServiceSpecificExitCode,
	DWORD dwCheckPoint,
	DWORD dwWaitHint)
{
	BOOL success;
	SERVICE_STATUS serviceStatus;

	// Fill in all of the SERVICE_STATUS fields
	serviceStatus.dwServiceType =
		SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = dwCurrentState;

	// If in the process of something, then accept
	// no control events, else accept anything
	if (dwCurrentState == SERVICE_START_PENDING)
		serviceStatus.dwControlsAccepted = 0;
	else
		serviceStatus.dwControlsAccepted = 
			SERVICE_ACCEPT_STOP |
			SERVICE_ACCEPT_PAUSE_CONTINUE |
			SERVICE_ACCEPT_SHUTDOWN;

	// if a specific exit code is defines, set up
	// the win32 exit code properly
	if (dwServiceSpecificExitCode == 0)
		serviceStatus.dwWin32ExitCode =
			dwWin32ExitCode;
	else
		serviceStatus.dwWin32ExitCode = 
			ERROR_SERVICE_SPECIFIC_ERROR;
	serviceStatus.dwServiceSpecificExitCode =
		dwServiceSpecificExitCode;

	serviceStatus.dwCheckPoint = dwCheckPoint;
	serviceStatus.dwWaitHint = dwWaitHint;

	// Pass the status record to the SCM
	success = SetServiceStatus (serviceStatusHandle,
		&serviceStatus);
	if (!success)
		StopService();

	return success;
}

// Dispatches events received from the service 
// control manager
VOID ServiceCtrlHandler (DWORD controlCode) 
{
	DWORD  currentState = 0;
	BOOL success;

	switch(controlCode)
	{
		// There is no START option because
		// ServiceMain gets called on a start

		// Stop the service
		case SERVICE_CONTROL_STOP:
			currentState = SERVICE_STOP_PENDING;
			// Tell the SCM what's happening
			success = SendStatusToSCM(
				SERVICE_STOP_PENDING,
				NO_ERROR, 0, 1, 5000);
			// Not much to do if not successful

			// Stop the service
			StopService();
			return;

		// Pause the service
		case SERVICE_CONTROL_PAUSE:
			if (runningService && !pauseService)
			{
				// Tell the SCM what's happening
				success = SendStatusToSCM(
					SERVICE_PAUSE_PENDING,
					NO_ERROR, 0, 1, 1000);
				PauseService();
				currentState = SERVICE_PAUSED;
			}
			break;

		// Resume from a pause
		case SERVICE_CONTROL_CONTINUE:
			if (runningService && pauseService)
			{
				// Tell the SCM what's happening
				success = SendStatusToSCM(
					SERVICE_CONTINUE_PENDING,
					NO_ERROR, 0, 1, 1000);
					ResumeService();
					currentState = SERVICE_RUNNING;
			}
			break;

		// Update current status
		case SERVICE_CONTROL_INTERROGATE:
			// it will fall to bottom and send status
			break;

		// Do nothing in a shutdown. Could do cleanup
		// here but it must be very quick.
		case SERVICE_CONTROL_SHUTDOWN:
			// Do nothing on shutdown
			return;
		default:
 			break;
	}
	SendStatusToSCM(currentState, NO_ERROR,
		0, 0, 0);
}

// Handle an error from ServiceMain by cleaning up
// and telling SCM that the service didn't start.
VOID terminate(DWORD error)
{
	// if terminateEvent has been created, close it.
	if (terminateEvent)
		CloseHandle(terminateEvent);

	// Send a message to the scm to tell about
	// stopage
	if (serviceStatusHandle)
		SendStatusToSCM(SERVICE_STOPPED, error,
			0, 0, 0);

	// If the thread has started kill it off
	if (threadHandle)
		CloseHandle(threadHandle);

	// Do not need to close serviceStatusHandle
}

// ServiceMain is called when the SCM wants to
// start the service. When it returns, the service
// has stopped. It therefore waits on an event
// just before the end of the function, and
// that event gets set when it is time to stop. 
// It also returns on any error because the
// service cannot start if there is an eror.
VOID ServiceMain(DWORD argc, LPTSTR *argv) 
{
	BOOL success;

	// immediately call Registration function
	serviceStatusHandle =
		RegisterServiceCtrlHandler(
			SERVICE_NAME,
			(LPHANDLER_FUNCTION) ServiceCtrlHandler);
	if (!serviceStatusHandle)
	{
		terminate(GetLastError());
		return;
	}

	// Notify SCM of progress
	success = SendStatusToSCM(
		SERVICE_START_PENDING,
		NO_ERROR, 0, 1, 5000);
	if (!success)
	{
		terminate(GetLastError()); 
		return;
	}

	// create the termination event
	terminateEvent = CreateEvent (0, TRUE, FALSE,
		0);
	if (!terminateEvent)
	{
		terminate(GetLastError());
		return;
	}

	// Notify SCM of progress
	success = SendStatusToSCM(
		SERVICE_START_PENDING,
		NO_ERROR, 0, 2, 1000);
	if (!success)
	{
		terminate(GetLastError()); 
		return;
	}

	// Notify SCM of progress
	success = SendStatusToSCM(
		SERVICE_START_PENDING,
		NO_ERROR, 0, 3, 5000);
	if (!success)
	{
		terminate(GetLastError()); 
		return;
	}

	// Start the service itself
	success = InitService();
	if (!success)
	{
		terminate(GetLastError());
		return;
	}

	// The service is now running. 
	// Notify SCM of progress
	success = SendStatusToSCM(
		SERVICE_RUNNING,
		NO_ERROR, 0, 0, 0);
	if (!success)
	{
		terminate(GetLastError()); 
		return;
	}

	// Wait for stop signal, and then terminate
	WaitForSingleObject (terminateEvent, INFINITE);

	terminate(0);
}


static void
register_service(int argc, char *argv[])
{
	SC_HANDLE newService, scm;
	BOOL		started;
	LPCTSTR	dependencies = __TEXT("Tcpip\0winmgmt\0\0");

	sw_debug(SW_LOG_NOTICE, "registering %s\n", argv[0]);

	//
	// open a connection to the SCM
	//
	scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
	if (!scm)
	{
		service_error_handler("In OpenScManager", GetLastError());
	}

	//
	// Install the new service
	//
	newService = CreateService(
						scm,
						SERVICE_NAME,
						SERVICE_NAME,
						SERVICE_ALL_ACCESS,
						SERVICE_WIN32_SHARE_PROCESS,
						SERVICE_AUTO_START,
						SERVICE_ERROR_NORMAL,
						argv[0],
						0,
						0, 
						dependencies, 
						0, 
						0);

	if (!newService)
	{
		service_error_handler("In CreateService", GetLastError());
		exit(0);
	}

	sw_debug(SW_LOG_NOTICE, "registered service\n");
	sw_debug(SW_LOG_NOTICE, "starting service\n");

	started = StartService(newService, 0, NULL);
	if (!started)
	{
		service_error_handler("StartService", GetLastError());
		exit(0);
	}

	//
	// clean up
	//
	CloseServiceHandle(newService);
	CloseServiceHandle(scm);
	sw_debug(SW_LOG_NOTICE, "finished\n");
}


static void
unregister_service(int argc, char *argv[])
{
	SC_HANDLE service, scm;
	BOOL success;
	SERVICE_STATUS status;

	sw_debug(SW_LOG_NOTICE, "starting...\n");

	//
	// Open a connection to the SCM
	//
	scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
	if (!scm)
	{
		service_error_handler("In OpenScManager", GetLastError());
		exit(0);
	}

	// Get the service's handle
	service = OpenService(scm, SERVICE_NAME, SERVICE_ALL_ACCESS | DELETE);
	if (!service)
	{
		service_error_handler("In OpenService", GetLastError());
	}
	
	//
	// Stop the service if necessary
	//
	success = QueryServiceStatus(service, &status);
	if (!success)
	{
		service_error_handler("In QueryServiceStatus", GetLastError());
	}

	if (status.dwCurrentState != SERVICE_STOPPED)
	{
		sw_debug(SW_LOG_NOTICE, "stopping service...\n");
		success = ControlService(service, SERVICE_CONTROL_STOP, &status);
		if (!success)
		{
			service_error_handler("In ControlService", GetLastError());
		}
		Sleep(500);
	}
	
	//
	// Remove the service
	//
	success = DeleteService(service);
	if (success)
	{
		sw_debug(SW_LOG_NOTICE, "service unregistered\n");
	}
	else
	{
		service_error_handler("DeleteService", GetLastError());
	}

	//
	// Clean up
	//
	CloseServiceHandle(service);
	CloseServiceHandle(scm);
}


static DWORD
start_mDNSResponder()
{
	sw_uint16 port	= 5335;

	if (sw_mdns_servant_new(&g_servant, port, NULL, 0) != SW_OKAY)
	{
		return ERROR;
	}

	return S_OK;
}


static DWORD
stop_mDNSResponder()
{
	DWORD ret = S_OK;

	if (sw_mdns_servant_delete(g_servant) != SW_OKAY)
	{
		ret = ERROR;
	}

	return ret;
}


static BOOL WINAPI
console_ctrl_handler(
			DWORD		type)
{
	SetEvent(g_console_event);
	return TRUE;
}


int __cdecl
main(int argc, char ** argv)
{
	int i;

	SERVICE_TABLE_ENTRY serviceTable[] = 
	{ 
		{ SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) ServiceMain},
		{ NULL, NULL }
	};

	BOOL success;

	if (argc == 1)
	{
		//
		// Register with the SCM
		//
		success = StartServiceCtrlDispatcher(serviceTable);
	
		if (!success)
		{
			service_error_handler("StartServiceCtrlDispatcher", GetLastError());
			exit(0);
		}
	}
	else for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-run") == 0)
		{
			g_console_event = CreateEvent(NULL, FALSE, FALSE, NULL);

			if (g_console_event == NULL)
			{
				sw_debug(SW_LOG_ERROR, "CreateEvent failed: %d\n", GetLastError());
				exit(-1);
			}

			SetConsoleCtrlHandler(console_ctrl_handler, TRUE);

			if (start_mDNSResponder() != S_OK)
			{
				exit(-1);
			}

			WaitForSingleObject(g_console_event, INFINITE);

			CloseHandle(g_console_event);

			stop_mDNSResponder();

			exit(0);
		}
		else if (strcmp(argv[i], "-install") == 0)
		{
			register_service(argc, argv);
			break;
		}
		else if (strcmp(argv[i], "-remove") == 0)
		{
			unregister_service(argc, argv);
			break;
		}
		else if (strcmp(argv[i], "-debug") == 0)
		{
			// sw_debug_enable(1);
		}
		else
		{
			fprintf(stderr, "usage: %s [-run][-install][-remove]\n", argv[0]);
			break;
		}
	}

	return 0;
}
