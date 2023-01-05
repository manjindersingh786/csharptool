#include "types.h"
#include "SimulConfig.h"
#include "SimulExtern.h"

#define C_ERASE_BYTE								(0xFF)
#define SimulGetBlockAddress(addr)					((uQuadByte)(addr - ((uDiByte)(addr) % PAGE_SIZE)))
/*
* Declaration of Low Level APIs
*/
extern uMonoByte SimulReadOneByte(uQuadByte uqAddress);
extern uQuadByte SimulGetPhysicalAddr(uQuadByte inuqLogicalAddr);
extern uMonoByte SimulWriteOneByte(uQuadByte uqDestAddr, uMonoByte umbValue);
extern uMonoByte SimulWriteNBytes(uQuadByte uqDestAddr, puMonoByte pumbSrcAddr, uDiByte udLength);
extern void SimulCopyRam2Ram(puMonoByte pumSrcBuff, puMonoByte pumDestBuff, uDiByte udLength);
extern void SimulWriteNBytesPhyAddrDirect(puMonoByte pumDestAddr, puMonoByte pumSrcAddr, uDiByte udLen);
extern uMonoByte SimulWriteOneBytePhyAddr(uQuadByte uqDestAddr, uMonoByte umValue);