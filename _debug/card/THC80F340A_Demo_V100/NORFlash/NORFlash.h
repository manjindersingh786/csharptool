#ifndef	__NOR_FLASH_H__
#define __NOR_FLASH_H__
//	Variables and function of NORFlash.c
extern BYTE	FlashBuffer[PageSize];

BYTE	Erase_Page(WORD tgtaddr);
BYTE	Write_Bytes(BYTE * pDest,BYTE * pSrc,HALFW len);
BYTE	Write_page(WORD flashaddr,BYTE * pSrc);
#endif
