#ifndef _SHADOW_SSDT_HEADER
#define _SHADOW_SSDT_HEADER
#ifdef __cplusplus
extern "C"
{
#endif
#include "DataDef.h"
#include "Services.h"
#include "DriverModule.h"
#ifdef __cplusplus
}
#endif

EXTERN_C PVOID GetKeServiceDescriptorTableShadow();
EXTERN_C ULONG GetShadowSSDTCount();
EXTERN_C VOID SetShadowSSDT(PVOID pInfo);
EXTERN_C NTSTATUS EnumShadowSSDT(PVOID pOuputBuufer);
EXTERN_C NTSTATUS FreePagedLookasideListForShadowSSDT();


NTSTATUS GetShadowSSDTFuncInfo(PVOID pInfo);

//���win32k��ַ
ULONG GetWin32kBase();

//���ݷ���Ż��ԭʼ������ַ
ULONG GetOldShadowSSDTAddress(ULONG ulIndex);


ULONG GetOriginalSSDTShadowAddress(ULONG ulSSDTCount);


#endif
