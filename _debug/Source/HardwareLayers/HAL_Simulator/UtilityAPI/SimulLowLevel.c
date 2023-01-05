#include "types.h"
#include "SimulConfig.h"
#include "Oasis.h"
#include <memory.h>

#define SUCCESS												(~0)
#define FAILURE												(0)

uMonoByte umbEEPRom[FLASH_SIZE] ;

uMonoByte dataYes = 0x05;
uMonoByte SimulReadOneByte(uQuadByte uqAddress)
{
	return (uMonoByte)*(((puMonoByte)(uqAddress)));
}

uMonoByte SimulWriteOneByte(uQuadByte uqDestAddr, uMonoByte umbValue)
{
	*((puMonoByte)uqDestAddr) = umbValue;

	return SUCCESS;
}

uMonoByte SimulWriteNBytes(uQuadByte uqDestAddr, puMonoByte pumbSrcAddr, uDiByte udLength)
{
	uDiByte udIndex;

	for (udIndex = 0x00; udIndex < udLength; udIndex++)
	{
		SimulWriteOneByte((uQuadByte)(uqDestAddr + udIndex), (uMonoByte)*(pumbSrcAddr + udIndex) );
	}

	return SUCCESS;
}

void SimulCopyRam2Ram(puMonoByte pumSrcBuff, puMonoByte pumDestBuff, uDiByte udLength)
{
	memcpy(pumDestBuff, pumSrcBuff, udLength);
}

void SimulWriteNBytesPhyAddrDirect(puMonoByte pumDestAddr, puMonoByte pumSrcAddr, uDiByte udLen)
{
	memcpy(pumDestAddr, pumSrcAddr, udLen);
}

boolean SimulWriteOneBytePhyAddr(uQuadByte uqDestAddr, uMonoByte umValue)
{
	memcpy((puMonoByte)uqDestAddr, &umValue, 0x01);

	return TRUE;
}


//====================================================
//	Copy memory from forward
void	mem_cpyFwd(uMonoByte* dst, const uMonoByte* src, uDiByte len)
{
	uDiByte	i;

	for (i = 0;i < len;i++)
	{
		dst[i] = *src++;
	}
}

//====================================================
//	Copy memory from backward
void	mem_cpyBwd(uMonoByte* dst, const uMonoByte* src, uDiByte len)
{
	while (len > 0)
	{
		dst[len - 1] = src[len - 1];
		len--;
	}
}

//====================================================
//	Compare memory
uMonoByte	mem_cmp(const uMonoByte* dst, const uMonoByte* src, uDiByte len)
{
	uDiByte	i;

	for (i = 0;i < len;i++)
	{
		if (dst[i] != *src++)
			return 1;
	}
	return 0;
}
//====================================================
// Set memory with given value
void	mem_set(uMonoByte* dst, uMonoByte value, uDiByte len) {
	uDiByte	i;

	for (i = 0;i < len;i++)
	{
		dst[i] = value;
	}
}