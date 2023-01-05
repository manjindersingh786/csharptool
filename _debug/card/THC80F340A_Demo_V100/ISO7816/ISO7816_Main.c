
#include	"TSIM.h"

BYTE	CMD_Buf[5];
BYTE	PPSFlag;
HALFW	SWptr;
HALFW	Foffset;
WORD	FlashBaseAddr;

const	BYTE	ValidFIDI[] = {11, 0x11, 0x12, 0x13, 0x18, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97};
const 	BYTE	ATR[11] ={0x3B,0x18,0x96,0x91,0x00,0x34,0x0A,0x01,0x00,0x90,0x00}; 
const 	BYTE	SW[14] = {0x90,0x00,0x6D,0x00,0x6C,0x00,0x65,0x01,0x6A,0x00,0x65,0x04,0x6E,0x00};
//				   		| Success | Invalid | P3 wrong|Write err|P1P2wrong| Read err| Invalid CLA |


__align(4) BYTE	IOBuf[256];
//====================================================
//	ISO7816 Main process
void	ISO7816_main(void)
{
	if(ISO7816_Time == 0)
	{	
		ISO7816_ConfigFIDI(0x11);			//	IO initial
		SendATR();							//	Send ATR
		PPSFlag = 0;  						//	Enable PPS
		ISO7816_Time ++;		
		FlashBaseAddr = FlashStart;			//	Set the flash base address as NOR flash starting in the 1st time
		return;								//	Return to the main process after sending ATR		
	}
	if(RcvAPDU())							//	Rcv APDU,include PPS
	{	
		Foffset = P1 * 0x100 + P2;			//	Start address of flash reading/writing
		SWptr = SUCCESS;					//	9000

       	TX_NULL_BYTE_ON						//	Enable every 8000 etu to send one NULL byte 60H
		CMD_Handle();
		TX_NULL_BYTE_OFF					//	Disable send NULL byte 60H
       	 	
		response();
		TxStatus(SWptr);					//	Tx SW
	}
}

//====================================================
//	Send ATR
void	SendATR(void)
{
	ISO7816_TxNByte(11,(BYTE *)ATR);
}
//====================================================
//	Receive APDU
BYTE	RcvAPDU(void)
{
	BYTE	PCK;
	
	CLA = ISO7816_RxByte();
	
	if(CLA == 0xFF && PPSFlag == 0)			//	To clear IO falling edge issure of ROCKY 301C4 reader
	{	
		INS = ISO7816_RxByte();
		switch(INS >> 4)
		{
			case	0x00:	PCK = ISO7816_RxByte();	break;
			
			case	0x01:	P1 = ISO7816_RxByte();
							PCK = ISO7816_RxByte();
							P2 = PCK;
							PCK ^= P1;				break;

			case	0x02:	P2 = ISO7816_RxByte();
							PCK = ISO7816_RxByte();
							PCK ^= P2;				break;

			case	0x03:	P1 = ISO7816_RxByte();
							P2 = ISO7816_RxByte();
							PCK = ISO7816_RxByte();
							PCK ^= P2;
							PCK ^= P1;				break;

			case	0x04:	P3 = ISO7816_RxByte();
							PCK = ISO7816_RxByte();
							PCK ^= P3;
							PCK ^= INS;				break;

			case	0x05:	P1 = ISO7816_RxByte();
							P3 = ISO7816_RxByte();
							PCK = ISO7816_RxByte();
							PCK ^= P3;
							PCK ^= P1;				break;

			case	0x06:	P2 = ISO7816_RxByte();
							P3 = ISO7816_RxByte();
							PCK = ISO7816_RxByte();
							PCK ^= P3;
							PCK ^= P2;				break;

			case	0x07:	P1 = ISO7816_RxByte();
							P2 = ISO7816_RxByte();
							P3 = ISO7816_RxByte();
							PCK = ISO7816_RxByte();
							PCK ^= P3;
							PCK ^= P2;
							PCK ^= P1;				break;
		}
		PCK ^= INS;

		if((PCK == 0xFF) && (INS == 0x10) && (PPSFlag == 0))	//	PPS
		{	
			ISO7816_PPS(CMD_Buf);		//	Set baud rate
			PPSFlag = 1;				//	PPS is enabled for the 1st time only
			return	0;
		}
	}
	else 
	{	
		INS = ISO7816_RxByte();
		P1 = ISO7816_RxByte();
		P2 = ISO7816_RxByte();
		P3 = ISO7816_RxByte();
		if(CLA != 0x00)
		{
			SWptr = IVDCLA;
			TxStatus(SWptr);					//	Tx SW
			return 0;
		}
		if(INS == PUTMEMORY || INS == ERFLASH ||INS == WRFLASH)
		{	
		    ETU_Delay(0x04);
			ISO7816_TxByte(INS);
			ISO7816_RxNByte(P3,IOBuf);
		}
		return	1;
	}
	return 0; 
}

//====================================================
//	ISO/IEC 7816 commands handling
//	Format: 00 5x P1 P2 P3 
void	CMD_Handle(void)
{			
	switch(INS)
	{	
		case	SETBASE:	SetBase();								break;		
				
		case	GETMEMORY:	ReadFlash(Foffset,IOBuf,P3);			break;		    
					    	
		case	PUTMEMORY:	UpdateMemory(Foffset,IOBuf,P3);			break;		   

		case	WRFLASH:	WriteFlash(Foffset,IOBuf,P3);			break;					

		case	ERFLASH:	EraseFlash(Foffset,IOBuf);				break;			

		case	CHECKCRC:	*(HALFW *)IOBuf = calCRC(Foffset,P3);	break;
		
		case	GETRDMN:	RandomNumGen(IOBuf,P3);					break;	

		case	RETURNBL:	ReturnToBL();							break;
				
		default:	SWptr = IVDINS;									break;	
	}
}

//====================================================
//	Tx response data
void	response(void)
{
	if((INS == GETMEMORY || INS ==GETRDMN) && SWptr == SUCCESS)
	{		
		ISO7816_TxByte(INS);						//	Procedure byte in Case 2 APDU,followed with Le data
		
		if(P3 == 0x00)
			ISO7816_TxNByte(0x100,IOBuf);	
		else
			ISO7816_TxNByte(P3,IOBuf);
	}
}

//====================================================
//	Send status word
void	TxStatus(int ptr)
{	
		ISO7816_TxByte(SW[ptr]);
		ISO7816_TxByte(SW[ptr+1]);
}

//====================================================
//	Handler SW
void	SWHandler(BYTE err)
{
	if(err == 0x02)
	{
		SWptr = WRERROR;					//	6501							
	}

	else if(err == 0x04)
	{
		SWptr = RDERROR;					//	6504						
	}
	
	else if(err == 0x08)
	{
		SWptr = P1P2ERR;					//	6A00
	}
}
//==================================================== 
void	ETU_Delay(HALFW ETUcount)
{
	ISOTRLD = ISOTDAT = ETUcount;
	ISOTMSK |= Bit0_En;						//	Interrupt disabled
	ISOTCON= 0x03;
	while(!(ISOTCON & Bit3_En))
	{;}
	ISOTCON |= Bit3_En;						//	Clear TF flag
	ISOTCON &= Bit0_Dis;					//	Timer stop
}

