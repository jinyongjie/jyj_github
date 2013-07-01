#include "DriverModule.h"

//ȫ�ֱ���
PDRIVER_MODULE_INFO g_pDriverModuleInfo = NULL;
PAGED_LOOKASIDE_LIST g_PageListDriverModule;


NTSTATUS EnumDriverModules(PDRIVER_OBJECT pDriverObj)
{
	NTSTATUS status = STATUS_SUCCESS;
	PLIST_ENTRY pListHead = NULL, pListCur = NULL;
	PLDR_DATA_TABLE_ENTRY pLdrDataTable = NULL;
	PDRIVER_MODULE_INFO pDriverModuleInfo = NULL,\
		pPreDriverModuleInfo = NULL;

	pListHead = ((PLIST_ENTRY)pDriverObj->DriverSection)->Flink;
	pListCur = pListHead;

	__try
	{

		FreePagedLookasideListForDriverModules();
		ExInitializePagedLookasideList(&g_PageListDriverModule, NULL, NULL, 0, sizeof(DRIVER_MODULE_INFO), NULL, 0);

		do 
		{
			pLdrDataTable = (PLDR_DATA_TABLE_ENTRY)CONTAINING_RECORD(pListCur,\
				LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

			if (pLdrDataTable != NULL && pLdrDataTable->EntryPoint != NULL)
			{
				pDriverModuleInfo = (PDRIVER_MODULE_INFO)ExAllocateFromPagedLookasideList(&g_PageListDriverModule);
				RtlZeroMemory(pDriverModuleInfo, sizeof(DRIVER_MODULE_INFO));
				pDriverModuleInfo->DllBase = (ULONG)pLdrDataTable->DllBase;
				pDriverModuleInfo->SizeOfImage = pLdrDataTable->SizeOfImage;
				RtlCopyMemory(pDriverModuleInfo->BaseDllName, pLdrDataTable->BaseDllName.Buffer, pLdrDataTable->BaseDllName.Length);
				RtlCopyMemory(pDriverModuleInfo->FullDllName, pLdrDataTable->FullDllName.Buffer, pLdrDataTable->FullDllName.Length);
				pDriverModuleInfo->next = NULL;


				if (g_pDriverModuleInfo == NULL)
				{
					g_pDriverModuleInfo = pDriverModuleInfo;
					pPreDriverModuleInfo = pDriverModuleInfo;
				}
				else
				{
					pPreDriverModuleInfo->next = pDriverModuleInfo;
					pPreDriverModuleInfo = pDriverModuleInfo;
				}
			}

			pListCur = pListCur->Flink;
		} while (pListCur != pListHead);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		FreePagedLookasideListForDriverModules();
		KdPrint(("DriverModule:EnumDriverModules failed!"));
		status = STATUS_UNSUCCESSFUL;
	}

	return status;
}


NTSTATUS FreePagedLookasideListForDriverModules()
{
	NTSTATUS status = STATUS_SUCCESS;
	PDRIVER_MODULE_INFO pDriverModuleInfo = NULL,\
		pNextDriverModuleInfo = NULL;

	__try
	{

		if (g_pDriverModuleInfo != NULL)
		{
			pDriverModuleInfo = g_pDriverModuleInfo;

			while (pDriverModuleInfo != NULL)
			{
				pNextDriverModuleInfo = pDriverModuleInfo->next;

				ExFreeToPagedLookasideList(&g_PageListDriverModule, pDriverModuleInfo);

				pDriverModuleInfo = pNextDriverModuleInfo;
			}

			ExDeletePagedLookasideList(&g_PageListDriverModule);
			g_pDriverModuleInfo = NULL;
		}

	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("DriverModule:FreePagedLookasideListForDriverModules failed!"));
		status = STATUS_UNSUCCESSFUL;
	}

	return status;
}

ULONG GetDriverModulesInfoCount()
{
	ULONG ulCount = 0;
	PDRIVER_MODULE_INFO pDriverModuleInf = g_pDriverModuleInfo;

	while (pDriverModuleInf)
	{
		++ulCount;
		pDriverModuleInf = pDriverModuleInf->next;
	}

	KdPrint(("DriverModule:GetDriverModulesInfoCount is %d", ulCount));
	return ulCount;
}

NTSTATUS GetDriverModulesInfo(PVOID pInfo)
{
	ULONG ulLength = 0;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PDRIVER_MODULE_INFO pDriverModuleInf = g_pDriverModuleInfo;

	__try
	{
		while (pDriverModuleInf)
		{

			RtlCopyMemory((PUCHAR)pInfo + ulLength, pDriverModuleInf, sizeof(DRIVER_MODULE_INFO));
			ulLength += sizeof(DRIVER_MODULE_INFO);
			pDriverModuleInf = pDriverModuleInf->next;
		}

	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		//_asm int 3
		KdPrint(("DriverModule:GetDriverModulesInfo failed"));
		status = STATUS_UNSUCCESSFUL;
	}

	return status;
}

EXTERN_C NTSTATUS GetDriverModulesName( ULONG ulFuncAddr, LPWSTR lpwPath )
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PDRIVER_MODULE_INFO pDriverModuleInf = g_pDriverModuleInfo;

	while (pDriverModuleInf)
	{
		if (pDriverModuleInf->DllBase >= ulFuncAddr && ulFuncAddr < pDriverModuleInf->DllBase + pDriverModuleInf->SizeOfImage)
		{
			RtlCopyMemory(lpwPath, pDriverModuleInf->FullDllName, MAX_PATH);
			status = STATUS_SUCCESS;
			break;
		}
		pDriverModuleInf = pDriverModuleInf->next;
	}

	return status;
}


//��ú������ڵ�ģ��·��
ULONG GetModuleName(PCHAR pModulePath, ULONG ulFuncAddress)
{
	ULONG i;
	ULONG ulAddress;

	ULONG ulSize; 
	NTSTATUS status; 
	PSYSTEM_MODULE_LIST pSystem_module_list; 

	//��һ������ʱ��ѯ�����ֽ��� 
	ZwQuerySystemInformation(SystemModuleInformation,NULL,0, &ulSize); 

	//ͨ�����ص��ֽ����������ڴ� 
	pSystem_module_list = (PSYSTEM_MODULE_LIST)ExAllocatePool(PagedPool,ulSize); 
	//��������ڴ�ʧ��
	if(!pSystem_module_list) 
	{ 
		//���������Ϣ
		DbgPrint("shadow.c GetWin32kBase() pSystem_module_list �����ڴ�ʧ�ܣ�");
		return 0; 
	} 

	//�ٴλ�� 
	status = ZwQuerySystemInformation(SystemModuleInformation,pSystem_module_list,ulSize,&ulSize); 
	//��ȡʧ�� 
	if(!NT_SUCCESS(status)) 
	{ 
		//���������Ϣ
		DbgPrint("shadow.c GetWin32kBase() ZwQuerySystemInformation ʧ�ܣ�");
		//�ͷ�������ڴ�
		ExFreePool(pSystem_module_list); 
		return 0; 
	} 

	//��������
	for (i=0; i < pSystem_module_list->ulCount; i++)
	{
		//ƥ���ַ���
		if (ulFuncAddress >= (ULONG)pSystem_module_list->smi[i].Base && ulFuncAddress < ((ULONG)pSystem_module_list->smi[i].Base + pSystem_module_list->smi[i].Size))
		{
			strcpy(pModulePath, (PCHAR)pSystem_module_list->smi[i].ImageName);
			ulAddress = (ULONG)pSystem_module_list->smi[i].Base;
			break;
		}
	}

	//�ͷ��ڴ�
	ExFreePool(pSystem_module_list);
	//���ص�ַ
	return ulAddress;
}