#ifndef __SC_SFR_H__
#define __SC_SFR_H__

//	Declare all of the registers provided with THC8000 chips

typedef	unsigned int	WORD;
typedef	unsigned short	HALFW;
typedef	unsigned char	BYTE;
typedef	unsigned short	USHORT;
typedef	unsigned int	UINT;

///******************System Control Unit******************/
//	SC	
#define	reg_base_scu	0x40000000
#define	SCSLEEP		*(volatile WORD *)(reg_base_scu + 0x000)		//	Idle-entry control register
#define	SCSYS	    *(volatile WORD *)(reg_base_scu + 0x010)		//	Clock selection for system
#define	SCRSTEN0	*(volatile WORD *)(reg_base_scu + 0x0A0)		//	Reset enable register
#define	SCRSTFLG0	*(volatile WORD *)(reg_base_scu + 0x0B0)		//	Reset flag register
#define	SCRSTFLG1	*(volatile WORD *)(reg_base_scu + 0x0B4)		//	Reset flag register
#define SCINTEN1    *(volatile WORD *)(reg_base_scu + 0x104)  		//  Interrupt enable register
#define SCINTEN2    *(volatile WORD *)(reg_base_scu + 0x108)  		//  Interrupt enable register
#define SCINTSTS1 	*(volatile WORD *)(reg_base_scu + 0x204)  		//   Interrupt status register
#define SCINTSTS2 	*(volatile WORD *)(reg_base_scu + 0x208)   		//   Interrupt status register

//	TIMER
#define	reg_base_timer	0x40003000
#define	TL0			*(volatile WORD *)(reg_base_timer + 0x000)	//	Timer0 low byte register
#define	TH0			*(volatile WORD *)(reg_base_timer + 0x004)	//	Timer0 high byte register
#define	TL1			*(volatile WORD *)(reg_base_timer + 0x008)	//	Timer1 low byte  register
#define	TH1			*(volatile WORD *)(reg_base_timer + 0x00c)	//	Timer1 high byte register
#define	TCON		*(volatile WORD *)(reg_base_timer + 0x018)	//	Timer0/1 control register
#define	TMOD		*(volatile WORD *)(reg_base_timer + 0x01c)	//	Timer0/1 mode select register

/******************Memory Control Unit******************/
//	NOR FLASH		
#define	reg_base_nor 	0x40020000
#define	FLSTS		*(volatile WORD *)(reg_base_nor+ 0x000)		//	Flash status register
#define	FLSDP1		*(volatile WORD *)(reg_base_nor+ 0x004)		//	Flash software protect register1
#define	FLSDP2		*(volatile WORD *)(reg_base_nor+ 0x008)		//	Flash software protect register2
#define	FLCON		*(volatile WORD *)(reg_base_nor+ 0x00C)		//	Flash control register
#define	FLPCON		*(volatile WORD *)(reg_base_nor+ 0x010)		//	Flash power control register
#define	FLRPVAD	    *(volatile WORD *)(reg_base_nor+ 0x020)	
#define	FLRPVLEN 	*(volatile WORD *)(reg_base_nor+ 0x024)	


///******************Encoder Control Unit******************/
//	DES

//	CRC
#define	reg_base_crc	0x40041000
#define	CRCDAT		*(volatile WORD *)(reg_base_crc + 0x000)	//	CRC data register
#define	CRCCON0		*(volatile WORD *)(reg_base_crc + 0x004)	//	CRC init value config register
#define	CRCCON1		*(volatile WORD *)(reg_base_crc + 0x008)	//	CRC arith config register

//	VD
#define	reg_base_vd		0x40060000	
#define	VDCON		*(volatile WORD *)(reg_base_vd + 0x000)		//	VD control register
#define	VDSTS		*(volatile WORD *)(reg_base_vd + 0x004)		//	VD status register

//	FD
#define	reg_base_fd		0x40061000
#define	FDCON		*(volatile WORD *)(reg_base_fd + 0x000)		//	FD control register
#define	FDSTS		*(volatile WORD *)(reg_base_fd + 0x004)		//	FD status register

//	RNG
#define	reg_base_rng	0x40065000
#define	RNGCON		*(volatile WORD *)(reg_base_rng + 0x000)	//	RNG control register
#define	RNGDAT		*(volatile WORD *)(reg_base_rng + 0x004)	//	RNG data register
#define	RNGSTS		*(volatile WORD *)(reg_base_rng + 0x008)	//	RNG status register

/******************Interface Control Unit******************/
//	ISO
#define	reg_base_uart	0x40080000
#define	ISOCON			*(volatile WORD *)(reg_base_uart + 0x000)	//	ISO control register	
#define	ISOCON1			*(volatile WORD *)(reg_base_uart + 0x004)	//	ISO control register 1
#define	ISOCON2			*(volatile WORD *)(reg_base_uart + 0x008)	//	ISO control register 2
#define	ISOSTS			*(volatile WORD *)(reg_base_uart + 0x00C)	//	ISO status register
#define	ISOBRC  		*(volatile WORD *)(reg_base_uart + 0x010)	//	ISO baut rate register
#define	ISOBUF			*(volatile WORD *)(reg_base_uart + 0x014)	//	ISO buffer register
#define	ISODIO			*(volatile WORD *)(reg_base_uart + 0x018)	//	ISO direct IO register
#define	ISOMSK			*(volatile WORD *)(reg_base_uart + 0x01C)	//	ISO mask register
#define	ISODMACON		*(volatile WORD *)(reg_base_uart + 0x030)	//	ISO DMA control register
#define	ISODMASTS		*(volatile WORD *)(reg_base_uart + 0x034)	//	ISO DMA status register
#define	ISODMABFAD		*(volatile WORD *)(reg_base_uart + 0x038)	//	ISO DMA buffer start address register1
#define	ISODMABFLEN		*(volatile WORD *)(reg_base_uart + 0x03C)	//	ISO DMA buffer length register
#define	ISODMABFPT		*(volatile WORD *)(reg_base_uart + 0x040)	//	ISO DMA buffer pointer register
#define	ISODMAMSK		*(volatile WORD *)(reg_base_uart + 0x044)	//	ISO DAM interrupt mask register
#define	ISOTCON			*(volatile WORD *)(reg_base_uart + 0x050)	//	ISO Timer control register
#define	ISOTDAT			*(volatile WORD *)(reg_base_uart + 0x054)	//	ISO Timer data register
#define	ISOTRLD			*(volatile WORD *)(reg_base_uart + 0x058)	//	ISO Timer reload data register
#define	ISOTMSK			*(volatile WORD *)(reg_base_uart + 0x05C)	//	ISO Timer interrupt mask register
#define	ISONULL			*(volatile WORD *)(reg_base_uart + 0x060)	//	ISO auto transmit null character
#endif
