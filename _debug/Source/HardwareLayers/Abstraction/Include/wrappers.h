
#ifndef _WRAPPERS_H
	#define _WRAPPERS_H

#include "types.h"
#ifdef WIN32
	#include "simulExtern.h"
#else
	#include "ChipExtern.h"
#endif
#include "wrappersExtern.h"
#include "AntiTearing_extern.h"
#include "MemoryWrappers.h"
#include "Constants.h"
#include "Debug.h"

#if (TABLE_ENTRY_SIZE==2)
	typedef uDiByte pageTableEntry;
	typedef uDiByte* pPageTableEntry;
#else
	typedef uMonoByte pageTableEntry;
	typedef uMonoByte* pPageTableEntry;
#endif

 

#endif
