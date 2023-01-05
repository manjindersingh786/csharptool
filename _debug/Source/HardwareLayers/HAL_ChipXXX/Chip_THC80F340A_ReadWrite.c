#include "../Abstraction/Include/types.h"

void Chip_XXX_ReadFileHandler(uQuadByte ineeStartAddress, puMonoByte inpStructAddress, uMonoByte inumbStructLength)
{
}

uMonoByte Chip_XXX_WriteFileHandler(uQuadByte ineeStartAddress, puMonoByte inpStructAddress, uMonoByte inumbStructLength)
{
}

uMonoByte Chip_XXX_WriteNBytesNonAtomic(uQuadByte ineeStartAddress, puMonoByte inpumbSourceBuffer, uDiByte inudbDataLength)
{
}

uMonoByte Chip_XXX_WriteNBytes(uQuadByte ineeStartAddress, puMonoByte inpumbSourceBuffer, uDiByte inudbDataLength)
{
}

uMonoByte Chip_XXX_WriteNBytesDirect(uQuadByte ineeStartAddress, puMonoByte inpumbSourceBuffer, uDiByte inudbDataLength)
{
}

uMonoByte Chip_XXX_WriteNBytesFromNVM(uQuadByte ineeDestAddress, uQuadByte ineeSourceAddress, uDiByte inudbDataLength, uMonoByte umbWriteMode)
{
}

uMonoByte Chip_XXX_ErasePage(uQuadByte ineeStartAddress)
{
}

uMonoByte Chip_XXX_BackupToPageAT(uQuadByte ineeDestAddress, uQuadByte ineeSourceAddress, uDiByte inudbDataLength)
{
}

uMonoByte Chip_XXX_WriteFromPageAT(void)
{
}
