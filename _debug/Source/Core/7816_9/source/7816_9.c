#include "7816_9.h"


/*
* Create file header under give DF
*
* @param fhaFile: Directory file under which given file to be created
* @param fhFile: Structure of File to be created
*
* @returns address of memory block if file created, otherwise EMPTY_OBJECT
* @precondition: fhaFile must be DF type
*/
tFileHeaderAddr _7816_9_createFileHeader(IN_ARG tFileHeaderAddr fhaFile, IN_ARG tFileHeader fhFile)
{
	/*stores file header address*/
	tFileHeaderAddr fhaSiblingFile;

	/*Allocates memory*/
	tFileHeaderAddr fhaCreatedFile;

	DBG_FUNC_ENTRY(">>[7816_9] _7816_9_createFileHeader() \n");

	fhaCreatedFile = mm_alloc(sizeof(tFileHeader), 0x00);

	if (NO_FREE_MEMORY == fhaCreatedFile) //if address is not invalid one
	{
		DBG_FUNC_EXIT("<<[7816_9] _7816_9_createFileHeader() returns '00'\n");
		return EMPTY_OBJECT;
	}

	/*file is created successfully*/
	/*Update the header of file*/
	mm_ReadUpdate(fhaCreatedFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_UPDATE);

	DBG_FUNC("[7816_9] _7816_9_createFileHeader: File header updated on new created file header address %2\n", fhaCreatedFile);

	/*Read DF file header to check where to write created file header address*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC("[7816_9] _7816_9_createFileHeader: Adding newly created file in File System under given File %2\n", fhFile.udbFileId);

	/*check if DF contains child or not*/
	if (NO_FILE_PRESENT == fhFile.FileSpecific.DF.fhaChild) { //DF doesnt have any child

		DBG_FUNC("[7816_9] _7816_9_createFileHeader: This is first child of given DF %2\n", fhaFile);

		/*stores address of created file*/
		fhFile.FileSpecific.DF.fhaChild = fhaCreatedFile;
	}
	else { //DF contains child

		DBG_FUNC("[7816_9] _7816_9_createFileHeader: DF already contains atleast one child\n");

		/*get Child file header address*/
		fhaSiblingFile = fhFile.FileSpecific.DF.fhaChild;

		DBG_FUNC("[7816_9] _7816_9_createFileHeader: Adding newly created EF as a sibling of last child\n");

		do {
			/*save current header address*/
			fhaFile = fhaSiblingFile;

			/*Created file would be child of given DF*/
			mm_ReadUpdate(fhaSiblingFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

			/*Get sibling o*/
			fhaSiblingFile = fhFile.fhaSibling;

		} while (fhaSiblingFile != NO_FILE_PRESENT); // loop until no sibling found

		DBG_FUNC("[7816_9] _7816_9_createFileHeader: Adding newly created EF %2 as a sibling of last child %2\n", fhaCreatedFile, fhaSiblingFile);

		/*Store address of created file as sibling*/
		fhFile.fhaSibling = fhaCreatedFile;
	}

	/*Update File header which contains updated sibling/Parent */
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_UPDATE);

	DBG_FUNC_EXIT("<<[7816_9] _7816_9_createFileHeader() returns '%2'\n", fhaCreatedFile);

	/*Address of newly created file*/
	return fhaCreatedFile;
}
