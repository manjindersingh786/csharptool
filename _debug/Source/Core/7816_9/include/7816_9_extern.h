#include "types.h"
#include "MemoryManager_extern.h"
#include "7816_4_extern.h"
#ifndef _7816_9_EXTERN_H
#define _7816_9_EXTERN_H

/*
* Macro defines
*/


/*
* Methods declarations
*/

/*
* Create file header under give DF
*
* @param fhaDf: Directory file under which given file to be created
* @param fhFile: Structure of File to be created
*
* @returns address of memory block if file created, otherwise EMPTY_OBJECT
*/
extern tFileHeaderAddr _7816_9_createFileHeader(tFileHeaderAddr fhaDf, tFileHeader fhFile);

#endif // !_7816_4_EXTERN_H
