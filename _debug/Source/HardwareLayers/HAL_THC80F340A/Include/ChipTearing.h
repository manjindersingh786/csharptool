#include "types.h"

extern uQuadByte		guqBlockHandle;

extern void SimulUpdateCommandCache(puMonoByte pumSrcBuff, uMonoByte umSrcOffset, uQuadByte uqDestAddr, uMonoByte umLen);
extern void SimulBackupEEPRom(uQuadByte uqBlockHandle, puMonoByte pumSrcAddress, uDiByte udOffset, uDiByte udDataLength);
extern void SimulRestorePage(uQuadByte uqBlockHandle);