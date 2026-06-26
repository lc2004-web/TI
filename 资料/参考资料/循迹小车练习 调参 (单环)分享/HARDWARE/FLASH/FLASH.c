#include "stm32f10x.h"                  // Device header


float Read_FLASH(uint32_t Addr)
{
	return *((uint32_t *)Addr);
}

void Write_FlASH(uint32_t Addr,float data)
{
	FLASH_Unlock();
	FLASH_ErasePage(Addr);
	FLASH_ProgramWord(Addr,data);
	FLASH_Lock();
}
