#include "types.h"
#ifndef _SIMUL_CONFIG_H
#define _SIMUL_CONFIG_H

/*
* Macros
*/
#define FLASH_SIZE						(0x55000) //340K
#define ADDRESS_ERASE_EEPROM_START		((uQuadByte)((puMonoByte) umbEEPRom))
#define AT_BUFF_MAX_SIZE				(0x6A4)

//Page size 256 bytes
#define PAGE_SIZE						(0x200)
/*
* Simul Variables Declaration
*/

/*
* It simulates EEPROM area of card
*/
extern uMonoByte umbEEPRom[];
extern uMonoByte dataYes;

#endif // !_SIMUL_CONFIG_H
