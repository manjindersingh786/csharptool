#ifndef _WRAPPERS_EXTERN_H
	#define _WRAPPERS_EXTERN_H

#include "types.h"
#ifdef WIN32
#include "simulExtern.h"
#else
#include "ChipExtern.h"
#endif
#include "AntiTearing_extern.h"

#ifdef _CHIP_THC80F340A
#define baseReadOneByteDirect(x)	((*(uMonoByte*)(x)))	
#else
#define baseReadOneByteDirect(x)	(Simul_ReadByte(x))
#endif

/*Read direct from the given address*/
extern void baseReadNBytesDirect(uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength);

/*Write direct on the given address*/
extern boolean baseWriteNBytesDirect(uQuadByte uqbAddress, puMonoByte pumbInput, uDiByte udbLength);

#ifdef FEAT_EXHAUSTEARING
/*
* baseReadNBytes (uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength)
* 
* @param : uqbAddress	: Memory Address To Read From
* @param : pumbOutput	: Pointer To The Buffer Where data Will Be Stored
* @param : udbLength	: How Much Data
*/
extern void baseReadNBytes(uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength);
	
/*
* baseReadOneByte (uQuadByte uqbAddress)
* 
* @param : uqbAddress	: Memory Address To Read From
* @ret: read byte value
*/
extern uMonoByte baseReadOneByte(uQuadByte uqbAddress);
	
/*
* baseWriteNBytes(uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength)
* 
* @param : uqbAddress	: Memory Address write to
* @param : pumbOutput	: Pointer To The Buffer Where data Will Be Stored
* @param : udbLength	: How Much Data
*/
extern void baseWriteNBytes(uQuadByte uqbAddress, puMonoByte pumbInput, uDiByte udbLength);

/*
* Below macro initalizes the page table and recover the data if tear was happened in last card session
*/
#define baseNvmInit()				at_osInit()
#else

/*
* baseReadNBytes (uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength)
*
* @param : uqbAddress	: Memory Address To Read From
* @param : pumbOutput	: Pointer To The Buffer Where data Will Be Stored
* @param : udbLength	: How Much Data
*/
#define baseReadNBytes(src,dest,len)				(baseReadNBytesDirect((src), (dest),(len)))

/*
* baseReadOneByte (uQuadByte uqbAddress)
*
* @param : uqbAddress	: Memory Address To Read From
* @ret: read byte value
*/
#ifdef _CHIP_THC80F340A
#define baseReadOneByte(src)						(*((puMonoByte)(src)))
#else
#define baseReadOneByte(src)						(*((puMonoByte)(SimulGetPhysicalAddr((uQuadByte)(src)))))
#endif

/*
* baseWriteNBytes(uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength)
*
* @param : uqbAddress	: Memory Address write to
* @param : pumbOutput	: Pointer To The Buffer Where data Will Be Stored
* @param : udbLength	: How Much Data
*/
#define baseWriteNBytes(dest,src,len)				(baseWriteNBytesDirect((dest), (src), (len)))

/*Dummy function of Exhaustearing*/
#define baseNvmInit()								((void) 0) // no action

#endif
#ifdef _CHIP_THC80F340A
	#define baseReadFileHandler(ineeStartAddress, inpStructAddress, inumbStructLength) \
				Chip_THC80F340A_ReadFileHandler(ineeStartAddress, inpStructAddress, inumbStructLength)
	#define baseWriteFileHandler(ineeStartAddress, inpStructAddress, inumbStructLength) \
				Chip_THC80F340A_WriteFileHandler(ineeStartAddress, inpStructAddress, inumbStructLength)
	#define baseWriteNBytesNonAtomic(ineeStartAddress, inpumbSourceBuffer, inudbDataLength) \
				Chip_THC80F340A_WriteNBytesNonAtomic(ineeStartAddress, inpumbSourceBuffer, inudbDataLength)
	#define baseWriteNBytesFromNVM(ineeDestAddress, ineeSourceAddress, inudbDataLength, umbWriteMode) \
				Chip_THC80F340A_WriteNBytesFromNVM(ineeDestAddress, ineeSourceAddress, inudbDataLength, umbWriteMode)
	#define baseBackupToPageAT(ineeDestAddress, ineeSourceAddress, inudbDataLength) \
				Chip_THC80F340A_BackupToPageAT(ineeDestAddress, ineeSourceAddress, inudbDataLength)
	#define baseWriteFromPageAT() \
				Chip_THC80F340A_WriteFromPageAT()
	#define SendByteToID(value) \
				Chip_THC80F340A_sendByte(value)
	#define SendLastByteToID(value) \
				Chip_THC80F340A_sendLastByte(value)
	#define receiveByte()	Chip_THC80F340A_receiveByte()
	#define setFiDi(x)		Chip_THC80F340A_setFiDi(x)
	#define sendPPSresponse(x,y)			Chip_THC80F340A_sendPPSresponse(x,y)
	#define enableNullProcedure()			Chip_THC80F340A_enableNullProcedureByte()
	#define disableNullProcedure()		Chip_THC80F340A_disableNullProcedureByte()
	#define switchToBootLoader()			ReturnToBL()
	#define erasePage(x)				Chip_THC80F340A_ErasePage(x)
	#define writePage(x,y)				Chip_THC80F340A_WritePage(x,y)
#else
	#define baseReadFileHandler(ineeStartAddress, inpStructAddress, inumbStructLength) \
				Simul_ReadFileHandler(ineeStartAddress, inpStructAddress, inumbStructLength)
	#define baseWriteFileHandler(ineeStartAddress, inpStructAddress, inumbStructLength) \
				Simul_WriteFileHandler(ineeStartAddress, inpStructAddress, inumbStructLength)
	#define baseWriteNBytesNonAtomic(ineeStartAddress, inpumbSourceBuffer, inudbDataLength) \
				Simul_WriteNBytesNonAtomic(ineeStartAddress, inpumbSourceBuffer, inudbDataLength)
	#define baseWriteNBytesFromNVM(ineeDestAddress, ineeSourceAddress, inudbDataLength, umbWriteMode) \
				Simul_WriteNBytesFromNVM(ineeDestAddress, ineeSourceAddress, inudbDataLength, umbWriteMode)
	#define baseBackupToPageAT(ineeDestAddress, ineeSourceAddress, inudbDataLength) \
				Simul_BackupToPageAT(ineeDestAddress, ineeSourceAddress, inudbDataLength)
	#define baseWriteFromPageAT() \
				Simul_WriteFromPageAT()
	#define SendByteToID(value) \
				Simul_sendByte(value)
	#define SendLastByteToID(value) \
				Simul_sendLastByte(value)
	#define receiveByte()	Simul_receiveByte()
	#define setFiDi(x)			((void) 0) // no action
	#define sendPPSresponse(x,y)		((void) 0) // no action
	#define enableNullProcedure()			((void) 0) // no action
	#define disableNullProcedure()		((void) 0) // no action
	#define switchToBootLoader()			((void) 0) // no action
	#define erasePage(x)				Chip_THC80F340A_ErasePage(x)
	#define writePage(x,y)				Chip_THC80F340A_WritePage(x,y)
#endif

#endif
