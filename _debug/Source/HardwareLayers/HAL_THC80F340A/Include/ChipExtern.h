
#ifndef _CHIP_EXTERN_H
#define _CHIP_EXTERN_H
#include "types.h"
#include <setjmp.h>
#include "ChipConfig.h"
#include "ISO7816.h"

/*
* Macros defines
*/
#define C_WRITE_DIRECT							(0x01)
/*
* Address of 3 bytes + length of 1 byte
*/
#define TEARING_HEADER_LEN						(0x04)

/*Its relative start address after vector table*/
#define ADDRESS_CHIP_START						(PAGE_SIZE)

#define ADDRESS_CHIP_END							((FLASH_SIZE - PAGE_SIZE) - 0x01) // remove 1 page of vector table

//first page contains Vector table
#define ADDRESS_ERASE_EEPROM_START					(PAGE_SIZE)


#define mem_cpy(x,y,z) 								mem_cpyFwd(x,y,z)

/**
* Used to indicate transaction
*/
extern uMonoByte gumStartTransaction;

/*
* functions declaration
*/
extern void			Chip_THC80F340A_sendByte(uMonoByte);
extern uMonoByte	Chip_THC80F340A_sendLastByte(uMonoByte);
extern uMonoByte	Chip_THC80F340A_receiveByte(void);
extern uMonoByte	Chip_THC80F340A_WriteNBytes(uQuadByte ineeStartAddress,\
																							puMonoByte inpumbSourceBuffer,\
																							uDiByte inudbDataLength);

extern void Chip_THC80F340A_enableNullProcedureByte(void);
extern void Chip_THC80F340A_disableNullProcedureByte(void);
extern uMonoByte	ReturnToBL(void);

extern void	mem_cpyFwd(BYTE * dst,const BYTE * src,USHORT len);
extern void	mem_cpyBwd(BYTE * dst,const BYTE * src,USHORT len);
extern BYTE	mem_cmp(const BYTE * dst,const BYTE * src,USHORT len);
extern void	mem_cpy_autoadapt(BYTE * dst,const BYTE * src,HALFW len);
extern void	mem_set(BYTE * dst,BYTE value,USHORT len);

/*
* Configure Fi-Di
*
* @param umbFiDi: Frequency & Baudrate
*/
extern void Chip_THC80F340A_setFiDi(uMonoByte umbFiDi);

/*
* Send PPS response and Configure Fi-Di
*
* @param umbFiDi: Frequency & Baudrate
*/
extern void Chip_THC80F340A_sendPPSresponse(puMonoByte pumbFiDi, uMonoByte umbLen);

//Simulator jmp buf variable as it covers 1 depth jump only
extern jmp_buf simul_ExceptionHandler;
#endif