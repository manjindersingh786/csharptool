
#ifndef	_CHIP_LOW_LEVEL_H
#define	_CHIP_LOW_LEVEL_H
#include "types.h"
#include "ChipConfig.h"
#include "ChipExtern.h"
#include "arm_acle.h"

#define C_ERASE_BYTE												(0xFF)
#define SimulGetBlockAddress(addr)					((uQuaduMonoByte)(addr - ((uDiuMonoByte)(addr) % PAGE_SIZE)))


#endif //!_CHIP_LOW_LEVEL_H
