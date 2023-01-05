#include	"TSIM.h"
__align(4)	BYTE	FlashBuffer[PageSize];

//====================================================
//	Page Erase 
//	Return value:	0x01 - success,0x02 - operation error,0x04 - verification failed
BYTE	Erase_Page(WORD tgtaddr)
{
	FLCON = 0x11;
	
//	Erase the target sector	
	FLSDP1 = 0x55;
	FLSDP2 = 0xAA;	
		__asm (
      "DSB"
	);
	*(BYTE *)tgtaddr = 0xFF;		//	Write a FFH to any address in the target sector	
	
	while(!(FLSTS & Bit0_En))		//	Waiting for erase complete
	{;}
	FLSTS &= Bit0_En;

	if(FLSTS & Bit1_En)				//	Mistaken operation,FLSTS.OVER will not be set if this error happens
	{	
		FLSTS &= Bit1_En;			//	Clear this bit
		return	0x02;
	}
	if(FLSTS & Bit2_En)            
	{
		FLSTS &= Bit2_En;
		return  0x04;
	}

	return	0x01;		
}


//====================================================
//	Write BYTE.
//	Return value:	0x01 - success,0x02 - operation error,0x04 - verification failed
BYTE	Write_Bytes(BYTE * pDest,BYTE * pSrc,HALFW len)
{
	HALFW	i;

 	for(i = 0;i < len;i ++)
	{
		FLCON = 0x01;
		FLSDP1 = 0xAA;
		FLSDP2 = 0x55;	
		__asm (
      "DSB"
	);
		*(pDest + i) = pSrc[i];

		while(!(FLSTS & Bit0_En));			//	FL_CTL.FL_OVER		
		FLSTS &= Bit0_En;					//	Clear FLSTS.OVER

		if(FLSTS & Bit1_En)					//	Mistaken operation
		{							   	
			FLSTS &= Bit1_En;
			return	0x02;
		}
	}
	if(mem_cmp((BYTE *)pDest,pSrc,len) == 0)
	{
		return	0x04;						//	Write verification failed
	}
	return 0x01;
}
//====================================================
BYTE	Write_ROW_Bytes(BYTE * pDest,BYTE * pSrc,HALFW len)
{
	FLRPVAD = (WORD)pSrc;
	FLRPVLEN = len;
	FLCON = (Bit6_En | Bit0_En);
	FLSDP1 = 0xAA;
	FLSDP2 = 0x55;
		__asm (
      "DSB"
	);

	*pDest = 0xFF;

 
	while(!(FLSTS & Bit0_En));			//	FL_CTL.FL_OVER		
	FLSTS &= Bit0_En;					//	Clear FLSTS.OVER

	if(FLSTS & Bit1_En)					//	Mistaken operation
	{							   	
		FLSTS &= Bit1_En;
		return	0x02;
	}
	if(mem_cmp((BYTE *)pDest,pSrc,len) == 0)
	{
		return	0x04;						//	Write verification failed
	}
	return 0x01;
}
//===================================================
BYTE	Write_page(WORD FlashAddr,BYTE * pSrc)
{
    BYTE i, res = 0;
	for (i = 0 ;i < PageSize/RowSize; i++)
	{
		if((res = Write_ROW_Bytes((BYTE *)(FlashAddr + i * RowSize),FlashBuffer + i * RowSize,RowSize)) != 0x01)
		{
			return res;
		}
	}
	return res;
}
