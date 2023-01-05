#ifndef _CHIP_CONFIG_H
#define _CHIP_CONFIG_H

#include "types.h"

/*
* Macros
*/
#define FLASH_SIZE							(0x55000) //340K



#define	FLASH_START						(0x00000)
#define	FLASH_LIMIT						(0x55000)
#define	OTP_START							(0x8000000)
#define	OTP_LIMIT							(0x8000400)
#define	RAM_BASE							(0x20000000)
#define	RAM_LIMIT							(0x20002A00)
#define	SCSFR_BASE						(0x40000000)
#define	SFR_LIMIT    					(0x60000000)
#define	PAGE_SIZE							(0x200)
#define ROW_SIZE							(0x80)


#endif // !_CHIP_CONFIG_H
