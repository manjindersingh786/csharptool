#include "types.h"
#include "SimulLowLevel.h"
#include "Oasis.h"
#include "SimulExtern.h"
#include "SimulTearing.h"

#define ResolveHandle(addr)						((uQuadByte)(ADDRESS_ERASE_EEPROM_START + (uQuadByte)addr))


uQuadByte SimulGetPhysicalAddr(uQuadByte uqLogicalAddr)
{
	return (((uQuadByte)ADDRESS_ERASE_EEPROM_START) + uqLogicalAddr);
}

uMonoByte Simul_ReadByte(uQuadByte uqStartAddress) {
	uqStartAddress = ResolveHandle(uqStartAddress);
	return  (*(uMonoByte*)(uqStartAddress));
}
void Simul_ReadFileHandler(
	uQuadByte uqStartAddress, 
	puMonoByte pumStructAddress, 
	uMonoByte umStructLength)
{
	uMonoByte umbOffset;

	for (umbOffset = 0x00; umbOffset < umStructLength; umbOffset++)
	{
		*(pumStructAddress + umbOffset) = SimulReadOneByte(SimulGetPhysicalAddr(uqStartAddress) + umbOffset);
	}
}

uMonoByte Simul_WriteFileHandler(
	uQuadByte uqStartAddress, 
	puMonoByte pumStructAddress, 
	uMonoByte umStructLength)
{
	return SimulWriteNBytes(SimulGetPhysicalAddr(uqStartAddress), pumStructAddress, umStructLength);
}

uMonoByte Simul_WriteNBytesNonAtomic(
	uQuadByte uqStartAddress, 
	puMonoByte pumSourceBuffer, 
	uDiByte udDataLength)
{
	return SimulWriteNBytes(SimulGetPhysicalAddr(uqStartAddress), pumSourceBuffer, udDataLength);
}

boolean Simul_WriteNBytes(
	uQuadByte ineeStartAddress, 
	puMonoByte inpumbSourceBuffer, 
	uDiByte inudbDataLength)
{
	ineeStartAddress = ResolveHandle(ineeStartAddress);
#ifdef TRANSACTION_ENABLED
	if (gumDoAtomicUpdation)
	{
		if (TR_BUFFER_FULL == backupDataToTRBuffer(ineeStartAddress, inudbDataLength))
		{
			if (FALSE == gumIsInternalTransaction)
			{
				setExceptionVairables(C_TECHNICAL_PROBLEM, C_TRANSACTION_EXCEPTION_OBJREF, 0x01, C_TRANSACTION_EXCEPN_BUFFER_FULL, C_REASON_CODE_REQUIRED);
			}

			ASSERT(FALSE == gumIsInternalTransaction);

			return FLASE;
		}
	}
#else
	uMonoByte	umbRetVal = 0x01;
	uDiByte		udbInputOffset = 0x00;

	if (gumStartTransaction
		&&
		(C_WRITE_ON_NVM != (gumStartTransaction & C_WRITE_ON_NVM))
		)
	{
		SimulUpdateCommandCache(inpumbSourceBuffer, 0x00, ineeStartAddress, (uMonoByte)inudbDataLength);

		return TRUE;
	}
	else
#endif
	{
		return SimulWriteNBytes(ineeStartAddress, inpumbSourceBuffer, inudbDataLength);
	}
}

uMonoByte Simul_WriteNBytesDirect(
	uQuadByte ineeStartAddress, 
	puMonoByte inpumbSourceBuffer, 
	uDiByte inudbDataLength)
{
	uDiByte udDataSize;
	ineeStartAddress = ResolveHandle(ineeStartAddress);

	while (inudbDataLength != 0x00)
	{
		//Calculating No. of bytes upto the end of current 'Write page'
		udDataSize = (PAGE_SIZE - ((ineeStartAddress) & (PAGE_SIZE - 0x01)));

		//Do not write more than available bytes
		if (udDataSize > inudbDataLength)
		{
			udDataSize = inudbDataLength;
		}

		SimulWriteNBytesPhyAddrDirect((puMonoByte)ineeStartAddress, (puMonoByte)inpumbSourceBuffer, udDataSize);
		
		if(memcmp((puMonoByte)ineeStartAddress, inpumbSourceBuffer, udDataSize))
		{
			return FALSE;
		}

		ineeStartAddress += udDataSize;
		inpumbSourceBuffer += udDataSize;
		inudbDataLength -= udDataSize;
	}
	return TRUE;
}

void Simul_ReadNBytesDirect(uQuadByte uqStartAddress, puMonoByte pumbOutput, uDiByte udbLen) {

	uqStartAddress = ResolveHandle(uqStartAddress);

	mem_cpy(pumbOutput, (const puMonoByte)uqStartAddress, udbLen);

}
uMonoByte Simul_WriteNBytesFromNVM(
	uQuadByte ineeDestAddress, 
	uQuadByte ineeSourceAddress, 
	uDiByte inudbDataLength, 
	uMonoByte umbWriteMode)
{
	ineeSourceAddress = ResolveHandle(ineeSourceAddress);

	if (C_WRITE_DIRECT == umbWriteMode)
	{
		return Simul_WriteNBytesDirect(ineeDestAddress, (puMonoByte)ineeSourceAddress, inudbDataLength);
	}
	else
	{
		return Simul_WriteNBytesNonAtomic(ineeDestAddress, (puMonoByte)ineeSourceAddress, inudbDataLength);
	}
}

boolean Simul_ErasePage(uQuadByte ineePageStartAddress)
{
	uDiByte udIndex;

	if (ineePageStartAddress % PAGE_SIZE)
	{
		return FALSE;
	}

	ineePageStartAddress = ResolveHandle(ineePageStartAddress);

	for (udIndex = 0; udIndex < PAGE_SIZE; udIndex++)
	{
		SimulWriteOneByte(ineePageStartAddress, (uMonoByte)C_ERASE_BYTE);
		ineePageStartAddress++;
	}

	return TRUE;
}

uMonoByte Simul_BackupToPageAT(
	uQuadByte ineeDestAddress, 
	uQuadByte ineeSrcAddress, 
	uDiByte inudbDataLength)
{
	uDiByte udOffset;

	ineeSrcAddress = ResolveHandle(ineeSrcAddress);
	ineeDestAddress = ResolveHandle(ineeDestAddress);

	guqBlockHandle = (uQuadByte)SimulGetBlockAddress((uQuadByte)ineeDestAddress);

	udOffset = (uDiByte)(ineeDestAddress - guqBlockHandle);

	SimulBackupEEPRom((uQuadByte)guqBlockHandle, (puMonoByte)ineeSrcAddress, udOffset, inudbDataLength);

	return 0x00;
}

uMonoByte Simul_WriteFromPageAT(void)
{
	SimulRestorePage(guqBlockHandle);
	return 0x00;
}

