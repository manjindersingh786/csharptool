#ifndef	__ISO7816_H__
#define __ISO7816_H__
#include "SC_SFR.h"
//	Define INS
#define	GETMEMORY	0x5A	//	Read flash
#define	PUTMEMORY	0x5C	//	Update flash
#define	ERFLASH		0x2E	//	Erase flash
#define	WRFLASH		0x58	//	Write flash
#define	SETBASE		0x50	//	Change base address of flash
#define	GETRDMN		0x84	//	Get Random Number
#define	CHECKCRC	0x30	//	Check by CRC
#define RETURNBL	0xF0    //	Return to BL
#define GETMEM		0xF2    //	Get memory layout

//	Define Status Word
#define	SUCCESS	0		//	Success,9000
#define	IVDINS	2		//	Invalid INS,6D00
#define	P3ERROR	4		//	P3 error,6C00
#define	WRERROR	6		//	Flash write error,6501
#define	P1P2ERR	8		//	P1 and/or P2 error,6A00
#define	RDERROR	10		//	Read NAND Flash error
#define	IVDCLA	12		//	Invalid instruction class

#define	FlashStart	0x00000
#define	FlashLimit	0x55000
#define	OTPStart	0x8000000
#define	OTPLimit	0x8000400
#define	RAMBase		0x20000000
#define	RAMLimit	0x20002A00
#define	SCSFRBase	0x40000000
#define	SFRLimit    0x60000000
#define	PageSize	0x200
#define RowSize		0x80

#define	TX_NULL_BYTE_ON		ISO7816_AutoTxNULL(1,0x60,8000 - 1);
#define	TX_NULL_BYTE_OFF	ISO7816_AutoTxNULL(0,0x60,8000 - 1);

//	Global Variables
extern 	BYTE	CMD_Buf[5];
#define	CLA	CMD_Buf[0]
#define	INS	CMD_Buf[1]
#define	P1	CMD_Buf[2]
#define	P2	CMD_Buf[3]
#define	P3	CMD_Buf[4]

extern 	BYTE    PPSFlag;
extern 	HALFW	SWptr;
extern 	HALFW	Foffset;
extern 	WORD    FlashBaseAddr;
extern	const	BYTE	ValidFIDI[];
extern	const 	BYTE	ATR[11];
extern	const 	BYTE	CHIP_SW[14];
extern 	BYTE	IOBuf[256];
extern 	BYTE	TEMP1[9200];

void	ISO7816_main(void);
void	SWHandler(BYTE err);

void	ISO7816_ConfigFIDI(BYTE FIDI);
void    ISO7816_PPS_ACK(BYTE * buf);
void	ISO7816_PPS(BYTE * PPSCMD);
BYTE	ISO7816_RxByte(void);
void    ISO7816_RxNByte(unsigned short n,BYTE * buf);
void	ISO7816_TxByte(BYTE c);
void 	ISO7816_TxNByte(unsigned short n,BYTE * databuf);
void	ISO7816_AutoTxNULL(BYTE mode,BYTE null,unsigned short ETUcount);

void	SendATR(void);
BYTE	RcvAPDU(void);
void	CMD_Handle(void);
void	response(void);
void	TxStatus(int ptr);
void	SetBase(void);
void	WriteFlash(HALFW foffset,BYTE * RAMbuf,BYTE length);
void	UpdateMemory(HALFW foffset,BYTE * RAMbuf,BYTE length);
void	EraseFlash(HALFW foffset,BYTE *RAMbuf);
HALFW	calCRC(WORD tgtaddr, WORD length);
void	ReadFlash(HALFW foffset,BYTE * RAMbuf,HALFW length);
void	RandomNumGen(BYTE * rngbuf,USHORT length);
BYTE	ReturnToBL(void);
void	ETU_Delay(HALFW ETUcount);

#endif




