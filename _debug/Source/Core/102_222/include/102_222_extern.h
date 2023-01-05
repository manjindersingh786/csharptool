#ifndef _102_222_EXTERN_H
#define _102_222_EXTERN_H
/*
* Macros defines
*/

/*Commands Instruction bytes*/
#define INS_CREATE_FILE			(0xE0)

//In case of DF and TF length will be 2
#define LENGTH_FILE_DESC									(0x02)
//In case of Linear File and Cyclic File record info. are extra added
#define LENGTH_REC_FILE_DESC								(0x04)

/*commands decalaration*/
extern void _102222_createFile(void);
#endif // !_102_222_EXTERN_H

