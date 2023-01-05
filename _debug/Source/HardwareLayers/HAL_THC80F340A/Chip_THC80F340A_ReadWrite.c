#include "types.h"
#include "ChipExtern.h"
void Chip_THC80F340A_ReadFileHandler(uQuadByte ineeStartAddress, puMonoByte inpStructAddress, uMonoByte inumbStructLength)
{
}

uMonoByte Chip_THC80F340A_WriteFileHandler(uQuadByte ineeStartAddress, puMonoByte inpStructAddress, uMonoByte inumbStructLength)
{
	return 0;
}

uMonoByte Chip_THC80F340A_WriteNBytesNonAtomic(uQuadByte ineeStartAddress, puMonoByte inpumbSourceBuffer, uDiByte inudbDataLength)
{
	return 0;
}

static uMonoByte umbFlashBuff[PAGE_SIZE];

uMonoByte Chip_THC80F340A_WriteNBytes(uQuadByte ineeStartAddress, puMonoByte inpumbSourceBuffer, uDiByte inudbDataLength)
{
	uMonoByte umbRet;
	uDiByte		udbOffset;
	uDiByte 	udbRemain;
	uDiByte 	udbToDo;
	uDiByte 	udbCounter=0x00;
	uQuadByte uqbFlashAddr;
	
	uqbFlashAddr = ineeStartAddress & (0xFFFFFE00 | PageSize); 
	udbOffset = (uDiByte)(ineeStartAddress & (PageSize - 1));
	    
	for(udbRemain = inudbDataLength; udbRemain; udbRemain  -= udbToDo)
	{
		udbToDo = (udbOffset + udbRemain) < PageSize ? udbRemain : (PageSize - udbOffset);
					
		mem_cpy(umbFlashBuff,(BYTE *)uqbFlashAddr,PageSize);
		

		mem_cpy(&umbFlashBuff[udbOffset],&inpumbSourceBuffer[udbCounter],udbToDo);
		udbCounter += udbToDo;			
			
		if((umbRet = Erase_Page(uqbFlashAddr)) == 0x01)
		{
			if((umbRet = 	Write_page((WORD)uqbFlashAddr, umbFlashBuff)) != 0x01)
			{	
				break;
			}
		}
		else
		{			
			break;
		}
			
		udbOffset = (uDiByte)(udbToDo + udbOffset) & (PageSize - 1);
		uqbFlashAddr += PageSize;
	}
	
	if(0x01 != umbRet)
	{
		return FALSE;
	}
	return TRUE;
}

uMonoByte Chip_THC80F340A_WriteNBytesDirect(uQuadByte ineeStartAddress, puMonoByte inpumbSourceBuffer, uDiByte inudbDataLength)
{
	return 0;
}

uMonoByte Chip_THC80F340A_WriteNBytesFromNVM(uQuadByte ineeDestAddress, uQuadByte ineeSourceAddress, uDiByte inudbDataLength)
{
	return 0;
}

/*Page MUST be erased before calling this function*/
uMonoByte Chip_THC80F340A_WritePage(uQuadByte ineeStartAddress, puMonoByte pumbInput){
	
	/*check if address is page aligned*/
	if(0x00 != (ineeStartAddress & (PageSize - 1))){
		
		/*if address is not page aligned*/
		return FALSE;
	}
	
	if(0x01 != (Write_page((WORD)ineeStartAddress, pumbInput))){
		
		/*PAGE write failed*/
		return FALSE;
	}
	/*page write succeed*/
	return TRUE;
}

uMonoByte Chip_THC80F340A_ErasePage(uQuadByte ineeStartAddress)
{
	uDiByte udbIndex=0;
	
	/*check if address is page aligned*/
	if(0x00 != (ineeStartAddress & (PageSize - 1))){
		
		/*if address is not page aligned*/
		return FALSE;
	}
	
	/*Check if content of file is already erased*/
	for(udbIndex=0; udbIndex<PAGE_SIZE; udbIndex++){
		
		if(0xFF != *((puMonoByte)(ineeStartAddress + udbIndex))){
			if(0x01 != Erase_Page((WORD)ineeStartAddress)){

				/*Page erase failed*/
				return FALSE;
			}
			return TRUE;			
		}
	}
	
	/*page erase succeeded*/ 
	return TRUE;
}


