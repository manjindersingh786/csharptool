#ifndef	__TSIM_H__
#define	__TSIM_H__
typedef	void (* voidfunc) (void);

#include	"SC_SFR.h"
#include	"ISO7816.h"
#include	"NORFlash.h"
#include "arm_acle.h"
#include "types.h"


#define	Bit0_En		0x1
#define	Bit1_En		0x1<<1
#define	Bit2_En		0x1<<2
#define	Bit3_En		0x1<<3
#define	Bit4_En		0x1<<4
#define	Bit5_En		0x1<<5
#define	Bit6_En		0x1<<6
#define	Bit7_En		0x1<<7
#define	Bit8_En		0x1<<8
#define	Bit9_En		0x1<<9
#define	Bit10_En	0x1<<10
#define	Bit11_En	0x1<<11
#define	Bit12_En	0x1<<12
#define	Bit13_En	0x1<<13
#define	Bit14_En	0x1<<14
#define	Bit15_En	0x1<<15
#define	Bit16_En	0x1<<16
#define	Bit17_En	0x1<<17
#define	Bit18_En	0x1<<18
#define	Bit19_En	0x1<<19
#define	Bit20_En	0x1<<20
#define	Bit21_En	0x1<<21
#define	Bit22_En	0x1<<22
#define	Bit23_En	0x1<<23
#define	Bit24_En	0x1<<24
#define	Bit25_En	0x1<<25
#define	Bit26_En	0x1<<26
#define	Bit27_En	0x1<<27
#define	Bit28_En	0x1<<28
#define	Bit29_En	0x1<<29
#define	Bit30_En	0x1<<30
#define	Bit31_En	0x1<<31

#define	Bit0_Dis	~(0x1)
#define	Bit1_Dis	~(0x1<<1)
#define	Bit2_Dis	~(0x1<<2)
#define	Bit3_Dis	~(0x1<<3)
#define	Bit4_Dis	~(0x1<<4)
#define	Bit5_Dis	~(0x1<<5)
#define	Bit6_Dis	~(0x1<<6)
#define	Bit7_Dis	~(0x1<<7)
#define	Bit8_Dis	~(0x1<<8)
#define	Bit9_Dis	~(0x1<<9)
#define	Bit10_Dis	~(0x1<<10)
#define	Bit11_Dis	~(0x1<<11)
#define	Bit12_Dis	~(0x1<<12)
#define	Bit13_Dis	~(0x1<<13)
#define	Bit14_Dis	~(0x1<<14)
#define	Bit15_Dis	~(0x1<<15)
#define	Bit16_Dis	~(0x1<<16)
#define	Bit17_Dis	~(0x1<<17)
#define	Bit18_Dis	~(0x1<<18)
#define	Bit19_Dis	~(0x1<<19)
#define	Bit20_Dis	~(0x1<<20)
#define	Bit21_Dis	~(0x1<<21)
#define	Bit22_Dis	~(0x1<<22)
#define	Bit23_Dis	~(0x1<<23)
#define	Bit24_Dis	~(0x1<<24)
#define	Bit25_Dis	~(0x1<<25)
#define	Bit26_Dis	~(0x1<<26)
#define	Bit27_Dis	~(0x1<<27)
#define	Bit28_Dis	~(0x1<<28)
#define	Bit29_Dis	~(0x1<<29)
#define	Bit30_Dis	~(0x1<<30)
#define	Bit31_Dis	~(0x1<<31)

#define	__align(x)	


//	Global Variables
extern	BYTE	ISO7816_Time;
extern const	WORD	TestFlag;

//	Functions of Main.c
void	Testmain(void);

//	Functions of SC_API.c
void	HWInit(void);
void	sleep_mode(void);

//	Functions of Memory_API.c
void	TransitBigLittle(BYTE * buffer);
void	TransitBigLittleHW(BYTE * buffer);
WORD	ExtractBEWord(BYTE * buffer);
HALFW	ExtractBEHalfword(BYTE * buffer);
BYTE	IsFlash(WORD addr);
BYTE	IsOTP(WORD addr);


void	TMR_ISR(void);
void	FLASH_ISR(void);
#endif
