/********************************************************************
	created:	2013/05/16
	created:	16:5:2013   22:15
	filename: 	Sysark\Sysark\Sysark.cpp
	file path:	Sysark\Sysark
	file base:	Sysark
	file ext:	cpp
	author:		hostzhen(http://hi.baidu.com/hostzhen)
	
	purpose:	
*********************************************************************/
#include "Sysark.h"


PEPROCESS g_pSystemProcess;
extern PServiceDescriptorTable g_KeServiceDescriptorTableShadow;

/************************************************************************
* ��������:DriverEntry
* ��������:��ʼ���������򣬶�λ������Ӳ����Դ�������ں˶���
* �����б�:
      pDriverObject:��I/O�������д���������������
      pRegistryPath:����������ע�����е�·��
* ���� ֵ:���س�ʼ������״̬
*************************************************************************/
#pragma INITCODE
extern "C" NTSTATUS DriverEntry (
			IN PDRIVER_OBJECT pDriverObject,
			IN PUNICODE_STRING pRegistryPath	) 
{
	NTSTATUS status;
	KdPrint(("Enter DriverEntry\n"));


	//ע�������������ú������
	pDriverObject->DriverUnload = IoDriverUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HOSTZHEN_DispatchCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HOSTZHEN_DispatchCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HOSTZHEN_DispatchCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HOSTZHEN_DispatchCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoDispatchDeviceControl;
	//_asm int 3
	//���������豸����
	status = CreateDevice(pDriverObject);

	pDriverObject->Flags |= DO_BUFFERED_IO;

	g_pSystemProcess = PsGetCurrentProcess();
	g_pDriverObj = pDriverObject;

	g_KeServiceDescriptorTableShadow = (PServiceDescriptorTable)GetKeServiceDescriptorTableShadow();

	DataInitialize();

	KdPrint(("DriverEntry end\n"));
	return status;
}

/************************************************************************
* ��������:CreateDevice
* ��������:��ʼ���豸����
* �����б�:
      pDriverObject:��I/O�������д���������������
* ���� ֵ:���س�ʼ��״̬
*************************************************************************/
#pragma INITCODE
NTSTATUS CreateDevice (
		IN PDRIVER_OBJECT	pDriverObject) 
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;
	UNICODE_STRING usDeviceName;
	//�����豸����
	RtlInitUnicodeString(&usDeviceName, DEVICE_NAME);

	
	//�����豸
	status = IoCreateDevice( pDriverObject,
						sizeof(DEVICE_EXTENSION),
						&(UNICODE_STRING)usDeviceName,
						FILE_DEVICE_UNKNOWN,
						0, TRUE,
						&pDevObj );
	if (!NT_SUCCESS(status))
		return status;

	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	RtlInitUnicodeString(&pDevExt->ustrDeviceName, DEVICE_NAME);
	//_asm int 3
	//������������
	RtlInitUnicodeString(&pDevExt->ustrSymLinkName, SYMLINK_NAME);
	status = IoCreateSymbolicLink( &pDevExt->ustrSymLinkName, &pDevExt->ustrDeviceName );
	if (!NT_SUCCESS(status)) 
	{
		IoDeleteDevice( pDevObj );
		return status;
	}
	return STATUS_SUCCESS;
}

/************************************************************************
* ��������:IoDriverUnload
* ��������:�������������ж�ز���
* �����б�:
      pDriverObject:��������
* ���� ֵ:����״̬
*************************************************************************/
#pragma PAGEDCODE
VOID IoDriverUnload (IN PDRIVER_OBJECT pDriverObject) 
{
	PDEVICE_OBJECT	pNextObj;
	KdPrint(("Enter IoDriverUnload\n"));
	//_asm int 3
	FreeAllPageLookasideLists();
		
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, SYMLINK_NAME);
	pNextObj = pDriverObject->DeviceObject;
	IoDeleteSymbolicLink(&symLinkName);
	while (pNextObj != NULL) 
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)
			pNextObj->DeviceExtension;

		//ɾ����������
		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice( pDevExt->pDevice );
	}
}

/************************************
* ��������:  FreeAllPageLookasideLists
* ��������:  �ͷ����е�PageLookasideList
* ���� ֵ:   VOID
/************************************/
#pragma PAGEDCODE
VOID FreeAllPageLookasideLists()
{
	//_asm int 3
	FreePagedLookasideListForDirectory();
	FreePagedLookasideListForDriverModules();
	FreePagedLookasideListForServices();
	FreePagedLookasideListForSSDT();
	FreePagedLookasideListForShadowSSDT();
	FreePagedLookasideListForStartup();
}

/************************************************************************
* ��������:IoDispatchDeviceControl
* ��������:��DeviceControl��IRP���д���
* �����б�:
      pDevObj:�����豸����
      pIrp:��IO�����
* ���� ֵ:����״̬
*************************************************************************/
#pragma PAGEDCODE
NTSTATUS IoDispatchDeviceControl( IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp )
{
	PVOID InputBuffer, OutputBuffer;
	ULONG InputBufferLength, OutputBufferLength;
	ULONG IoControlCode;
	TCHAR pMsg[MAX_PATH];
	ULONG msgLen, pid, ulBuffer;
	PEPROCESS pEProces;
	UNICODE_STRING usBuffer;
	NTSTATUS status = STATUS_SUCCESS;
	PALL_PROCESSES_INFO pAllProcessInfo;
	PALL_PROCESS_MODULE_INFO pAllProcessModuleInf;
	PALL_PROCESS_THREAD_INFO pAllProcessThreadInf;


	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(pIrp);


	InputBuffer = pIrp->AssociatedIrp.SystemBuffer;
	InputBufferLength = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
	OutputBufferLength = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
	IoControlCode = IrpStack->Parameters.DeviceIoControl.IoControlCode;
	pIrp->IoStatus.Information = NULL;

	KdPrint(("IoControlCode is 0x%08x\n", IoControlCode));

	switch(IrpStack->Parameters.DeviceIoControl.IoControlCode)
	{
	//ǿɱ����
	case IOCTL_KILL_PROCESS:
		{
			//_asm int 3
			KdPrint(("enum kill process:\n"));
			__try
			{
				pid = *(ULONG*)InputBuffer;
				status = PsLookupProcessByProcessId((HANDLE)pid, &pEProces);
				if(NT_SUCCESS(status))
				{
					ObDereferenceObject(pEProces);
				}
				status = TerminateProcess(pEProces);
				if(NT_SUCCESS(status))
				{
					DbgPrint("TerminateProcess Ok!\n");
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				pIrp->IoStatus.Information = STATUS_INVALID_PARAMETER;
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//ö�ٽ���
	case IOCTL_ENUM_PROCESS:
		{
			KdPrint(("enum process :\n"));
			//_asm int 3
			__try {

				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);

				pAllProcessInfo = (PALL_PROCESSES_INFO)ExAllocatePool(PagedPool, sizeof(ALL_PROCESSES_INFO));
				RtlZeroMemory(pAllProcessInfo, sizeof(ALL_PROCESSES_INFO));

				//EnumProcessByActiveProcess(pDeviceObject, pIrp, pAllProcessInfo);
				ScanXpHandleTable(pDeviceObject, pIrp, pAllProcessInfo);
				EnumProcessByThreadList(pDeviceObject, pIrp, pAllProcessInfo);

				if (RtlCopyMemory(OutputBuffer, pAllProcessInfo, sizeof(PROCESS_INFO) * pAllProcessInfo->uCount + sizeof(ULONG32)))
				{
					pIrp->IoStatus.Information = sizeof(PROCESS_INFO) * pAllProcessInfo->uCount + sizeof(ULONG32);
				}

			} __except( EXCEPTION_EXECUTE_HANDLER ) {

				pIrp->IoStatus.Information = STATUS_INVALID_PARAMETER;
				status = STATUS_UNSUCCESSFUL;
			}
			if (NULL != pAllProcessInfo)
			{
				ExFreePool(pAllProcessInfo);
				pAllProcessInfo = NULL;
			}

		}
		break;
	//ö�ٽ���ģ��
	case IOCTL_ENUM_PROCESS_MODULE:
		{
			KdPrint(("enum process module:\n"));
			//_asm int 3
			__try {
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				pAllProcessModuleInf = (PALL_PROCESS_MODULE_INFO)ExAllocatePool(PagedPool, sizeof(ALL_PROCESS_MODULE_INFO));
				RtlZeroMemory(pAllProcessModuleInf, sizeof(ALL_PROCESS_MODULE_INFO));
				pid = *(ULONG*)InputBuffer;
				PsLookupProcessByProcessId((HANDLE)pid, &pEProces);
				ObDereferenceObject(pEProces);

				EnumProcessModuleByPEB(pEProces, pAllProcessModuleInf);

				if (RtlCopyMemory(OutputBuffer, pAllProcessModuleInf, sizeof(PROCESS_MODULE_INFO) * pAllProcessModuleInf->uCount + sizeof(ULONG32)))
				{
					pIrp->IoStatus.Information = sizeof(PROCESS_MODULE_INFO) * pAllProcessModuleInf->uCount + sizeof(ULONG32);
				}

			} __except( EXCEPTION_EXECUTE_HANDLER ) {

				pIrp->IoStatus.Information = STATUS_INVALID_PARAMETER;
				status = STATUS_UNSUCCESSFUL;
			}
			if (NULL != pAllProcessModuleInf)
			{
				ExFreePool(pAllProcessModuleInf);
				pAllProcessModuleInf = NULL;
			}
		}
		break;
	//ö�ٽ����߳�
	case IOCTL_ENUM_PROCESS_THREAD:
		{
			KdPrint(("enum process thread:\n"));
			//_asm int 3
			__try { 

				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				pAllProcessThreadInf = (PALL_PROCESS_THREAD_INFO)ExAllocatePool(PagedPool, sizeof(PALL_PROCESS_THREAD_INFO));
				RtlZeroMemory(pAllProcessThreadInf, sizeof(PALL_PROCESS_THREAD_INFO));
				pid = *(ULONG*)InputBuffer;
				PsLookupProcessByProcessId((HANDLE)pid, &pEProces);
				ObDereferenceObject(pEProces);

				EnumProcessThread(pEProces, pAllProcessThreadInf);

				if (RtlCopyMemory(OutputBuffer, pAllProcessThreadInf, sizeof(PROCESS_THREAD_INFO) * pAllProcessThreadInf->uCount + sizeof(ULONG32)))
				{
					pIrp->IoStatus.Information = sizeof(PROCESS_THREAD_INFO) * pAllProcessThreadInf->uCount + sizeof(ULONG32);
				}

			} __except( EXCEPTION_EXECUTE_HANDLER ) {

				pIrp->IoStatus.Information = STATUS_INVALID_PARAMETER;
				status = STATUS_UNSUCCESSFUL;
			}
			if (NULL != pAllProcessThreadInf)
			{
				ExFreePool(pAllProcessThreadInf);
				pAllProcessThreadInf = NULL;
			}

		}
		break;
	//ɾ���ļ�
	case IOCTL_DELETE_FILE:
		{
			KdPrint(("enum delete file :\n"));
			//_asm int 3
			__try { 

				RtlInitUnicodeString(&usBuffer, (PWCHAR)InputBuffer);
				DeleteFile(&usBuffer);

			} __except( EXCEPTION_EXECUTE_HANDLER ) {

				pIrp->IoStatus.Information = STATUS_INVALID_PARAMETER;
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//��ȡ����ģ�����Ŀ
	case IOCTL_GET_DRIVER_MODULE_INFO_SIZE:
		{
			//_asm int 3
			KdPrint(("get driver modules info size :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				EnumDriverModules(g_pDriverObj);
				ulBuffer = GetDriverModulesInfoCount();
				if (RtlCopyMemory(OutputBuffer, &ulBuffer, sizeof(ULONG)))
				{
					pIrp->IoStatus.Information = sizeof(ulBuffer);
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				FreePagedLookasideListForDriverModules();
				KdPrint(("IOCTL_GET_DRIVER_MODULE_INFO_SIZE error"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//��ȡ����ģ���б���Ϣ
	case IOCTL_ENUM_DRIVER_MODULE:
		{
			//_asm int 3
			KdPrint(("enum driver module :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				GetDriverModulesInfo(OutputBuffer);
				ulBuffer = GetDriverModulesInfoCount();
				pIrp->IoStatus.Information = ulBuffer * sizeof(DRIVER_MODULE_INFO);
				FreePagedLookasideListForDriverModules();
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_ENUM_DRIVER_MODULE error!"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//��ȡ�����б����Ŀ
	case IOCTL_GET_SERVICES_COUNT:
		{
			//_asm int 3
			KdPrint(("get services count :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				EnumServices();
				ulBuffer = GetServicesCount();
				if (RtlCopyMemory(OutputBuffer, &ulBuffer, sizeof(ULONG)))
				{
					pIrp->IoStatus.Information = sizeof(ulBuffer);
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				FreePagedLookasideListForServices();
				KdPrint(("IOCTL_GET_DRIVER_MODULE_INFO_SIZE error"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//��ȡ�����б����Ϣ
	case IOCTL_ENUM_SERVICES:
		{
			//_asm int 3
			KdPrint(("enum services :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				GetServicesInfo(OutputBuffer);
				ulBuffer = GetServicesCount();
				pIrp->IoStatus.Information = ulBuffer * sizeof(DRIVER_MODULE_INFO);
				FreePagedLookasideListForServices();
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_ENUM_DRIVER_MODULE error!"));
				status = STATUS_UNSUCCESSFUL;
			}

		}
		break;
	//��ȡ�������б����Ŀ
	case IOCTL_GET_STARTUP_INFO_COUNT:
		{
			//_asm int 3
			KdPrint(("get startup count :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				EnumSystemStartupInfo();
				ulBuffer = GetSystemStartupInfoCount();
				if (RtlCopyMemory(OutputBuffer, &ulBuffer, sizeof(ULONG)))
				{
					pIrp->IoStatus.Information = sizeof(ulBuffer);
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				//FreePagedLookasideListForStartup();
				KdPrint(("IOCTL_GET_STARTUP_INFO_COUNT error"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//��ȡ�������б����Ϣ
	case IOCTL_ENUM_STARTUP:
		{
			//_asm int 3
			KdPrint(("enum startup :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				GetSystemStartupInfo(OutputBuffer);
				ulBuffer = GetSystemStartupInfoCount();
				pIrp->IoStatus.Information = ulBuffer * sizeof(STARTUP_INFO);
				FreePagedLookasideListForStartup();
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_ENUM_STARTUP error!"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//���SSDT����
	case IOCTL_GET_SSDT_COUNT:
		{
			//_asm int 3
			KdPrint(("hook count :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				ulBuffer = GetSSDTCount();
				if (RtlCopyMemory(OutputBuffer, &ulBuffer, sizeof(ULONG)))
				{
					pIrp->IoStatus.Information = sizeof(ulBuffer);
				}

			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_GET_SSDT_COUNT error!"));
				status = STATUS_UNSUCCESSFUL;
			}


		}
		break;
	case IOCTL_ENUM_SSDT:
		{
			//_asm int 3
			KdPrint(("enum SSDT :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				EnumSSDT(OutputBuffer);
				ulBuffer = GetSSDTCount();
				pIrp->IoStatus.Information = ulBuffer * sizeof(STARTUP_INFO);
				FreePagedLookasideListForSSDT();
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_ENUM_SSDT error!"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//����SSDT
	case IOCTL_SET_SSDT:
		{
			//_asm int 3
			KdPrint(("set ssdt :\n"));
			__try
			{
				DisableWriteProtection();
				SetSSDT((PVOID)InputBuffer);
				EnableWriteProtection();

			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_SETSSDT error!"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//���SHADOW SSDT ����
	case IOCTL_GET_SHADOW_HOOK_COUNT:
		{
			//_asm int 3
			KdPrint(("shadow hook count :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				ulBuffer = GetShadowSSDTCount();
				if (RtlCopyMemory(OutputBuffer, &ulBuffer, sizeof(ULONG)))
				{
					pIrp->IoStatus.Information = sizeof(ulBuffer);
				}

			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_GET_SHADOW_HOOK_COUNT error!"));
				status = STATUS_UNSUCCESSFUL;
			}

		}
		break;
	case IOCTL_ENUM_SHADOW_HOOK:
		{			
			//_asm int 3
			KdPrint(("enum shadow ssdt :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				EnumShadowSSDT(OutputBuffer);
				ulBuffer = GetShadowSSDTCount();
				pIrp->IoStatus.Information = ulBuffer * sizeof(STARTUP_INFO);
				FreePagedLookasideListForShadowSSDT();
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_ENUM_SHADOW_HOOK error!"));
				status = STATUS_UNSUCCESSFUL;
			}

		}
		break;
	//����SHADOW SSDT
	case IOCTL_SET_SHADOW_HOOK:
		{
			//_asm int 3
			KdPrint(("set shadow ssdt :\n"));
			__try
			{
				DisableWriteProtection();
				SetShadowSSDT((PVOID)InputBuffer);
				EnableWriteProtection();

			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_SET_SHADOW_HOOK error!"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//��ȡĿ¼���ļ��ļ���
	case IOCTL_GET_DIRECTORY_INFO_COUNT:
		{
			//_asm int 3
			KdPrint(("get directory info count :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				EnumDirectory((PCHAR)InputBuffer);
				ulBuffer = GetDirectoryInfoCount();
				if (RtlCopyMemory(OutputBuffer, &ulBuffer, sizeof(ULONG)))
				{
					pIrp->IoStatus.Information = sizeof(ulBuffer);
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_GET_DIRECTORY_INFO_COUNT error!"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//��ȡĿ¼�������ļ�����Ϣ
	case IOCTL_ENUM_DIRECTORY_INFO:
		{
			//_asm int 3
			KdPrint(("enum directory info :\n"));
			__try
			{
				OutputBuffer = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
				RtlZeroMemory(OutputBuffer, pIrp->MdlAddress->ByteCount);
				GetDirectoryInfo((PVOID)OutputBuffer);
				ulBuffer = GetDirectoryInfoCount();
				pIrp->IoStatus.Information = ulBuffer * sizeof(DIRECTORY_INFO);

			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_ENUM_DIRECTORY_INFO error!"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	//�޸��ļ�Ȩ��
	case IOCTL_MODIFY_FILE_RIGHT:
		{
			//_asm int 3
			KdPrint(("modify file right :\n"));
			__try
			{
				status = ModifyRight((FILE_RIGHT_MODIFY*)InputBuffer);
				status = STATUS_SUCCESS;

			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("IOCTL_MODIFY_FILE_RIGHT error!"));
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		pIrp->IoStatus.Information = 0;
		break;
	}

	//ClearLog();

	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}



#pragma PAGEDCODE
NTSTATUS HOSTZHEN_DispatchCreateClose(
									  IN PDEVICE_OBJECT		DeviceObject,
									  IN PIRP					Irp
									  )
{

	KIRQL CurIrql = KeGetCurrentIrql();
	//_asm int 3
	//KeSwitchProcess(4);

	//KIRQL oldIrql;
	//KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);
	//EnumProcess();
	//KeLowerIrql(oldIrql);
	NTSTATUS status = STATUS_SUCCESS;
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
