#include <ntddk.h>
#include <wdm.h>
#include "uninst_360.h"

int Delete360Reg()
{
	return 0;
}
int Delete360File()
{
	return 0;
}
int Kill360Service()
{
	DbgPrint("Kill360Service Begin");
	return 0;
}