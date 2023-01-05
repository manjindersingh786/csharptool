#include <assert.h>
#ifdef WIN32
#include "SimulDebug.h"
#endif
#ifndef _DEBUG_H
#define _DEBUG_H

/*
* Macros defines
*/
typedef enum 
{
	red = 12,
	green = 10,
	yellow = 14,
	blue = 9,
	cyan = 11,
	magenta = 13,
}colors;

#define RED 			(red)
#define GREEN 			(green)
#define YELLOW 			(yellow)
#define BLUE 			(blue)
#define CYAN 			(cyan)
#define MAGENTA			(magenta)

#ifdef _DEBUG
	#define DBG_FUNC_ENTRY(...)					debugPrint(GREEN,__VA_ARGS__);addAIndent()
	#define DBG_FUNC_EXIT(...)					removeAIndent(); debugPrint(GREEN,__VA_ARGS__)
	#define DBG_FUNC(...)						(debugPrint(YELLOW,__VA_ARGS__))
	#define DBG_ERR(...)						(debugPrint(RED,__VA_ARGS__))
	#define DBG_WARN(...)						(debugPrint(CYAN,__VA_ARGS__))
	#define DBG_HEAD(...)						(debugPrint(MAGENTA,__VA_ARGS__))
	#define DBG_PRINT_ARRAY(x,y)				(debugPrintArray(YELLOW,x,y))
	#define DBG_BACKUP_INDENT()					(backupIndent())
	#define DBG_RESTORE_INDENT()				(restoreIndent())
#else
	#define DBG_FUNC_ENTRY(...)					((void) 0) // no action
	#define DBG_FUNC_EXIT(...)					((void) 0) // no action
	#define DBG_FUNC(...)						((void) 0) // no action
	#define DBG_ERR(...)						((void) 0) // no action
	#define DBG_WARN(...)						((void) 0) // no action
	#define DBG_HEAD(...)						((void) 0) // no action
	#define DBG_PRINT_ARRAY(x,y)				((void) 0) // no action
	#define DBG_BACKUP_INDENT()					((void) 0) // no action
	#define DBG_RESTORE_INDENT()					((void) 0) // no action
#endif

#endif //_DEBUG_H