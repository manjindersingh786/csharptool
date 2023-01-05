/*
* Data Types For Oasis
*/

#ifndef _TYPES_H

	#define _TYPES_H

	/*
	macro defines
	*/
	//indicates that global vairables
	#define GLOBAL								extern 
#ifndef NULL
	#define NULL								(0x00)
#endif
	#define UNINITIALIZED_ADDR					(0xFFFFFFFF)
	#define C_WRITE_ON_NVM						(0x02)
	#define C_WRITE_ON_BUFFER					(~C_WRITE_ON_NVM)

	#define IN_ARG		//used to represent Input arguments, unmodified by called function
	#define OUT_ARG		//used to represent ouput arguments, called function will update the arg
	#define INOUT_ARG	//used to represent Input-output arg both, called function can modify it as well


#define TRUE												((uMonoByte)~0)
#define FALSE												((uMonoByte)0)
	/*
	* Basic Types
	*/

	// Char Type
	typedef unsigned char uMonoByte;
	typedef unsigned char boolean;
	typedef signed char sMonoByte;
	typedef unsigned char* puMonoByte;
	typedef signed char* psMonoByte;

	// Short Type
	typedef unsigned short uDiByte;
	typedef signed short sDiByte;
	typedef unsigned short* puDiByte;
	typedef signed short* psDiByte;

	// Long Type
	typedef unsigned long uQuadByte;
	typedef signed long sQuadByte;
	typedef unsigned long* puQuadByte;
	typedef signed long* psQuadByte;

	// Long Long Type
	typedef unsigned long long uOctaByte;
	typedef signed long long sOctaByte;
	typedef unsigned long long* puOctaByte;
	typedef signed long long* psOctaByte;

	/*
	* Handle Types
	*/
	typedef puDiByte pmFileHandle;
	typedef uDiByte mFileHandle;
	typedef uQuadByte mHandle;
	typedef uQuadByte T_HEAP_OFFSET_HANDLE;
	typedef void (*T_FPTR)(void);
	typedef uMonoByte(*T_FPTR_NUM_METHODS_IN_PKG)(uMonoByte);
	typedef uDiByte(*T_DISPATCHER_ARRAY)(uMonoByte);



#endif // !_TYPES_H