#include "ExceptionHandling.h"

/*Exception handling variables definition*/

uMonoByte	gumbActiveTryCatch=0x00;
jmp_buf		jmpBuff[MAX_SIZE_JUMP_BUFF]; // if it is converted into Array, make sure implement same for Debug log feature of indentation DBG_BACKUP_INDENT
tSwExceps	excepRet;

