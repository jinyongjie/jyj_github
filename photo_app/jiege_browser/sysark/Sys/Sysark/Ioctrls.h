#ifndef IOCTLS_H
#define IOCTLS_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <NTIFS.h>
#ifdef __cplusplus
}
#endif 

#ifndef CTL_CODE
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
	((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
	)
#pragma message("CTL_CODE undefined. Include winioctl.h or ntddk.h")
#endif

#define	FILE_DRIVER_SYSARK	0x0000420
#define IOCTL_FUNCTION_BASE 0X8000
#define	SystemModuleInfo	0x0B

#define DEVICE_NAME			L"\\Device\\SysarkDriver"
#define SYMLINK_NAME		L"\\DosDevices\\SysarkDriver"
#define DRIVER_PATH			"Sysark.sys"
#define CONFIG_FILE_NAME	"SysarkConf.xml"


//////////////////////////////////////////////////////////////////////////
//IO ������

#define CTL_CODE_SYSARK(i) CTL_CODE(FILE_DRIVER_SYSARK, IOCTL_FUNCTION_BASE+i, METHOD_IN_DIRECT, FILE_ANY_ACCESS)


//��NtQuerySystemInformation��ַ��������
#define IOCTL_QSIADDR			(ULONG)CTL_CODE_SYSARK(5)

//��NtDeviceIoControlFile��ַ��������
#define IOCTL_DICFADDR			(ULONG)CTL_CODE_SYSARK(6)

//ǿɱ����
#define IOCTL_KILL_PROCESS		(ULONG)CTL_CODE_SYSARK(7)

//����ע�����
#define IOCTL_REG_PROTECTION	(ULONG)CTL_CODE_SYSARK(8)

//ֹͣע�����
#define IOCTL_STOP_PROTECTION   (ULONG)CTL_CODE_SYSARK(9)

//���¼���������
#define IOCTL_SAVE_EVENT		(ULONG)CTL_CODE_SYSARK(10)

//���ע�����Ϣ
#define IOCTL_REGISTRY_INFO		(ULONG)CTL_CODE_SYSARK(11)

//�����޸�
#define IOCTL_ALLOW_MODIFY		(ULONG)CTL_CODE_SYSARK(12)

//ö��TCP
#define IOCTL_ENUMTCP			(ULONG)CTL_CODE_SYSARK(13)

//ö��UDP
#define IOCTL_ENUMUDP			(ULONG)CTL_CODE_SYSARK(14)

#define IOCTL_HOSTZHEN_OPERATION  (ULONG)CTL_CODE_SYSARK(15)

#define IOCTL_ENUM_PROCESS		(ULONG)CTL_CODE_SYSARK(16)//��ȡ�����б�

#define IOCTL_ENUM_PROCESS_THREAD  (ULONG)CTL_CODE_SYSARK(17)//��ȡ�߳��б�

#define IOCTL_ENUM_PROCESS_MODULE  (ULONG)CTL_CODE_SYSARK(18)//��ȡ����ģ���б�




//ɾ���ļ�
#define IOCTL_DELETE_FILE		(ULONG)CTL_CODE_SYSARK(19)


#define IOCTL_GET_DRIVER_MODULE_INFO_SIZE				(ULONG)CTL_CODE_SYSARK(20)

#define IOCTL_ENUM_DRIVER_MODULE						(ULONG)CTL_CODE_SYSARK(21)

#define IOCTL_GET_SERVICES_COUNT						(ULONG)CTL_CODE_SYSARK(22)

#define IOCTL_ENUM_SERVICES								(ULONG)CTL_CODE_SYSARK(23)

#define IOCTL_GET_STARTUP_INFO_COUNT					(ULONG)CTL_CODE_SYSARK(24)

#define IOCTL_ENUM_STARTUP								(ULONG)CTL_CODE_SYSARK(25)

//��ȡSSDT�ṹ
#define IOCTL_GET_SSDT_COUNT							(ULONG)CTL_CODE_SYSARK(26)

#define IOCTL_ENUM_SSDT									(ULONG)CTL_CODE_SYSARK(27)

#define IOCTL_SET_SSDT									(ULONG)CTL_CODE_SYSARK(28)

//��ѯSSDT HOOK������ַ
#define IOCTL_GET_SHADOW_HOOK_COUNT						(ULONG)CTL_CODE_SYSARK(29)

#define IOCTL_ENUM_SHADOW_HOOK							(ULONG)CTL_CODE_SYSARK(30)

#define IOCTL_SET_SHADOW_HOOK							(ULONG)CTL_CODE_SYSARK(31)

//��ѯĿ¼��Ϣ
#define IOCTL_GET_DIRECTORY_INFO_COUNT					(ULONG)CTL_CODE_SYSARK(32)

#define IOCTL_ENUM_DIRECTORY_INFO						(ULONG)CTL_CODE_SYSARK(33)

//�޸��ļ�Ȩ��
#define IOCTL_MODIFY_FILE_RIGHT							(ULONG)CTL_CODE_SYSARK(34)

#endif
