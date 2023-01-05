
#ifndef _SIMUL_DEBUG_H
#define _SIMUL_DEBUG_H


/*Methods declaration
* 
*/
extern void addAIndent(void);
extern void removeAIndent(void);
extern void debugPrint(unsigned char color, const char* message, ...);
extern void debugPrintArray(unsigned char color, const char* array, unsigned short len);
extern void backupIndent(void);
extern void restoreIndent(void);
#endif //_SIMUL_DEBUG_H