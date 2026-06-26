#ifndef __FLASH_H
#define __FLASH_H


#define 	KP1_Addr  	0x800ec00
#define 	KI1_Addr   	0x800f000
#define 	KD1_Addr   	0x800f400
#define 	KP2_Addr  	0x800f800
#define 	KD2_Addr  	0x800fc00



float Read_FLASH(uint32_t Addr);
void Write_FlASH(uint32_t Addr,float data);




#endif
