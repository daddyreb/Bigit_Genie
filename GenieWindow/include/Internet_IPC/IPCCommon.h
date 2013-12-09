#ifndef IPCCOMMON_H
#define IPCCOMMON_H

#define PACKAGE_CONSUMER_SEMAPHORE \
"PACKAGE_CONSUMER_SEMAPHORE_{373C5780-0B21-4b71-BFB6-AE540C370BAD}"
#define PACKAGE_PRODUCER_SEMAPHORE \
        "PACKAGE_PRODUCER_SEMAPHORE_{B36520D7-E3D4-4b23-8BC3-9FC10150C312}"

#define PACKAGE_SHARED_MEMORY_KEY \
        "PACKAGE_SHARED_MEMORY_KEY_{48E25A46-233F-4302-8A21-F75E689D316F}"
#define PACKAGE_SHARED_MEMORY_SIZE 1024

#define MESSAGE_FLAG_INVOKE         1
#define MESSAGE_FLAG_RETURN         0
#define MESSAGE_SENDER_INVALID      -1

        typedef struct Tag_IPC_MESSAGE
{
    int     msg_sender;//may be something like a process id
    int     msg_code;//function code
    int     msg_flag;//flags
    int     msg_size;//message size in bytes
    int     msg_context_size;//context size in bytes
    char    msg_context[1];//message context

}IPC_MESSAGE;

//        typedef struct Tag_IPC_Package
//        {
//            int total_size;
//            int size;
//            int sequence;
//            int package_code;
//            int fragment;
//
//        }IPC_Package;

#endif // IPCCOMMON_H
