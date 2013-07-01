#include "Services.h"

//ȫ�ֱ���
PSERVICES_INFO g_pServicesInfo = NULL;
PAGED_LOOKASIDE_LIST g_PageListServices;

NTSTATUS EnumServices()
{
	//_asm int 3
	NTSTATUS status = STATUS_SUCCESS;
	ULONG	SubKeyIndex, ResultLength, ulSize;
	HANDLE					i, HandleRegKey = NULL;
	UNICODE_STRING			RegistryKeyName, KeyValue;
	UNICODE_STRING			KeyName;
	OBJECT_ATTRIBUTES		ObjectAttributes;
	PLIST_ENTRY pListHead = NULL, pListCur = NULL;
	PLDR_DATA_TABLE_ENTRY pLdrDataTable = NULL;
	PSERVICES_INFO pServicesInfo = NULL,\
		pPreServicesInfo = NULL;
	PKEY_BASIC_INFORMATION pKeyBasicInfo = NULL;
	PKEY_FULL_INFORMATION pKeyFullInfo = NULL;
	/************************************************************************/
	/* 
	User-mode Handle		Corresponding Object Name 
	HKEY_LOCAL_MACHINE		\Registry\Machine 
	HKEY_USERS				\Registry\User 
	HKEY_CLASSES_ROOT		No kernel-mode equivalent 
	HKEY_CURRENT_USER		No simple kernel-mode equivalent, but see Registry Run-Time Library Routines 
	*/
	/************************************************************************/

	WCHAR ServiceRegisterPath[] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\";

	//pListHead = ((PLIST_ENTRY)pDriverObj->DriverSection)->Flink;
	//pListCur = pListHead;

	__try
	{

		FreePagedLookasideListForServices();
		ExInitializePagedLookasideList(&g_PageListServices, NULL, NULL, 0, sizeof(SERVICES_INFO), NULL, 0);

		RtlInitUnicodeString(&RegistryKeyName, ServiceRegisterPath);
		InitializeObjectAttributes(&ObjectAttributes, 
			&RegistryKeyName,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
			NULL,    // handle
			NULL);
		status = ZwOpenKey(&HandleRegKey, KEY_READ, &ObjectAttributes);

		// ��һ�ε�����Ϊ�˻�ȡ��Ҫ�ĳ���
		ZwQueryKey(HandleRegKey, KeyFullInformation, NULL, 0, &ulSize);
		pKeyFullInfo = (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);
		// �ڶ��ε�����Ϊ�˻�ȡ����
		ZwQueryKey(HandleRegKey, KeyFullInformation, pKeyFullInfo, ulSize, &ulSize);

		//ѭ��������������
		for (SubKeyIndex = 0; SubKeyIndex <pKeyFullInfo->SubKeys; SubKeyIndex++)
		{
			ZwEnumerateKey(HandleRegKey, SubKeyIndex, KeyBasicInformation, NULL, 0, &ulSize);
			pKeyBasicInfo = (PKEY_BASIC_INFORMATION)ExAllocatePool(PagedPool, ulSize);
			//��ȡ��I�����������
			ZwEnumerateKey(HandleRegKey, SubKeyIndex, KeyBasicInformation, pKeyBasicInfo, ulSize, &ulSize);

			pServicesInfo = (PSERVICES_INFO)ExAllocateFromPagedLookasideList(&g_PageListServices);
			RtlZeroMemory(pServicesInfo, sizeof(SERVICES_INFO));


			//���������
			RtlCopyMemory(pServicesInfo->lpwzSrvName, pKeyBasicInfo->Name, pKeyBasicInfo->NameLength);
			KeyName.Buffer = (PWCH)ExAllocatePool(PagedPool, RegistryKeyName.Length + pKeyBasicInfo->NameLength);
			KeyName.Length = RegistryKeyName.Length + pKeyBasicInfo->NameLength;
			KeyName.MaximumLength = KeyName.Length;
			RtlZeroMemory(KeyName.Buffer, KeyName.Length);
			RtlCopyMemory(KeyName.Buffer, RegistryKeyName.Buffer, RegistryKeyName.Length);
			RtlCopyMemory((PUCHAR)KeyName.Buffer + RegistryKeyName.Length, pKeyBasicInfo->Name, pKeyBasicInfo->NameLength);
			if (!QueryServiceRunType(&KeyName, pServicesInfo))
			{
				if (NULL != pServicesInfo)
				{
					ExFreeToPagedLookasideList(&g_PageListServices, pServicesInfo);
					pServicesInfo = NULL;
				}
			}
			else
			{
				pServicesInfo->next = NULL;

				if (g_pServicesInfo == NULL)
				{
					g_pServicesInfo = pServicesInfo;
					pPreServicesInfo = pServicesInfo;
				}
				else
				{
					pPreServicesInfo->next = pServicesInfo;
					pPreServicesInfo = pServicesInfo;
				}
			}


			if (KeyName.Buffer != NULL)
			{
				ExFreePool(KeyName.Buffer);
				KeyName.Buffer = NULL;
			}

			if (pKeyBasicInfo != NULL)
			{
				ExFreePool(pKeyBasicInfo);
				pKeyBasicInfo = NULL;
			}
		}

	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		FreePagedLookasideListForServices();
		KdPrint(("Services:EnumServices failed!"));
		status = STATUS_UNSUCCESSFUL;
	}

	if (NULL != HandleRegKey)
	{
		ZwClose(HandleRegKey);
		HandleRegKey = NULL;
	}

	if (pKeyBasicInfo != NULL)
	{
		ExFreePool(pKeyBasicInfo);
		pKeyBasicInfo = NULL;
	}

	if (pKeyFullInfo != NULL)
	{
		ExFreePool(pKeyFullInfo);
		pKeyFullInfo = NULL;
	}

	return status;
}


NTSTATUS FreePagedLookasideListForServices()
{
	NTSTATUS status = STATUS_SUCCESS;
	PSERVICES_INFO pServicesInfo = NULL,\
		pNextServicesInfo = NULL;

	__try
	{

		if (g_pServicesInfo != NULL)
		{
			pServicesInfo = g_pServicesInfo;

			while (pServicesInfo != NULL)
			{
				pNextServicesInfo = pServicesInfo->next;

				ExFreeToPagedLookasideList(&g_PageListServices, pServicesInfo);

				pServicesInfo = pNextServicesInfo;
			}

			ExDeletePagedLookasideList(&g_PageListServices);
			g_pServicesInfo = NULL;
		}

	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Services:FreePagedLookasideListForDriverModule failed!"));
		status = STATUS_UNSUCCESSFUL;
	}

	return status;
}

ULONG GetServicesCount()
{
	ULONG ulCount = 0;
	PSERVICES_INFO pServicesInf = g_pServicesInfo;

	while (pServicesInf)
	{
		++ulCount;
		pServicesInf = pServicesInf->next;
	}

	KdPrint(("Services:GetServicesCount is %d", ulCount));
	return ulCount;
}

NTSTATUS GetServicesInfo(PVOID pInfo)
{
	ULONG ulLength = 0;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PSERVICES_INFO pServicesInf = g_pServicesInfo;

	__try
	{
		while (pServicesInf)
		{

			RtlCopyMemory((PUCHAR)pInfo + ulLength, pServicesInf, sizeof(SERVICES_INFO));
			ulLength += sizeof(SERVICES_INFO);
			pServicesInf = pServicesInf->next;
		}

	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		//_asm int 3
		KdPrint(("Services:GetServicesInfo failed"));
		status = STATUS_UNSUCCESSFUL;
	}

	return status;
}

BOOLEAN QueryServiceRunType( PUNICODE_STRING pSrvName, PSERVICES_INFO pServicesInfo )
{
	OBJECT_ATTRIBUTES ObjectAttributes;
	HANDLE	HandleRegKey = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG SubItemIndex, SubItemCount, ulSize, ulItemValue;
	UNICODE_STRING usItemName, usItemValue;
	PKEY_VALUE_FULL_INFORMATION pKeyFullValueInfo = NULL;
	PKEY_FULL_INFORMATION pKeyFullInfo = NULL;
	BOOLEAN bRet = FALSE;

	/************************************************************************/
	/* ServiceType �������ͣ�0��10Ϊ�������̷���0��20Ϊ������̷��񣨱���svchost����
	StartType �������ͣ�0 ϵͳ����ʱ���أ�1 OS��ʼ��ʱ���أ�2 ��SCM��������ƹ��������Զ�������3 �ֶ�������4 ���á���ע�⣬0��1ֻ��������������
	ErrorControl ������ƣ�0 ���ԣ�1 ���������棬2 �л���LastKnownGood�����ã�3 ������
	ServiceBinary �������λ�ã�%11%��ʾsystem32Ŀ¼��%10%��ʾϵͳĿ¼(WINNT��Windows)��%12%Ϊ����Ŀ¼system32\drivers��Ҳ���Բ��ñ�����ֱ��ʹ��ȫ·����
	������MentoHUST�������������ţ���Ϊ�м�ʡ����һ�������õĲ���flags��
	Description��ServiceType��StartType��ErrorControl�����Ǳ���Ҫ�еģ�����LoadOrderGroup��Dependencies�ȾͲ��������ˡ�                                                                     */
	/************************************************************************/

	__try
	{

		InitializeObjectAttributes(&ObjectAttributes, 
			pSrvName,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
			NULL,    // handle
			NULL);
		status = ZwOpenKey(&HandleRegKey, KEY_READ, &ObjectAttributes);


		// ��һ�ε�����Ϊ�˻�ȡ��Ҫ�ĳ���
		ZwQueryKey(HandleRegKey, KeyFullInformation, NULL, 0, &ulSize);
		pKeyFullInfo = (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);
		// �ڶ��ε�����Ϊ�˻�ȡ����
		ZwQueryKey(HandleRegKey, KeyFullInformation, pKeyFullInfo, ulSize, &ulSize);
		SubItemCount = pKeyFullInfo->Values;

		//ѭ��������������
		for (SubItemIndex = 0; SubItemIndex < SubItemCount; SubItemIndex++)
		{
			//_asm int 3
			ZwEnumerateValueKey(HandleRegKey, SubItemIndex, KeyValueFullInformation, NULL, 0, &ulSize);
			pKeyFullValueInfo = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);
			//��ȡ��I�����������
			status = ZwEnumerateValueKey(HandleRegKey, SubItemIndex, KeyValueFullInformation, pKeyFullValueInfo, ulSize, &ulSize);

			if (status == STATUS_SUCCESS)
			{
				usItemName.Length = usItemName.MaximumLength =  (USHORT)pKeyFullValueInfo->NameLength;
				usItemName.Buffer = pKeyFullValueInfo->Name;
				usItemValue.Length = usItemValue.MaximumLength = (USHORT)pKeyFullValueInfo->DataLength;
				usItemValue.Buffer = (PWCHAR)((PUCHAR)pKeyFullValueInfo + pKeyFullValueInfo->DataOffset);


				if (RtlCompareMemory(usItemName.Buffer, L"Type", usItemName.Length))
				{
					ulItemValue = *(PULONG)usItemValue.Buffer;
					//RtlUnicodeStringToInteger(&usItemValue, 16, &ulItemValue);
					if (0x10 != ulItemValue && 0x20 != ulItemValue && 0x110 != ulItemValue && 0x120 != ulItemValue)
					{
						bRet = FALSE;
						break;
					}
					else
					{
						bRet = TRUE;
						//_asm int 3
					}
				}
				else if (RtlCompareMemory(usItemName.Buffer, L"DisplayName", usItemName.Length) == usItemName.Length)
				{
					RtlCopyMemory(pServicesInfo->lpwzSrvName, usItemValue.Buffer, usItemValue.Length);
				}
				else if (RtlCompareMemory(usItemName.Buffer, L"ImagePath", usItemName.Length) == usItemName.Length)
				{
					RtlCopyMemory(pServicesInfo->lpwzImageName, usItemValue.Buffer, usItemValue.Length);
				}
				else if (RtlCompareMemory(usItemName.Buffer, L"Description", usItemName.Length) == usItemName.Length)
				{
					RtlCopyMemory(pServicesInfo->lpwzDescription, usItemValue.Buffer, usItemValue.Length);
				}
				else if (RtlCompareMemory(usItemName.Buffer, L"Start", usItemName.Length) == usItemName.Length)
				{
					ulItemValue = *(PULONG)usItemValue.Buffer;
					switch(ulItemValue)
					{ 
					case(SERVICE_AUTO_START):
						RtlCopyMemory(pServicesInfo->lpwzBootType, L"�Զ�", sizeof(L"�Զ�"));
						break;
					case(SERVICE_BOOT_START):
						RtlCopyMemory(pServicesInfo->lpwzBootType, L"����", sizeof(L"����"));
						break;
					case(SERVICE_DEMAND_START):
						RtlCopyMemory(pServicesInfo->lpwzBootType, L"�ֶ�", sizeof(L"�ֶ�"));
						break;
					case(SERVICE_DISABLED):
						RtlCopyMemory(pServicesInfo->lpwzBootType, L"�ѽ���", sizeof(L"�ѽ���"));
						break;
					case(SERVICE_SYSTEM_START):
						RtlCopyMemory(pServicesInfo->lpwzBootType, L"ϵͳ", sizeof(L"ϵͳ"));
						break;
					default:
						RtlCopyMemory(pServicesInfo->lpwzBootType, L"δ֪", sizeof(L"δ֪"));
						break;
					}
				}
				if (pKeyFullValueInfo->Type == REG_SZ)
				{
					KdPrint(("The sub value type:REG_SZ\n"));
				}
				else if (pKeyFullValueInfo->Type == REG_EXPAND_SZ)
				{
					KdPrint(("The sub value type:REG_EXPAND_SZ\n"));
				}
				else if (pKeyFullValueInfo->Type == REG_DWORD)
				{
					KdPrint(("The sub value type:REG_DWORD\n"));
				}
				else if (pKeyFullValueInfo->Type == REG_BINARY)
				{
					KdPrint(("The sub value type:REG_BINARY\n"));
				}
			}

			if (NULL != pKeyFullValueInfo)
			{
				ExFreePool(pKeyFullValueInfo);
				pKeyFullValueInfo = NULL;
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		bRet = FALSE;

	}

	if (NULL != pKeyFullInfo)
	{
		ExFreePool(pKeyFullInfo);
		pKeyFullInfo = NULL;
	}
	if (NULL != pKeyFullValueInfo)
	{
		ExFreePool(pKeyFullValueInfo);
		pKeyFullValueInfo = NULL;
	}

	return bRet;
}