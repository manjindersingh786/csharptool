#include "setjmp.h"
#include "statuswords.h"
#include "Debug.h"
#ifndef _EXCEPTION_HANDLING_EXTERN_H
#define _EXCEPTION_HANDLING_EXTERN_H

// Nested try-catch to handle for RFM so size of below array is 2
#define		MAX_SIZE_JUMP_BUFF				(0x02)

#define IS_EXCEPTION(ex)					(ex == excepRet)
#define UPDATE_STATUS_WORDS(ex,sw2)			excepRet = ex;P3_BYTE = (uMonoByte)sw2
/*Exception handling declarations*/
#ifdef WIN32
#define TRY					DBG_BACKUP_INDENT(); if((excepRet = setjmp(jmpBuff[gumbActiveTryCatch++])) == FIRST_CALL)
#define CATCH				else
#define THROW(ex)			DBG_WARN(#ex##"\n");DBG_RESTORE_INDENT();longjmp(jmpBuff[--gumbActiveTryCatch], (int)ex)
#define THROW2(ex,sw2)		DBG_WARN(#ex);P3_BYTE=sw2; DBG_RESTORE_INDENT();longjmp(jmpBuff[--gumbActiveTryCatch], (int)ex) 
#else
#define TRY								if((excepRet = setjmp(jmpBuff[gumbActiveTryCatch++])) == FIRST_CALL)
#define CATCH							else
#define THROW(ex)					longjmp(jmpBuff[--gumbActiveTryCatch], (int)ex)
#define THROW2(ex,sw2)		P3_BYTE=sw2; longjmp(jmpBuff[--gumbActiveTryCatch], (int)ex)
#endif
extern uMonoByte	gumbActiveTryCatch;
extern jmp_buf		jmpBuff[MAX_SIZE_JUMP_BUFF];
extern tSwExceps	excepRet;

#endif // !_EXCEPTION_HANDLING_EXTERN_H

