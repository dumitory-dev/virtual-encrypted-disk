#pragma once
#include "libraries.h"
ULONG g_uCountDevice = 1;
HANDLE g_pDirHandle = NULL;
PDEVICE_OBJECT g_pDeviceObject = NULL;
PDRIVER_OBJECT g_pDriverObject = NULL;

UNICODE_STRING g_usDeviceName = RTL_CONSTANT_STRING(DEVICE_MAIN_DEVICE_NAME);
UNICODE_STRING g_usSymLinkName = RTL_CONSTANT_STRING(MAIN_DEVICE_SYM_LINK);

