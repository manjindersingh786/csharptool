

#include	"TSIM.h"
const BYTE VectorTable[0x78] = {
0xA8, 0x11, 0x00, 0x20, 0x45, 0x00, 0x00, 0x00, 0x59, 0x00, 
0x00, 0x00, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 
0x61, 0x00, 0x00, 0x00, 0x55, 0x43, 0x05, 0x00, 0x07, 0x48, 
0x01, 0x78, 0x07, 0x4A, 0x91, 0x42, 0x02, 0xD1, 0x07, 0x48, 
0x07, 0x49, 0x01, 0x60, 0x07, 0x48, 0x00, 0x47, 0x70, 0x47, 
0x70, 0x47, 0x70, 0x47, 0x70, 0x47, 0x70, 0x47, 0x00, 0xBF, 
0x00, 0x44, 0x05, 0x00, 0xA5, 0x00, 0x00, 0x00, 0x08, 0xED, 
0x00, 0xE0, 0x00, 0x2C, 0x05, 0x00, 0xB9, 0x42, 0x05, 0x00
};
#if 0
//====================================================
//	Configure FI/DI
void	ISO7816_ConfigFIDI(BYTE FIDI)
{	
	BYTE	counter = 0;
	
	while(counter < ValidFIDI[0])
	{
		if(FIDI == ValidFIDI[counter + 1])
		{
			ISOBRC = FIDI;
			break;
		}
		else
		{
			counter ++;
		}
	}
}

//====================================================
//	PPS acknowledge
void    ISO7816_PPS_ACK(BYTE * buf)
{
	ISO7816_TxNByte(4,buf);
}

//====================================================
//	PPS
void	ISO7816_PPS(BYTE * PPSCMD)
{	
	BYTE	counter = 0;
	
	while(counter < ValidFIDI[0])	//	PCK checked before
	{
		if(PPSCMD[2] == ValidFIDI[counter + 1])
		{
			ISO7816_PPS_ACK((BYTE *)PPSCMD);
			ISOBRC = PPSCMD[2];
			break;
		}
		else
		{
			counter ++;
		}
	}
}
#endif
//====================================================
//	Receive one byte by ISO
BYTE	ISO7816_RxByte(void)
{
	while(!(ISOSTS & 0x02))			//	Waiting for ISO buffer is full
	{;}								
	return	ISOBUF;					//	Return the received byte
}

//====================================================
//	Receive n bytes by ISO
void    ISO7816_RxNByte(HALFW n,BYTE * buf)
{	
	WORD	i = 0;

	while(n > 0)
	{	
		buf[i] = ISO7816_RxByte();
		i ++;
		n --;
	}
}

//====================================================
//	Transmit one byte by ISO
void	ISO7816_TxByte(BYTE c)
{
	ISOCON |= 0x20;					//	ISO mode,Tx

	ISOBUF = c;						//	Load the transmitted byte into buffer
	while(!(ISOSTS & 0x01))			//	Until buffer is empty
	{;}

	ISOCON &= 0xDF;					//	ISO mode,Rx
}

//====================================================
//	Transmit n bytes by ISO
void 	ISO7816_TxNByte(HALFW n,BYTE * databuf)
{
	WORD	i;

	for(i = 0;i < n;i ++)
	{	
		ISO7816_TxByte(databuf[i]);
	}
}

//====================================================
//	ISO/IEC 7816 interface ISO Tx NULL byte automatically
void	ISO7816_AutoTxNULL(BYTE mode,BYTE null,HALFW ETUcount)
{
	if(mode == 1)
	{
		ISOTDAT = ISOTRLD = ETUcount;				//	Every 128-etu to send 1 NULL byte
		ISONULL = null;								//	NULL byte is 0x60
		ISOTMSK |= 0x01;							//	ISO timing to send NULL byte interrupt disabled
		ISOTCON &= 0xF9;							//	Counting mode
		ISOTCON |= 0x01;							//	Timer start
	}
	else
	{
		ISOTCON &= 0xFE;
	}
}
#if 0
//====================================================
//	Change base address of flash
void	SetBase(void)
{
	if(P3 != 0x00)
	{
		SWptr = P3ERROR;						//	6C00
	}
	else
	{
		FlashBaseAddr = Foffset * 0x10000;
	}
}
//====================================================
//	Read memory
//	Parameter: foffset=P1P2,RAMbuf=IO buffer,length=P3
void	ReadFlash(HALFW foffset,BYTE * RAMbuf,HALFW length)
{
	WORD	FlashAddr;

	if(length == 0x00)
	{
		length = 0x100;	   									//	P3 = 0x00, read 256 bytes
	}
	
	FlashAddr = FlashBaseAddr + foffset;
		
	mem_cpyFwd(RAMbuf,(BYTE *)FlashAddr,length);
}
 
//====================================================
//	Flash erase: INS = 0x2E
//	Parameter: foffset=P1P2,
void	EraseFlash(HALFW foffset,BYTE *RAMbuf)
{
	BYTE	flasherr;
	HALFW	i,sectors;
	WORD	FlashAddr;

	FlashAddr = FlashBaseAddr + foffset;		//	Start page 
	sectors = ExtractBEHalfword((BYTE *)RAMbuf);

	if(P3 != 2)
	{
		SWptr = P3ERROR;
		return;	
	}
			
	for(i = 0; i < sectors; i++)
	{		
		if((flasherr = Erase_Page(FlashAddr)) != 0x01)			
		{
			SWHandler(flasherr);
			break;
		}
		FlashAddr += PageSize;
	}	
	return;	
}

//====================================================
//	Update memory,return value: success-1,fail-0
//	Parameter: foffset=P1P2,RAMbuf=source data(IO buffer),length=P3
void	UpdateMemory(HALFW foffset,BYTE * RAMbuf,BYTE length)
{
	WORD	FlashAddr;
	HALFW	intRemain,toDo,poffset;
	BYTE    flasherr,counter = 0;

	FlashAddr = FlashBaseAddr + foffset;
	
	//***************************************************//	
	//	 update SFR and RAM
	if(((FlashBaseAddr >= SCSFRBase) && (FlashAddr < SFRLimit))|| ((FlashBaseAddr >= RAMBase) && (FlashBaseAddr < RAMLimit)))
	{
		mem_cpy_autoadapt((BYTE *)FlashAddr,RAMbuf,length);
	}
	
	//***************************************************//	
	//	update flash
	else if(IsFlash(FlashAddr) || IsOTP(FlashAddr))					//	Flash and OTP memory area
	{   					
	    FlashAddr &= (0xFFFFFE00 | PageSize); 
	    poffset = (HALFW)foffset & (PageSize - 1);
	    
		for(intRemain = length; intRemain; intRemain  -= toDo)
		{
			toDo = (poffset + intRemain) < PageSize ? intRemain : (PageSize - poffset);
						
			mem_cpyFwd(FlashBuffer,(BYTE *)FlashAddr,PageSize);
			
	
			mem_cpyFwd(&FlashBuffer[poffset],&RAMbuf[counter],toDo);
			counter += toDo;			
				
			if((flasherr = Erase_Page(FlashAddr)) == 0x01)
			{
				if((flasherr = 	Write_page((WORD)FlashAddr, FlashBuffer)) != 0x01)
				{	
					SWHandler(flasherr);
					return;
				}
			}
			else
			{
				SWHandler(flasherr);
				return;
			}
				
			poffset = (HALFW)(toDo + poffset) & (PageSize - 1);
			FlashAddr += PageSize;
		}
	}
	else
	{
		SWptr = P1P2ERR;						//	6A00
	}
}
//====================================================
//	Flash write: INS = 0x58
//	Parameter: foffset=P1P2,0x80=P3
void	WriteFlash(HALFW foffset,BYTE * RAMbuf,BYTE length)
{
	WORD	FlashAddr;
	BYTE	flasherr;

	FlashAddr = FlashBaseAddr + foffset;

	if((flasherr = Write_Bytes((BYTE *)FlashAddr,RAMbuf,length)) != 0x01)
	{	
		SWHandler(flasherr);
	}
}

//====================================================

//	Compute CRC by hardware
HALFW	calCRC(WORD tgtaddr, WORD length)
{
	USHORT sResult;
	CRCCON0 = 0x00;
	
	while(length != 0)
	{
		CRCDAT = *(BYTE *)tgtaddr;
		tgtaddr ++;
		length --;
	}

	sResult = CRCDAT;
	sResult = (sResult << 8) + CRCDAT;

	return sResult;
}

//====================================================
//	Generate random number
void	RandomNumGen(BYTE * rngbuf,USHORT length)
{
	RNGCON &= Bit0_Dis;						//	Open RNG
	
	if (length == 0x00)
	{
		length = 0x100;
	}
	while(length > 0)					//	Length could be 3,2,or 1 byte(s)
	{
		while(!(RNGSTS & Bit0_En))
		{;}
		*rngbuf++ = (BYTE)RNGDAT;
		length--;
	}
	
	RNGCON |= Bit0_En;					//	Close RNG	 
}

#endif
//====================================================
uMonoByte	ReturnToBL(void)
{
	SCINTEN1 = SCINTEN2 = 0;
	
	if((0x01 != Erase_Page(0x0000)) || (0x01 != (Write_Bytes((BYTE *)(WORD)0x0000,VectorTable,0x78)))){
		return 1;
	}
	return 0;
}
