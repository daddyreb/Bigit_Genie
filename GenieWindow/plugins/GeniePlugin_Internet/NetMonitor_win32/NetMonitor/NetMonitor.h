#ifndef __NetMonitor_h__
#define __NetMonitor_h__

#define NM_CALL_CONV __stdcall

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum
{
	NM_OK = 0,
	NM_ERR_UNKNOWN = -1,
	NM_ERR_NULL_PTR = -2,
	NM_ERR_NOT_ENOUGH_BUFFER = -3,
} nmStatus;

typedef struct nmInstance nmInstance;

typedef struct
{
	void (NM_CALL_CONV *Destroy)(nmInstance *pthis);
	void (NM_CALL_CONV *Start)(nmInstance *pthis);
	void (NM_CALL_CONV *Stop)(nmInstance *pthis);
	nmStatus (NM_CALL_CONV *Snapshot)(nmInstance *pthis, void *buffer, int *bufferSize);
	nmStatus (NM_CALL_CONV *GetAppIcon)(nmInstance *pthis, int pid, void **handle);
	nmStatus (NM_CALL_CONV *ReleaseAppIcon)(nmInstance *pthis, void *handle);
	nmStatus (NM_CALL_CONV *TestLoop)(nmInstance *pthis);
} nmFuncTable;

struct nmInstance
{
	nmFuncTable *vtbl;
};

nmStatus NM_CALL_CONV NetMonitorCreate(nmInstance **ppInstance);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // __NetMonitor_h__
