#pragma once
#include "libraries.h"
#include "constants.h"
#include "crypt.h"
#include "open_file_info.h"



typedef struct _DEVICE_EXTENSION {
    BOOLEAN                     media_in_device;
    UNICODE_STRING              device_name;
    ULONG                       device_number;
    DEVICE_TYPE                 device_type;
    HANDLE                      file_handle;
	UNICODE_STRING              file_name;
    LARGE_INTEGER               file_size;
	ANSI_STRING					password;
    LIST_ENTRY                  list_head;
    KSPIN_LOCK                  list_lock;
    KEVENT                      request_event;
    PVOID                       thread_pointer;
    BOOLEAN                     terminate_thread;
	enum Crypt					crypt_mode;
	WCHAR						letter;
	

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

