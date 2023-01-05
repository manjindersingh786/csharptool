#include "types.h"
#include <setjmp.h>
#include "SimulConfig.h"
#include <memory.h>
#ifndef _SIMUL_EXTERN_H
#define _SIMUL_EXTERN_H
/*
* Macros defines
*/
#define C_WRITE_DIRECT							(0x01)
/*
* Address of 3 bytes + length of 1 byte
*/
#define TEARING_HEADER_LEN						(0x04)

/*Simulator start address*/
#define ADDRESS_SIMUL_START						(PAGE_SIZE)

#define ADDRESS_SIMUL_END						((FLASH_SIZE - (PAGE_SIZE)) - 0x01) // 1 page for vector table removed



#define mem_cpy(x,y,z) 								mem_cpyFwd(x,y,z)

/**
* Used to indicate transaction
*/
extern uMonoByte gumStartTransaction;

/*
* functions declaration
*/
extern void			Simul_sendByte(uMonoByte);
extern uMonoByte	Simul_sendLastByte(uMonoByte);
extern uMonoByte	Simul_receiveByte(void);
extern uMonoByte	Simul_WriteNBytes(uQuadByte ineeStartAddress,\
									puMonoByte inpumbSourceBuffer,\
									uDiByte inudbDataLength);

extern boolean Simul_ErasePage(uQuadByte ineePageStartAddress);

extern uMonoByte Simul_WriteNBytesDirect(uQuadByte ineeStartAddress, puMonoByte inpumbSourceBuffer, uDiByte inudbDataLength);
extern uMonoByte Simul_ReadByte(uQuadByte uqStartAddress);
extern uQuadByte SimulGetPhysicalAddr(uQuadByte uqLogicalAddr);
extern void Simul_ReadNBytesDirect(uQuadByte uqStartAddress, puMonoByte pumbOutput, uDiByte udbLen);

extern void	mem_cpyFwd(uMonoByte* dst, const uMonoByte* src, uDiByte len);
extern void	mem_cpyBwd(uMonoByte* dst, const uMonoByte* src, uDiByte len);
extern uMonoByte	mem_cmp(const uMonoByte* dst, const uMonoByte* src, uDiByte len);
extern void	mem_set(uMonoByte* dst, uMonoByte value, uDiByte len);

//Simulator jmp buf variable as it covers 1 depth jump only
extern jmp_buf simul_ExceptionHandler;
#endif