#include "types.h"
#include "SimulLowLevel.h"
#include "Oasis.h"
#include "SimulExtern.h"
#include "SimulConfig.h"

/*
* Definition of Variables
*/
uDiByte		gudTotalDataInCache = 0x00;

uDiByte		guqBlockHandle = 0;

//Store data in Buffer to protect from Command Tearing
uMonoByte	SimulATBuffer[AT_BUFF_MAX_SIZE];


void SimulUpdateCommandCache(puMonoByte pumSrcBuff, 
	uMonoByte umSrcOffset, 
	uQuadByte uqDestAddr, 
	uMonoByte umLen)
{
	uMonoByte	umTempLen		= umLen;
	uDiByte		udIndex			= 0x00;
	uQuadByte	udTargetAddr	= 0x00;
	uMonoByte	umATHeader[4];

	// Appending Handle and Len
	umATHeader[0] = (uMonoByte)(uqDestAddr >> 0x10);
	umATHeader[1] = (uMonoByte)(uqDestAddr >> 0x08);
	umATHeader[2] = (uMonoByte)(uqDestAddr);

	// Appending Len
	umATHeader[3] = umLen;

	// Copy tearing header in AT Buffer
	SimulCopyRam2Ram(umATHeader, (SimulATBuffer + gudTotalDataInCache), TEARING_HEADER_LEN);
	gudTotalDataInCache += TEARING_HEADER_LEN;

	//Copy data part too
	SimulCopyRam2Ram((puMonoByte)(pumSrcBuff + umSrcOffset), (puMonoByte)(SimulATBuffer + gudTotalDataInCache), umLen);
	gudTotalDataInCache += umLen;

}

void SimulBackupEEPRom(uQuadByte uqBlockHandle, 
	puMonoByte pumSrcAddress, 
	uDiByte udOffset, 
	uDiByte udDataLength)
{
	//TODO:

}

void SimulRestorePage(uQuadByte uqBlockHandle)
{
	//TODO:
}
