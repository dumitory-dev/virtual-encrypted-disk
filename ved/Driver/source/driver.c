#include "../headers/headers.h"
#include "../headers/crypt.h"

/*
INFO BUILD:
OS VERSION: Windows 7
SDK VERSION: 10.0.18362.0
VISUAl STUDIO 2019 COMMUNITY
*/

NTSTATUS CreateDevice(struct _DRIVER_OBJECT* pDriverObject, ULONG uNumber, DEVICE_TYPE DeviceType);
PDEVICE_OBJECT DeleteDevice(IN PDEVICE_OBJECT pDeviceObject);

NTSTATUS CreateFile(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp, BOOLEAN bIsOpen);
NTSTATUS CloseFile(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);

DRIVER_UNLOAD Unload;
DRIVER_DISPATCH CreateAndCloseDevice;
DRIVER_DISPATCH ReadAndWriteDevice;
DRIVER_DISPATCH ControlDevice;
KSTART_ROUTINE Thread;

#pragma code_seg("INIT")
NTSTATUS DriverEntry(struct _DRIVER_OBJECT* DriverObject, UNICODE_STRING* pRegPath)
{
	UNREFERENCED_PARAMETER(pRegPath);

	UNICODE_STRING        unDeviceDirName;
	OBJECT_ATTRIBUTES     ObjectAttributes;

	RtlInitUnicodeString(&unDeviceDirName, DEVICE_DIR_NAME);

#ifdef  DBG
	DbgBreakPoint();
#endif


	InitializeObjectAttributes(
		&ObjectAttributes,
		&unDeviceDirName,
		OBJ_PERMANENT,
		NULL,
		NULL
	);

	NTSTATUS status = ZwCreateDirectoryObject(
		&g_pDirHandle,
		DIRECTORY_ALL_ACCESS,
		&ObjectAttributes);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	status = IoCreateDevice(
		DriverObject,
		sizeof(DEVICE_EXTENSION),
		&g_usDeviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&g_pDeviceObject
	);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("VED: Failed create device!\n\r");
		return status;
	}

	((PDEVICE_EXTENSION)g_pDeviceObject->DeviceExtension)->media_in_device = TRUE;
	((PDEVICE_EXTENSION)g_pDeviceObject->DeviceExtension)->device_type = MAIN_DEVICE_TYPE;

	IoDeleteSymbolicLink(&g_usSymLinkName);
	status = IoCreateSymbolicLink(&g_usSymLinkName, &g_usDeviceName);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("VED: Failed creating Symbolic Link device!");
		IoDeleteDevice(g_pDeviceObject);
		return status;
	}


	/*
	 * A temporary object has a name only as long as its handle count is greater than zero.
	 * When the handle count reaches zero, the system deletes the object name and appropriately adjusts the object's pointer count.
	 */
	ZwMakeTemporaryObject(g_pDirHandle);


	DriverObject->DriverUnload = Unload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateAndCloseDevice;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateAndCloseDevice;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = ReadAndWriteDevice;
	DriverObject->MajorFunction[IRP_MJ_READ] = ReadAndWriteDevice;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ControlDevice;
	g_pDriverObject = DriverObject;

	/*for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = StubFunc;
	}*/

	DbgPrint("VED: Success driver installation!\r\n");
	return status;
}
#pragma code_seg() // end INIT section


#pragma code_seg("PAGE")
_Use_decl_annotations_
NTSTATUS ControlDevice(struct _DEVICE_OBJECT* pDeviceObject, struct _IRP* pIrp)
{

	PDEVICE_EXTENSION  pDeviceExtension = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;
	PIO_STACK_LOCATION IoStack = IoGetCurrentIrpStackLocation(pIrp);
	NTSTATUS status = STATUS_SUCCESS;

	/*if (!pDeviceExtension->media_in_device
		&&
		IoStack->Parameters.DeviceIoControl.IoControlCode != IOCTL_FILE_DISK_OPEN_FILE
		)
	{
		pIrp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
		pIrp->IoStatus.Information = 0;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_NO_MEDIA_IN_DEVICE;
	}*/


	switch (IoStack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_GET_MOUNT_DEVICES:
	{
#ifdef DBG
			DbgBreakPoint();
#endif

		if (pDeviceObject != g_pDeviceObject)
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			pIrp->IoStatus.Information = 0;
			break;
		}

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(OPEN_FILE_INFORMATION))
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			pIrp->IoStatus.Information = 0;
			break;
		}


		POPEN_FILE_INFORMATION  data_buf = (POPEN_FILE_INFORMATION)pIrp->UserBuffer;

		if (data_buf == NULL)
		{
			status = STATUS_INVALID_PARAMETER;
			pIrp->IoStatus.Information = 0;
			break;
		}

		int index = 0;

		if (g_pDriverObject->DeviceObject->NextDevice != NULL)
		{

			PDEVICE_OBJECT pDeviceObject_ = g_pDriverObject->DeviceObject;


			while (pDeviceObject_ != NULL)
			{

				PDEVICE_EXTENSION tmp = (PDEVICE_EXTENSION)pDeviceObject_->DeviceExtension;
				if (tmp == NULL)
				{
					break;
				}

				if (tmp->media_in_device && pDeviceObject_ != g_pDeviceObject)
				{

					RtlCopyMemory(data_buf[index].FileName, tmp->file_name.Buffer, tmp->file_name.Length);

					data_buf[index].FileNameLength = tmp->file_name.Length;
					data_buf[index].FileSize = tmp->file_size;
					data_buf[index].DriveLetter = tmp->letter;
					data_buf[index].CryptMode = tmp->crypt_mode;
					++index;

				}

				pDeviceObject_ = pDeviceObject_->NextDevice;


			};

		}

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = index;
		break;


	}

	case IOCTL_FILE_ADD_DEVICE:
	{
		PAGED_CODE();
#ifdef DBG
		DbgBreakPoint();
#endif

		if (g_uCountDevice == 25)
		{
			status = STATUS_THREAD_NOT_RUNNING;
			pIrp->IoStatus.Information = 0;
			break;
		}
			
			
		status = CreateDevice(
			g_pDriverObject,
			g_uCountDevice,
			FILE_DEVICE_DISK);

		if (!NT_SUCCESS(status))
		{
			status = STATUS_INVALID_THREAD;
			pIrp->IoStatus.Information = 0;
			break;
		}
		pIrp->IoStatus.Information = g_uCountDevice;
		g_uCountDevice++;
		DbgPrint("VED: Device is load!\n\r");
		break;
	}
	case IOCTL_GET_FREE_DEVICE:
	{
		PAGED_CODE();

#ifdef DBG
		DbgBreakPoint();
#endif
		if (g_pDriverObject->DeviceObject->NextDevice != NULL)
		{

			PDEVICE_OBJECT pDeviceObject_ = g_pDriverObject->DeviceObject;


			while (pDeviceObject_ != NULL)
			{

				PDEVICE_EXTENSION tmp = (PDEVICE_EXTENSION)pDeviceObject_->DeviceExtension;
				if (tmp == NULL)
				{
					break;
				}

				if (tmp->media_in_device)
				{
					pDeviceObject_ = pDeviceObject_->NextDevice;
					continue;
				}

				pIrp->IoStatus.Status = STATUS_SUCCESS;
				pIrp->IoStatus.Information = tmp->device_number;
				IoCompleteRequest(pIrp, IO_NO_INCREMENT);
				return pIrp->IoStatus.Status;


			};

		}

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = 0;
		break;

	}
	case IOCTL_FILE_DISK_OPEN_FILE:
	{
#ifdef  DBG
		DbgBreakPoint();
#endif

		if (pDeviceObject == g_pDeviceObject)
		{

			status = CreateFile(g_pDeviceObject, pIrp, FALSE);
			if (status != STATUS_SUCCESS || pIrp->IoStatus.Information != 0)
			{
				break;
			}

			status = CloseFile(g_pDeviceObject, pIrp);
			if (pIrp->IoStatus.Status != STATUS_SUCCESS)
			{
				break;
			}

			status = STATUS_SUCCESS;
			break;
		}

		if (pDeviceExtension->media_in_device)
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			pIrp->IoStatus.Information = 0;
			break;
		}

		if (IoStack->Parameters.DeviceIoControl.InputBufferLength
			<
			sizeof(OPEN_FILE_INFORMATION))
		{
			status = STATUS_INVALID_PARAMETER;
			pIrp->IoStatus.Information = 0;
			break;
		}

		IoMarkIrpPending(pIrp);

		ExInterlockedInsertTailList(
			&pDeviceExtension->list_head,
			&pIrp->Tail.Overlay.ListEntry,
			&pDeviceExtension->list_lock
		);

		KeSetEvent(
			&pDeviceExtension->request_event,
			(KPRIORITY)0,
			FALSE
		);

		status = STATUS_PENDING;
		break;

	}
	case IOCTL_FILE_DISK_CLOSE_FILE:
	{
		IoMarkIrpPending(pIrp);
		ExInterlockedInsertTailList(
			&pDeviceExtension->list_head,
			&pIrp->Tail.Overlay.ListEntry,
			&pDeviceExtension->list_lock
		);

		KeSetEvent(
			&pDeviceExtension->request_event,
			(KPRIORITY)0,
			FALSE
		);

		status = STATUS_PENDING;

		break;

	}
	case IOCTL_DISK_CHECK_VERIFY:
	case IOCTL_STORAGE_CHECK_VERIFY:
	case IOCTL_STORAGE_CHECK_VERIFY2:
	{
		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = 0;
		break;
	}

	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
	{

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(DISK_GEOMETRY))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PDISK_GEOMETRY pDiskGeometry = (PDISK_GEOMETRY)pIrp->AssociatedIrp.SystemBuffer;

		const ULONGLONG uLength = pDeviceExtension->file_size.QuadPart;

		const  ULONG uSectorSize = 2048;

		pDiskGeometry->Cylinders.QuadPart = uLength / uSectorSize / 32 / 2;
		pDiskGeometry->MediaType = FixedMedia;
		pDiskGeometry->TracksPerCylinder = 2;
		pDiskGeometry->SectorsPerTrack = 32;
		pDiskGeometry->BytesPerSector = uSectorSize;

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = sizeof(DISK_GEOMETRY);
		break;
	}


	case IOCTL_DISK_GET_LENGTH_INFO:
	{
		//Retrieves information about the type, size, and nature of a disk partition.

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(GET_LENGTH_INFORMATION))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PGET_LENGTH_INFORMATION pGetLengthInformation = (PGET_LENGTH_INFORMATION)pIrp->AssociatedIrp.SystemBuffer;
		pGetLengthInformation->Length.QuadPart = pDeviceExtension->file_size.QuadPart;

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);

		break;
	}

	case IOCTL_DISK_GET_PARTITION_INFO:
	{

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(PARTITION_INFORMATION))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PPARTITION_INFORMATION pPartitionInformation = (PPARTITION_INFORMATION)pIrp->AssociatedIrp.SystemBuffer;

		const ULONGLONG uLength = pDeviceExtension->file_size.QuadPart;

		pPartitionInformation->StartingOffset.QuadPart = 0;
		pPartitionInformation->PartitionLength.QuadPart = uLength;
		pPartitionInformation->HiddenSectors = 1;
		pPartitionInformation->PartitionNumber = 0;
		pPartitionInformation->PartitionType = 0;
		pPartitionInformation->BootIndicator = FALSE;
		pPartitionInformation->RecognizedPartition = FALSE;
		pPartitionInformation->RewritePartition = FALSE;

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = sizeof(PARTITION_INFORMATION);

		break;
	}

	case IOCTL_DISK_GET_PARTITION_INFO_EX:
	{
		//Retrieves extended information about the type, size, and nature of a disk partition.

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(PARTITION_INFORMATION_EX))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PPARTITION_INFORMATION_EX pPartitionInformationEx = (PPARTITION_INFORMATION_EX)pIrp->AssociatedIrp.SystemBuffer;

		const ULONGLONG  uLength = pDeviceExtension->file_size.QuadPart;

		pPartitionInformationEx->PartitionStyle = PARTITION_STYLE_MBR;
		pPartitionInformationEx->StartingOffset.QuadPart = 0;
		pPartitionInformationEx->PartitionLength.QuadPart = uLength;
		pPartitionInformationEx->PartitionNumber = 0;
		pPartitionInformationEx->RewritePartition = FALSE;
		pPartitionInformationEx->Mbr.PartitionType = 0;//Contains partition information specific to master boot record (MBR) disks.
		pPartitionInformationEx->Mbr.BootIndicator = FALSE;
		pPartitionInformationEx->Mbr.RecognizedPartition = FALSE;
		pPartitionInformationEx->Mbr.HiddenSectors = 1;

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = sizeof(PARTITION_INFORMATION_EX);

		break;
	}

	case IOCTL_DISK_IS_WRITABLE:
	{
		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = 0;
		break;
	}
	case IOCTL_DISK_MEDIA_REMOVAL:
	case IOCTL_STORAGE_MEDIA_REMOVAL:
	{
		//Enables or disables the mechanism that ejects media, for those devices possessing that locking capability.
		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = 0;
		break;
	}

	case IOCTL_DISK_SET_PARTITION_INFO:
	{
		if (IoStack->Parameters.DeviceIoControl.InputBufferLength <
			sizeof(SET_PARTITION_INFORMATION))
		{
			status = STATUS_INVALID_PARAMETER;
			pIrp->IoStatus.Information = 0;
			break;
		}

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = 0;

		break;
	}

	case IOCTL_DISK_VERIFY:
	{
		//Verifies the specified extent on a fixed disk.
		if (IoStack->Parameters.DeviceIoControl.InputBufferLength <
			sizeof(VERIFY_INFORMATION))
		{
			status = STATUS_INVALID_PARAMETER;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PVERIFY_INFORMATION pVerifyInformation = (PVERIFY_INFORMATION)pIrp->AssociatedIrp.SystemBuffer;

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = pVerifyInformation->Length;

		break;
	}


	case IOCTL_STORAGE_GET_DEVICE_NUMBER:
	{

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(STORAGE_DEVICE_NUMBER))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PSTORAGE_DEVICE_NUMBER pDeviceNumber = (PSTORAGE_DEVICE_NUMBER)pIrp->AssociatedIrp.SystemBuffer;

		pDeviceNumber->DeviceType = pDeviceExtension->device_type;
		pDeviceNumber->DeviceNumber = pDeviceExtension->device_number;
		pDeviceNumber->PartitionNumber = (ULONG)-1;//device can not be partioned

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = sizeof(STORAGE_DEVICE_NUMBER);

		break;
	}

	case IOCTL_STORAGE_GET_HOTPLUG_INFO:
	{

		//Retrieves the hotplug configuration of the specified device.

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(STORAGE_HOTPLUG_INFO))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PSTORAGE_HOTPLUG_INFO pHotPlugInfo = (PSTORAGE_HOTPLUG_INFO)pIrp->AssociatedIrp.SystemBuffer;

		pHotPlugInfo->Size = sizeof(STORAGE_HOTPLUG_INFO);
		pHotPlugInfo->MediaRemovable = 0;
		pHotPlugInfo->MediaHotplug = 0;
		pHotPlugInfo->DeviceHotplug = 0;
		pHotPlugInfo->WriteCacheEnableOverride = 0;

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = sizeof(STORAGE_HOTPLUG_INFO);

		break;
	}


	case IOCTL_VOLUME_GET_GPT_ATTRIBUTES:
	{
		//Retrieves the attributes for a volume.
		//GUID Partition table


		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION  pAttributesInfo = (PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION)pIrp->AssociatedIrp.SystemBuffer;

		pAttributesInfo->GptAttributes = 0;//means nothing , non hidden....

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION);

		break;
	}


	case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
	{
		//Retrieves the physical location of a specified volume on one or more disks.

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(VOLUME_DISK_EXTENTS))
		{
			status = STATUS_INVALID_PARAMETER;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PVOLUME_DISK_EXTENTS pVolumeDiskExtents = (PVOLUME_DISK_EXTENTS)pIrp->AssociatedIrp.SystemBuffer;

		pVolumeDiskExtents->NumberOfDiskExtents = 1;
		pVolumeDiskExtents->Extents[0].DiskNumber = pDeviceExtension->device_number;
		pVolumeDiskExtents->Extents[0].StartingOffset.QuadPart = 0;
		pVolumeDiskExtents->Extents[0].ExtentLength.QuadPart = pDeviceExtension->file_size.QuadPart;

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = sizeof(VOLUME_DISK_EXTENTS);

		break;
	}

	case IOCTL_DISK_IS_CLUSTERED:
	{
		//Allows a driver or application to determine if a disk is clustered.
		//Just answer no

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(BOOLEAN))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PBOOLEAN pCluster = (PBOOLEAN)pIrp->AssociatedIrp.SystemBuffer;

		*pCluster = FALSE;

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = sizeof(BOOLEAN);

		break;
	}

	case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
	{
		//!!!Support for this IOCTL by the mount manager clients is mandatory.!!!
		//Upon receiving this IOCTL a client driver must provide the (nonpersistent) device (or target) name for the volume.

		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(MOUNTDEV_NAME))
		{
			status = STATUS_INVALID_PARAMETER;
			pIrp->IoStatus.Information = 0;
			break;
		}

		PMOUNTDEV_NAME pMountedName = (PMOUNTDEV_NAME)pIrp->AssociatedIrp.SystemBuffer;
		pMountedName->NameLength = pDeviceExtension->device_name.Length * sizeof(WCHAR);


		if (IoStack->Parameters.DeviceIoControl.OutputBufferLength <
			pMountedName->NameLength + sizeof(USHORT))
		{
			status = STATUS_BUFFER_OVERFLOW;
			pIrp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
			break;
		}

		RtlCopyMemory(pMountedName->Name, pDeviceExtension->device_name.Buffer, pMountedName->NameLength);

		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = pMountedName->NameLength + sizeof(USHORT);

		break;
	}

	default:
	{
		/*DbgPrint(
			"VED: Unknown IoControlCode %#x\n",
			IoStack->Parameters.DeviceIoControl.IoControlCode
		);*/

		status = STATUS_INVALID_DEVICE_REQUEST;
		pIrp->IoStatus.Information = 0;
	}

	}

	if (status != STATUS_PENDING)
	{
		pIrp->IoStatus.Status = status;

		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	}

	return status;

}
#pragma code_seg() // end PAGE section
/*
   The DRIVER_UNLOAD function type is defined in the Wdm.h header file.
   To more accurately identify errors when you run the code analysis tools, be sure to add the _Use_decl_annotations_ annotation to your function definition.
   The _Use_decl_annotations_ annotation ensures that the annotations that are applied to the DRIVER_UNLOAD function type in the header file are used.
   For more information about the requirements for function declarations, see Declaring Functions by Using Function Role Types for WDM Drivers.
   For information about _Use_decl_annotations_, see Annotating Function Behavior.
*/

#pragma code_seg("PAGE")
_Use_decl_annotations_
VOID Unload(IN PDRIVER_OBJECT pDriverObject)
{

	PAGED_CODE();

	IoDeleteDevice(g_pDeviceObject);

	PDEVICE_OBJECT pDeviceObject = pDriverObject->DeviceObject;

#ifdef  DBG
	DbgBreakPoint();
#endif
	
	if (pDeviceObject != NULL)
	{
		while (pDeviceObject)
		{
			pDeviceObject = DeleteDevice(pDeviceObject);
		}
	}

	ZwClose(g_pDirHandle);

#ifdef DBG
	DbgPrint("Unload success!\r\n");
#endif
}
#pragma code_seg() // end PAGE section

#pragma code_seg("PAGE")
PDEVICE_OBJECT DeleteDevice(IN PDEVICE_OBJECT pDeviceObject)
{
	PAGED_CODE();
	ASSERT(pDeviceObject != NULL);

#ifdef  DBG
	DbgBreakPoint();
#endif

	
	PDEVICE_EXTENSION pDeviceExtension = pDeviceObject->DeviceExtension;
	pDeviceExtension->terminate_thread = TRUE;

	KeSetEvent(
		&pDeviceExtension->request_event,
		(KPRIORITY)0,
		FALSE
	);

	KeWaitForSingleObject(
		pDeviceExtension->thread_pointer,
		Executive,
		KernelMode,
		FALSE,
		NULL
	);

	ObDereferenceObject(pDeviceExtension->thread_pointer);
	if (pDeviceExtension->device_name.Buffer != NULL)
	{
		ExFreePool(pDeviceExtension->device_name.Buffer);
	}
			
	
	/*
	 * A pointer to the next device object, if any, that was created by the same driver.
	 * The I/O manager updates this list at each successful call to IoCreateDevice or IoCreateDeviceSecure.
	 */
	PDEVICE_OBJECT pNextDevice = pDeviceObject->NextDevice;

	IoDeleteDevice(pDeviceObject);

	return pNextDevice;

}
#pragma code_seg() // end PAGE section

#pragma code_seg("PAGE")
_Use_decl_annotations_
NTSTATUS CreateAndCloseDevice(struct _DEVICE_OBJECT* pDeviceObject, struct _IRP* pIrp)
{
	PAGED_CODE();
	UNREFERENCED_PARAMETER(pDeviceObject);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = FILE_OPENED;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
#pragma code_seg() // end PAGE section

#pragma code_seg("PAGE")
_Use_decl_annotations_
NTSTATUS ReadAndWriteDevice(struct _DEVICE_OBJECT* pDeviceObject, struct _IRP* pIrp)
{
	PDEVICE_EXTENSION pDeviceExtension = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;
	if (!pDeviceExtension->media_in_device)
	{
		pIrp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
		pIrp->IoStatus.Information = 0;

		IoCompleteRequest(pIrp, IO_NO_INCREMENT);

		return STATUS_NO_MEDIA_IN_DEVICE;

	}

	PIO_STACK_LOCATION IoStack = IoGetCurrentIrpStackLocation(pIrp);
	if (IoStack->Parameters.Read.Length == 0)
	{
		pIrp->IoStatus.Status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = 0;

		IoCompleteRequest(pIrp, IO_NO_INCREMENT);

		return STATUS_SUCCESS;
	}


	IoMarkIrpPending(pIrp);

	ExInterlockedInsertTailList(
		&pDeviceExtension->list_head,
		&pIrp->Tail.Overlay.ListEntry,
		&pDeviceExtension->list_lock
	);


	KeSetEvent(
		&pDeviceExtension->request_event,
		(KPRIORITY)0,
		FALSE
	);

	return STATUS_PENDING;

}
#pragma code_seg() // end PAGE section

#pragma code_seg("PAGE")
NTSTATUS CreateDevice(struct _DRIVER_OBJECT* pDriverObject, ULONG uNumber, DEVICE_TYPE DeviceType)
{

	ASSERT(pDriverObject != NULL);
	UNICODE_STRING     usDeviceName;

	usDeviceName.Buffer = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, MAXIMUM_FILENAME_LENGTH * 2, FILE_DISK_POOL_TAG);
	if (usDeviceName.Buffer == NULL)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	usDeviceName.Length = 0;
	usDeviceName.MaximumLength = MAXIMUM_FILENAME_LENGTH * 2;
	RtlUnicodeStringPrintf(&usDeviceName, DEVICE_NAME_PREFIX L"%u", uNumber);

	UNICODE_STRING    usSSDDL;
	//“D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;BU)(A;;GA;;;WD)”
	RtlInitUnicodeString(&usSSDDL, L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;BU)");

	//DbgBreakPoint();

	//Creates a named device
	PDEVICE_OBJECT      pDeviceObject = NULL;
	NTSTATUS status = IoCreateDeviceSecure(
		pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&usDeviceName,
		DeviceType,
		0,
		FALSE,
		&usSSDDL,
		NULL,
		&pDeviceObject
	);

	if (pDeviceObject == NULL)
	{
		ExFreePool(usDeviceName.Buffer);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	if (!NT_SUCCESS(status))
	{
		ExFreePool(usDeviceName.Buffer);

		return status;
	}

	/*if (g_uCountDevice == 0)
	{
		IoDeleteSymbolicLink(&g_usSymLinkName);

		status = IoCreateSymbolicLink(
			&g_usSymLinkName,
			&usDeviceName
		);

		if (!NT_SUCCESS(status))
		{
			DbgPrint("Error create symbol link!\n\r");
			return status;
		}
		DbgPrint("VED: Create symbolic link success!\n\r");
	}*/


	//The operating system locks the application's buffer in memory.
	//It then creates a memory descriptor list (MDL) that identifies the locked memory pages, and passes the MDL to the driver stack.
	//Drivers access the locked pages through the MDL.
	pDeviceObject->Flags |= DO_DIRECT_IO;

	/*
	 * For most intermediate and lowest-level drivers, the device extension is the most important data structure associated with a device object. Its internal structure is driver-defined, and it is typically used to:

		Maintain device state information.
		Provide storage for any kernel-defined objects or other system resources, such as spin locks, used by the driver.
		Hold any data the driver must have resident and in system space to carry out its I/O operations.
	 */
	PDEVICE_EXTENSION pDeviceExtension = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;
	pDeviceExtension->media_in_device = FALSE;
	pDeviceExtension->device_name.Length = usDeviceName.Length;
	pDeviceExtension->device_name.MaximumLength = usDeviceName.MaximumLength;
	pDeviceExtension->device_name.Buffer = usDeviceName.Buffer;
	pDeviceExtension->device_number = uNumber;
	pDeviceExtension->device_type = DeviceType;
	pDeviceExtension->crypt_mode = RC4;



	InitializeListHead(&pDeviceExtension->list_head);
	KeInitializeSpinLock(&pDeviceExtension->list_lock);

	KeInitializeEvent(
		&pDeviceExtension->request_event,
		SynchronizationEvent,
		FALSE);

	pDeviceExtension->terminate_thread = FALSE;

	HANDLE hThread = NULL;
	status = PsCreateSystemThread(
		&hThread,
		(ACCESS_MASK)0L,
		NULL,
		NULL,
		NULL,
		Thread,
		pDeviceObject
	);

	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDeviceObject);
		ExFreePool(usDeviceName.Buffer);
		return status;
	}

	/*The ObReferenceObjectByHandle routine provides access validation on the object handle, and, if access can be granted, returns the corresponding pointer to the object's body.*/

	status = ObReferenceObjectByHandle(
		hThread,
		THREAD_ALL_ACCESS,
		NULL,
		KernelMode,
		&pDeviceExtension->thread_pointer,
		NULL
	);

	if (!NT_SUCCESS(status))
	{
		ZwClose(hThread);
		pDeviceExtension->terminate_thread = TRUE;
		KeSetEvent(
			&pDeviceExtension->request_event,
			(KPRIORITY)0,
			FALSE
		);
		IoDeleteDevice(pDeviceObject);
		ExFreePool(usDeviceName.Buffer);
		return status;

	}

	ZwClose(hThread);

	//ExFreePool(usDeviceName.Buffer)
	return status;

}
#pragma code_seg() // end PAGE section

#pragma code_seg("PAGE")
VOID Thread(IN PVOID pContext)
{

	//PUCHAR              uSystemBuffer;
	//PUCHAR              uBuffer;
	//ULONGLONG			i = 0, j = 0;

	//The PAGED_CODE macro ensures that the calling thread is running at an IRQL that is low enough to permit paging.
	PAGED_CODE();

	ASSERT(pContext != NULL);

	PDEVICE_OBJECT pDeviceObject = (PDEVICE_OBJECT)pContext;
	PDEVICE_EXTENSION pDeviceExtension = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;
	KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

	PLIST_ENTRY         pRequest;
	PUCHAR pSystemBuffer = NULL;
	PUCHAR pBuffer = NULL;

	PIRP pIrp = NULL;
	//NTSTATUS status = 0;
	ULONG uCountBlock = 0;
	//ULONGLONG i = 0, j = 0;

	while (TRUE)
	{

		KeWaitForSingleObject(
			&pDeviceExtension->request_event,
			Executive,
			KernelMode,
			FALSE,
			NULL
		);

		if (pDeviceExtension->terminate_thread)
		{
			PsTerminateSystemThread(STATUS_SUCCESS);
		}

		while ((pRequest = ExInterlockedRemoveHeadList(
			&pDeviceExtension->list_head,
			&pDeviceExtension->list_lock))
			!= NULL)
		{
			/*The CONTAINING_RECORD macro returns the base address of an instance of a structure
			 *given the type of the structure and the address of a field within the containing structure.*/
			pIrp = CONTAINING_RECORD(pRequest, IRP, Tail.Overlay.ListEntry);
			PIO_STACK_LOCATION IoStack = IoGetCurrentIrpStackLocation(pIrp);


			switch (IoStack->MajorFunction)
			{
			case IRP_MJ_READ:

				uCountBlock = 0;
				/*j = IoStack->Parameters.Read.ByteOffset.QuadPart % pDeviceExtension->password.Length;
				if (j != 0 || IoStack->Parameters.Read.Length % 2048  != 0 || IoStack->Parameters.Read.ByteOffset.QuadPart % 2048 != 0)
				{
					DbgBreakPoint();
				}*/
				pSystemBuffer = (PUCHAR)MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				if (pSystemBuffer == NULL)
				{
					pIrp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					pIrp->IoStatus.Information = 0;
					break;
				}
				pBuffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, IoStack->Parameters.Read.Length, FILE_DISK_POOL_TAG);
				if (pBuffer == NULL)
				{
					pIrp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					pIrp->IoStatus.Information = 0;
					break;
				}
				IoStack->Parameters.Read.ByteOffset.QuadPart += PASSWORD_OFFSET;

				ZwReadFile(
					pDeviceExtension->file_handle,
					NULL,
					NULL,
					NULL,
					&pIrp->IoStatus,
					pBuffer,
					IoStack->Parameters.Read.Length,
					&IoStack->Parameters.Read.ByteOffset,
					NULL
				);
				/*for (i=0, j; i < IoStack->Parameters.Read.Length; ++i, j++)
				{
					j == pDeviceExtension->password.Length ? j = 0 : j;
					pBuffer[i] ^= pDeviceExtension->password.Buffer[j];
				}*/


				uCountBlock = IoStack->Parameters.Read.Length / SECTOR_SIZE;
				ULONG uMemoryOffset = 0;
				for (ULONG index = 0; index < uCountBlock; ++index)
				{
					//Rc4Crypt(pDeviceExtension->password.Buffer,pDeviceExtension->password.Length,(PCHAR)(pBuffer + uMemoryOffset), SECTOR_SIZE);
					//EncryptDecrypt(pDeviceExtension->password.Buffer,pDeviceExtension->password.Length,(pBuffer + uMemoryOffset), SECTOR_SIZE,FALSE);


					if (pDeviceExtension->crypt_mode == RC4)
					{
						Rc4Crypt(pDeviceExtension->password.Buffer, pDeviceExtension->password.Length, (PCHAR)(pBuffer + uMemoryOffset), SECTOR_SIZE);
					}
					else
					{
						DecryptEncrypt((UCHAR*)pDeviceExtension->password.Buffer, pDeviceExtension->password.Length, pBuffer + uMemoryOffset, SECTOR_SIZE, FALSE);
					}

					uMemoryOffset += SECTOR_SIZE;

				}


				//Rc4Crypt(pDeviceExtension->password.Buffer,pDeviceExtension->password.Length,(PCHAR)pBuffer, IoStack->Parameters.Read.Length);

				RtlCopyMemory(pSystemBuffer, pBuffer, IoStack->Parameters.Read.Length);
				ExFreePool(pBuffer);
				break;

			case IRP_MJ_WRITE:
				uCountBlock = 0;
				/*j = IoStack->Parameters.Write.ByteOffset.QuadPart % pDeviceExtension->password.Length;
				if ( IoStack->Parameters.Write.ByteOffset.QuadPart % 2048 == 0)
				{
					DbgBreakPoint();
				}*/
				if ((IoStack->Parameters.Write.ByteOffset.QuadPart +
					IoStack->Parameters.Write.Length) >
					pDeviceExtension->file_size.QuadPart)
				{
					pIrp->IoStatus.Status = STATUS_INVALID_PARAMETER;
					pIrp->IoStatus.Information = 0;
					break;
				}
				pSystemBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				pBuffer = (PUCHAR)ExAllocatePoolWithTag(PagedPool, IoStack->Parameters.Write.Length, FILE_DISK_POOL_TAG);
				if (pBuffer == NULL || pSystemBuffer == NULL)
				{
					pIrp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					pIrp->IoStatus.Information = 0;
					break;
				}

				RtlCopyMemory(pBuffer, pSystemBuffer, IoStack->Parameters.Write.Length);

				uCountBlock = IoStack->Parameters.Write.Length / SECTOR_SIZE;
				ULONG uMemoryOffsetWrite = 0;
				for (ULONG indexWrite = 0; indexWrite < uCountBlock; ++indexWrite)
				{
					//Rc4Crypt(pDeviceExtension->password.Buffer,pDeviceExtension->password.Length,(PCHAR)(pBuffer + uMemoryOffsetWrite), SECTOR_SIZE);
					//EncryptDecrypt(pDeviceExtension->password.Buffer,pDeviceExtension->password.Length,(pBuffer + uMemoryOffsetWrite), SECTOR_SIZE,TRUE);
					//
					if (pDeviceExtension->crypt_mode == RC4)
					{
						Rc4Crypt(pDeviceExtension->password.Buffer, pDeviceExtension->password.Length, (PCHAR)(pBuffer + uMemoryOffsetWrite), SECTOR_SIZE);
					}
					else
					{
						DecryptEncrypt((UCHAR*)pDeviceExtension->password.Buffer, pDeviceExtension->password.Length, pBuffer + uMemoryOffsetWrite, SECTOR_SIZE, TRUE);
					}


					uMemoryOffsetWrite += SECTOR_SIZE;

				}




				/*for (i=0, j; i < IoStack->Parameters.Write.Length; ++i, j++)
				{
					j == pDeviceExtension->password.Length ? j = 0 : j;
					pBuffer[i] ^= pDeviceExtension->password.Buffer[j];
				}*/
				IoStack->Parameters.Write.ByteOffset.QuadPart += PASSWORD_OFFSET;
				if (ZwWriteFile(
					pDeviceExtension->file_handle,
					NULL,
					NULL,
					NULL,
					&pIrp->IoStatus,
					pBuffer,
					IoStack->Parameters.Write.Length,
					&IoStack->Parameters.Write.ByteOffset,
					NULL
				) != STATUS_SUCCESS)
				{
					//DbgBreakPoint();
				}
				ExFreePool(pBuffer);
				break;


			case IRP_MJ_DEVICE_CONTROL:
				switch (IoStack->Parameters.DeviceIoControl.IoControlCode)
				{

				case IOCTL_FILE_DISK_OPEN_FILE:
					pIrp->IoStatus.Status = CreateFile(pDeviceObject, pIrp, TRUE);
					break;

				case IOCTL_FILE_DISK_CLOSE_FILE:
					pIrp->IoStatus.Status = CloseFile(pDeviceObject, pIrp);
					break;

				default:
					pIrp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;

				}
				break;
			default:
				pIrp->IoStatus.Status = STATUS_SUCCESS;

			}

			IoCompleteRequest(
				pIrp,
				(CCHAR)(NT_SUCCESS(pIrp->IoStatus.Status) ?
					IO_DISK_INCREMENT : IO_NO_INCREMENT
					)
			);


		}

	}


}
#pragma code_seg() // end PAGE section

#pragma code_seg("PAGE")
NTSTATUS CreateFile(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp, BOOLEAN bIsOpen)
{
	PAGED_CODE();
	ASSERT(pDeviceObject != NULL);
	ASSERT(pIrp != NULL);

	PDEVICE_EXTENSION pDeviceExtension = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;
	POPEN_FILE_INFORMATION pOpenFileInformation = (POPEN_FILE_INFORMATION)pIrp->AssociatedIrp.SystemBuffer;
	pDeviceExtension->letter = pOpenFileInformation->DriveLetter;

	pDeviceExtension->file_name.Length = pOpenFileInformation->FileNameLength * sizeof(WCHAR);
	pDeviceExtension->file_name.MaximumLength = pOpenFileInformation->FileNameLength * sizeof(WCHAR);
	pDeviceExtension->file_name.Buffer = ExAllocatePoolWithTag(NonPagedPool, pOpenFileInformation->FileNameLength * sizeof(WCHAR), FILE_DISK_POOL_TAG);
#ifdef DBG
	DbgBreakPoint();
#endif

	pDeviceExtension->password.Length = pOpenFileInformation->PasswordLength;
	pDeviceExtension->password.MaximumLength = MAX_PASSWORD_SIZE;
	pDeviceExtension->password.Buffer = ExAllocatePoolWithTag(NonPagedPool, pOpenFileInformation->PasswordLength, FILE_DISK_POOL_TAG);
	pDeviceExtension->crypt_mode = pOpenFileInformation->CryptMode;


	if (pDeviceExtension->file_name.Buffer == NULL
		||
		pDeviceExtension->password.Buffer == NULL)
	{


		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlCopyMemory(
		pDeviceExtension->file_name.Buffer,
		pOpenFileInformation->FileName,
		pOpenFileInformation->FileNameLength * sizeof(WCHAR)
	);

	RtlCopyMemory(
		pDeviceExtension->password.Buffer,
		pOpenFileInformation->Password,
		pOpenFileInformation->PasswordLength
	);

	UNICODE_STRING usFileName;
	usFileName.Length = pDeviceExtension->file_name.Length;
	usFileName.MaximumLength = pDeviceExtension->file_name.MaximumLength;
	usFileName.Buffer = ExAllocatePoolWithTag(
		NonPagedPool,
		pDeviceExtension->file_name.Length * sizeof(WCHAR),
		FILE_DISK_POOL_TAG
	);

	if (usFileName.Buffer == NULL)
	{
		return STATUS_DATA_ERROR;
	}

	RtlCopyMemory(
		usFileName.Buffer,
		pDeviceExtension->file_name.Buffer,
		pDeviceExtension->file_name.Length * sizeof(WCHAR)
	);


	NTSTATUS status = STATUS_SUCCESS;

	OBJECT_ATTRIBUTES attributes;

	InitializeObjectAttributes(
		&attributes,
		&usFileName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
	);
	if (!bIsOpen)
	{
		goto CreateFile;
	}
	status = ZwCreateFile(
		&pDeviceExtension->file_handle,
		GENERIC_READ | GENERIC_WRITE,
		&attributes,
		&pIrp->IoStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN,
		FILE_NON_DIRECTORY_FILE |
		FILE_RANDOM_ACCESS |
		FILE_NO_INTERMEDIATE_BUFFERING |
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (NT_SUCCESS(status))
	{
		if (pDeviceExtension->crypt_mode != CryptPrev)
		{
			status = STATUS_INVALID_PARAMETER;

			ExFreePool(pDeviceExtension->file_name.Buffer);
			ExFreePool(pDeviceExtension->password.Buffer);
			RtlFreeUnicodeString(&usFileName);
			ZwClose(pDeviceExtension->file_handle);
			return status;

		}
		const PUCHAR pPasswordBuffer = (PUCHAR)ExAllocatePoolWithTag(
			PagedPool,
			MAX_PASSWORD_SIZE + CRYPT_OFFSET,
			FILE_DISK_POOL_TAG);


		if (pPasswordBuffer == NULL)
		{

			status = STATUS_INSUFFICIENT_RESOURCES;

			ExFreePool(pDeviceExtension->file_name.Buffer);
			ExFreePool(pDeviceExtension->password.Buffer);
			RtlFreeUnicodeString(&usFileName);
			ZwClose(pDeviceExtension->file_handle);
			return status;

		}

		ZwReadFile(
			pDeviceExtension->file_handle,
			NULL,
			NULL,
			NULL,
			&pIrp->IoStatus,
			pPasswordBuffer,
			MAX_PASSWORD_SIZE + CRYPT_OFFSET,
			NULL,
			NULL
		);

		const enum Crypt mode = pPasswordBuffer[CRYPT_OFFSET - 1];
		pDeviceExtension->crypt_mode = mode;


		if (mode == RC4)
		{
			Rc4Crypt(
				pDeviceExtension->password.Buffer,
				MAX_PASSWORD_SIZE,
				(PCHAR)(pPasswordBuffer + CRYPT_OFFSET),
				MAX_PASSWORD_SIZE);
		}
		else
		{
			DecryptEncrypt(
				(UCHAR*)pDeviceExtension->password.Buffer,
				MAX_PASSWORD_SIZE,
				pPasswordBuffer + CRYPT_OFFSET,
				MAX_PASSWORD_SIZE,
				FALSE);

		}

		for (int i = CRYPT_OFFSET; i <= pDeviceExtension->password.Length; ++i)
		{
			if (pPasswordBuffer[i] != pDeviceExtension->password.Buffer[i])
			{
				ExFreePool(pDeviceExtension->file_name.Buffer);
				ExFreePool(pDeviceExtension->password.Buffer);
				ExFreePool(pPasswordBuffer);

				RtlFreeUnicodeString(&usFileName);
				ZwClose(pDeviceExtension->file_handle);
				status = STATUS_INVALID_PARAMETER;

				return status;
			}
		}

#ifdef DBG
		DbgPrint("VED: Password is valid!.\n\r");
#endif

		DbgPrint("VED: File %.*S opened.\n",
			usFileName.Length / 2,
			usFileName.Buffer);
		ExFreePool(pPasswordBuffer);

		}

	if (status == STATUS_OBJECT_NAME_NOT_FOUND || status == STATUS_NO_SUCH_FILE)
	{
		if (pOpenFileInformation->FileSize.QuadPart == 0)
		{
			DbgPrint("VED: File %.*S not found.\n",
				usFileName.Length / 2,
				usFileName.Buffer);

			ExFreePool(pDeviceExtension->file_name.Buffer);
			ExFreePool(pDeviceExtension->password.Buffer);
			RtlFreeUnicodeString(&usFileName);


			return STATUS_NO_SUCH_FILE;
		}

	CreateFile:

		if (pDeviceExtension->crypt_mode == CryptPrev)
		{

			status = STATUS_INVALID_PARAMETER;

			ExFreePool(pDeviceExtension->file_name.Buffer);
			ExFreePool(pDeviceExtension->password.Buffer);
			RtlFreeUnicodeString(&usFileName);
			return status;

		}

		status = ZwCreateFile(
			&pDeviceExtension->file_handle,
			GENERIC_READ | GENERIC_WRITE,
			&attributes,
			&pIrp->IoStatus,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			0,
			FILE_OPEN_IF,
			FILE_NON_DIRECTORY_FILE |
			FILE_RANDOM_ACCESS |
			FILE_NO_INTERMEDIATE_BUFFERING |
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			0
		);


		if (!NT_SUCCESS(status))
		{
			DbgPrint("VED: File %.*S  could not be created.\n\r",
				usFileName.Length / 2,
				usFileName.Buffer);
			ExFreePool(pDeviceExtension->file_name.Buffer);
			ExFreePool(pDeviceExtension->password.Buffer);
			RtlFreeUnicodeString(&usFileName);
			pIrp->IoStatus.Information = status;
			return status;
		}

		if (pIrp->IoStatus.Information == FILE_OPENED)
		{

			DbgPrint("VED: File %.*S  file now is exist.\n\r",
				usFileName.Length / 2,
				usFileName.Buffer);
			ExFreePool(pDeviceExtension->file_name.Buffer);
			ExFreePool(pDeviceExtension->password.Buffer);
			RtlFreeUnicodeString(&usFileName);
			pIrp->IoStatus.Information = 0x50;
			ZwClose(pDeviceExtension->file_handle);
			return STATUS_SUCCESS;

		}

		if (pIrp->IoStatus.Information == FILE_CREATED)
		{


			DbgPrint("VED: File %.*S created.\n\r",
				usFileName.Length / 2,
				usFileName.Buffer);

			/*
			 * The ZwFsControlFile routine sends a control code directly to a specified file system or file system filter driver, causing the corresponding driver to perform the specified action.
			 */
			status = ZwFsControlFile(
				pDeviceExtension->file_handle,
				NULL,
				NULL,
				NULL,
				&pIrp->IoStatus,
				FSCTL_SET_SPARSE,
				NULL,
				0,
				NULL,
				0
			);

			if (NT_SUCCESS(status))
			{
				DbgPrint("VED:File attributes set to sparse.\n\r");
			}

			FILE_END_OF_FILE_INFORMATION EofFile;
			EofFile.EndOfFile.QuadPart = pOpenFileInformation->FileSize.QuadPart;

			status = ZwSetInformationFile(
				pDeviceExtension->file_handle,
				&pIrp->IoStatus,
				&EofFile,
				sizeof(FILE_END_OF_FILE_INFORMATION),
				FileEndOfFileInformation
			);

			if (!NT_SUCCESS(status))
			{
				DbgPrint("VED: eof could not be set.\n");
				ExFreePool(pDeviceExtension->file_name.Buffer);
				ExFreePool(pDeviceExtension->password.Buffer);
				RtlFreeUnicodeString(&usFileName);
				ZwClose(pDeviceExtension->file_handle);
				return status;

			}

			DbgPrint("VED: eof set to %lld.\n",
				EofFile.EndOfFile.QuadPart);


			PUCHAR pPasswordBuffer = (PUCHAR)ExAllocatePoolWithTag(
				NonPagedPool,
				PASSWORD_OFFSET,
				FILE_DISK_POOL_TAG);

			if (pPasswordBuffer == NULL)
			{
				ExFreePool(pDeviceExtension->file_name.Buffer);
				ExFreePool(pDeviceExtension->password.Buffer);
				RtlFreeUnicodeString(&usFileName);
				ZwClose(pDeviceExtension->file_handle);
				return status;
			}
			memset(pPasswordBuffer, 0, PASSWORD_OFFSET);

			pPasswordBuffer[CRYPT_OFFSET - 1] = (char)pDeviceExtension->crypt_mode;

			for (int i = CRYPT_OFFSET; i <= pDeviceExtension->password.Length; ++i)
			{
				pPasswordBuffer[i] = pDeviceExtension->password.Buffer[i];
			}


			if (pDeviceExtension->crypt_mode == RC4)
			{
				Rc4Crypt(
					pDeviceExtension->password.Buffer,
					MAX_PASSWORD_SIZE,
					(PCHAR)(pPasswordBuffer + CRYPT_OFFSET),
					MAX_PASSWORD_SIZE);
			}
			else
			{
				DecryptEncrypt(
					(UCHAR*)pDeviceExtension->password.Buffer,
					MAX_PASSWORD_SIZE,
					pPasswordBuffer + CRYPT_OFFSET,
					MAX_PASSWORD_SIZE,
					TRUE);

			}


			if (ZwWriteFile(
				pDeviceExtension->file_handle,
				NULL,
				NULL,
				NULL,
				&pIrp->IoStatus,
				pPasswordBuffer,
				PASSWORD_OFFSET,
				NULL, // byte offset
				NULL
			) != STATUS_SUCCESS)
			{
				DbgPrint("VED: Failed write password in file!.\n\r");
				ExFreePool(pDeviceExtension->file_name.Buffer);
				ExFreePool(pDeviceExtension->password.Buffer);
				RtlFreeUnicodeString(&usFileName);
				ZwClose(pDeviceExtension->file_handle);
				ExFreePool(pPasswordBuffer);
				return status;

			}

			DbgPrint("VED: Password was be written!\n\r");

			ExFreePool(pPasswordBuffer);

		}

	}
	else if (!NT_SUCCESS(status))
	{
		DbgPrint("VED: File %.*S could not be opened.\n\r",
			usFileName.Length / 2,
			usFileName.Buffer);
		ExFreePool(pDeviceExtension->file_name.Buffer);
		ExFreePool(pDeviceExtension->password.Buffer);
		RtlFreeUnicodeString(&usFileName);
		return status;
	}

	RtlFreeUnicodeString(&usFileName);

	FILE_BASIC_INFORMATION FileBasicInfo;
	status = ZwQueryInformationFile(
		pDeviceExtension->file_handle,
		&pIrp->IoStatus,
		&FileBasicInfo,
		sizeof(FILE_BASIC_INFORMATION),
		FileBasicInformation
	);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("VED: Failed ZwQueryInformationFile!(FileBasicInformation).\n\r");

		ExFreePool(pDeviceExtension->file_name.Buffer);
		ExFreePool(pDeviceExtension->password.Buffer);
		ZwClose(pDeviceExtension->file_handle);
		return status;
	}
	FILE_STANDARD_INFORMATION FileStandardInfo;

	status = ZwQueryInformationFile(
		pDeviceExtension->file_handle,
		&pIrp->IoStatus,
		&FileStandardInfo,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation
	);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("VED: Failed ZwQueryInformationFile!(FileStandardInformation).\n\r");
		ExFreePool(pDeviceExtension->file_name.Buffer);
		ExFreePool(pDeviceExtension->password.Buffer);
		ZwClose(pDeviceExtension->file_handle);
		return status;
	}
	pDeviceExtension->file_size.QuadPart = FileStandardInfo.EndOfFile.QuadPart;

	FILE_ALIGNMENT_INFORMATION FileAlignmentInfo;

	status = ZwQueryInformationFile(
		pDeviceExtension->file_handle,
		&pIrp->IoStatus,
		&FileAlignmentInfo,
		sizeof(FILE_ALIGNMENT_INFORMATION),
		FileAlignmentInformation
	);


	if (!NT_SUCCESS(status))
	{
		DbgPrint("VED: Failed ZwQueryInformationFile!(FILE_ALIGNMENT_INFORMATION).\n\r");

		ExFreePool(pDeviceExtension->file_name.Buffer);
		ExFreePool(pDeviceExtension->password.Buffer);
		ZwClose(pDeviceExtension->file_handle);
		return status;
	}

	pDeviceObject->AlignmentRequirement = FileAlignmentInfo.AlignmentRequirement;

	/*
	 * Specifies one or more system-defined constants, combined with a bitwise OR operation, that provide additional information about the driver's device.
	 * These constants include the following:
	 */
	pDeviceObject->Characteristics &= ~FILE_READ_ONLY_DEVICE;
	pDeviceExtension->media_in_device = TRUE;

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;

	return STATUS_SUCCESS;
	}
#pragma code_seg() // end PAGE section

#pragma code_seg("PAGE")
NTSTATUS CloseFile(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	PAGED_CODE();
	ASSERT(pDeviceObject != NULL);
	ASSERT(pIrp != NULL);

	PDEVICE_EXTENSION pDeviceExtension = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;
	ExFreePool(pDeviceExtension->file_name.Buffer);
	ExFreePool(pDeviceExtension->password.Buffer);
#ifdef DBG
	DbgBreakPoint();
#endif


	ZwClose(pDeviceExtension->file_handle);
	

	pDeviceExtension->media_in_device = FALSE;
	pDeviceExtension->crypt_mode = CryptPrev;
	pDeviceExtension->file_size.QuadPart = 0;
	pDeviceExtension->letter = ' ';

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;

	return STATUS_SUCCESS;

}
#pragma code_seg() // end PAGE section
