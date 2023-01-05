#include "7816_4.h"

/*
* Macros define
*/
//LCSI values
#define LCSI_FOR_CREATION					(0x01)
#define LCSI_FOR_INIT						(0x03)
#define LCSI_FOR_ACTIVATE					(0x05)
#define LCSI_FOR_DEACTIVATE					(0x04)
#define LCSI_FOR_TERMINATE					(0x0C)
#define INS_INCREASE						(0x32)

/*
* Variables definition
*/

/*Below buffer will be not used to store data across commands, it will be used in a function only and after execution of function , data will be no more valid*/
uMonoByte gumbWorkingBuffer[LENGTH_WORKING_BUFFER];

/*
* It returns Address of MF
*
* @param 
*/
tFileHeaderAddr _7816_4_getMfAddress(void) {

	/*Store Address of MF*/
	tFileHeaderAddr fhaFileAddr;

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getMfAddress() \n");

	DBG_FUNC("[7816_4] _7816_4_getMfAddress: Read Address of MF file from System page\n");

	/*Read MF address from system page*/
	baseReadNBytes(ADDRESS_MASTER_FILE, (puMonoByte) &fhaFileAddr, SIZE_OF_FILE_HEADER_ADDR);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getMfAddress() returns %2 \n", fhaFileAddr);
	/*Retur MF address*/
	return fhaFileAddr;
}

/*
* It searches for given AID in the ADF List file, and return its header address
* 
* @param pumbAid: Contains AID, 1st byte indicates length and remaining AID bytes
* @param pfhaFile: if an ADF found, it contains Address of that file.'
* 
* @return If AID matches return record no. 
*		  if EOF reached return return 0x00 i.e. invalid record no.
*		  if AID doesnt match and encounter FF before EOF, return record no.
*/
uMonoByte _7816_4_ifAdfExists(IN_ARG puMonoByte pumbAid, OUT_ARG ptFileHeaderAddr pfhaFile) {

	uMonoByte	umbFileRecord = 0x00;
	uMonoByte	umbNoOfRec;
	tFileHeader fhFile; //file header
	tMemAddr	maFileData;
	uMonoByte	umbRecord[LENGTH_RECORD_ADF_LIST]; // 1 byte for length and 16 bytes for AID

	/*Check address must not be NULL*/
	assert(pfhaFile != NULL);
	assert(pumbAid != NULL);

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_ifAdfExists() \n");

	DBG_FUNC("[7816_4] _7816_4_ifAdfExists: Incoming DF name:\n");
	DBG_PRINT_ARRAY(pumbAid + 0x01, pumbAid[0]);

	DBG_FUNC("[7816_4] _7816_4_ifAdfExists: Get File address of Prop. admin use DF 7F49 \n");

	/*Assuming All prop. files must be present with intended file attributes,
	* so no extra checks are added for validation of prop file*/
	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, pfhaFile);

	assert(*pfhaFile != EMPTY_OBJECT); //ensure file must be created

	DBG_FUNC("[7816_4] _7816_4_ifAdfExists: Get File address of Prop. Adf list file under DF 7F49 \n");

	/*get memory reference of ADF list ef*/
	_7816_4_ifFileExists(FID_PROP_ADF_LIST, *pfhaFile, MODE_FIND_FID, pfhaFile);

	assert(*pfhaFile != EMPTY_OBJECT); // ensure file must be created

	DBG_FUNC("[7816_4] _7816_4_ifAdfExists: Read file header of of Prop. Adf list file \n");

	//TODO: here we can use _7816_4_ForwardSearchRecordFile() for searching AID in Prop. file

	/*Read file header*/
	mm_ReadUpdate(*pfhaFile, (puMonoByte) &fhFile, OFFSET_START, sizeof(fhFile), MEM_MODE_READ);

	/*read memory addess of file data*/
	maFileData = fhFile.FileSpecific.EF.maFileData;

	/*Set file header to EMPTY*/
	*pfhaFile = NO_FILE_PRESENT;

	/*get total record nos*/
	umbNoOfRec = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec;

	do {
		/*increment record pointer*/
		umbFileRecord++;

		DBG_FUNC("[7816_4] _7816_4_ifAdfExists: Read record %1  of Prop. ADF List EF :\n", umbFileRecord);

		/*Read file data*/
		mm_ReadUpdate(maFileData, umbRecord, (umbFileRecord - 1) * LENGTH_RECORD_ADF_LIST, LENGTH_RECORD_ADF_LIST, MEM_MODE_READ);

		DBG_PRINT_ARRAY(umbRecord, LENGTH_RECORD_ADF_LIST);

		if (EQUAL == mem_cmp(pumbAid + 0x01, umbRecord + 0x01, pumbAid[0x00])) // Both arrays are equal
		{
			DBG_FUNC("[7816_4] _7816_4_ifAdfExists: DF name found\n");
			/*Copy file Header Address*/
			mem_cpy((puMonoByte)pfhaFile, umbRecord + OFFSET_ADDR_FILE_HEADER, SIZE_OF_FILE_HEADER_ADDR);

			/*return record no.*/
			DBG_FUNC_EXIT("<<[7816_4] _7816_4_ifAdfExists() returns %1 \n", umbFileRecord);

			return umbFileRecord;
		}
	} while (
		(umbFileRecord < umbNoOfRec) //Record no. must be less than or equal to total no. of records
		&&												//AND
		(umbRecord[0x00] != DEFAULT_BYTE_IN_FILE)		//If data is not FF
		);


	if (umbFileRecord == umbNoOfRec) //check if above loop is failed as EOF reached
	{
		DBG_FUNC("[7816_4] _7816_4_ifAdfExists: No empty record left\n");
		// AID is not present and EOF reached
		umbFileRecord = RECORD_UNDEFINED;
	}

	DBG_FUNC("[7816_4] _7816_4_ifAdfExists: DF name not found\n");

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_ifAdfExists() returns %1 \n", umbFileRecord);
	return umbFileRecord;
}

/*
* It searches for given File ID in the given, and return its header address
*
* @param udbFileId: Contains File ID
* @param fhaDf: Address of DF under which search being done
* @param umbSearchMode: Mode: Search by FID or SFID
* @param pfhaFile: if a File found, it contains Address of that file.
*/
void _7816_4_ifFileExists(IN_ARG uDiByte udbFileId,
	IN_ARG tFileHeaderAddr fhaDf,
	IN_ARG uMonoByte umbSearchMode,
	OUT_ARG ptFileHeaderAddr pfhaFile) {

	tFileHeader		fhFile;

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_ifFileExists() \n");

	DBG_FUNC("[7816_4] _7816_4_ifFileExists: Searching File ID %2 under DF %2\n", udbFileId, fhaDf);

	/*Check address must not be NULL*/
	assert(pfhaFile != NULL);

	*pfhaFile = NO_FILE_PRESENT; //default set it to empty

	if (NO_FILE_PRESENT == fhaDf) // check if given DF address is not empty
	{
		DBG_WARN("[7816_4] _7816_4_ifFileExists: Given DF address is empty \n");
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_ifFileExists() \n");
		return;
	}

	DBG_FUNC("[7816_4] _7816_4_ifFileExists: Reading file header from Given DF address %2 \n", fhaDf);

	/*Read DF file header to check where to write created file header address*/
	mm_ReadUpdate(fhaDf, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	/*Check if searching for SFI*/
	if (MODE_FIND_SFID == umbSearchMode) {

		/*Searching for SFI*/

		//TODO: SFI cant be 00 so below check can be removed
		if ((0x00 == (udbFileId) || (SFI_NOT_SUPPORTED == (udbFileId)))) { // Check if invalid Short file identifier
			/*Invalid SFI presented*/
			DBG_WARN("[7816_4] _7816_4_ifFileExists: SFI is invalid i.e. 00 or 1F \n");
			DBG_FUNC_EXIT("<<[7816_4] _7816_4_ifFileExists() \n");
			return;
		}

	}



	/*check if DF contains child or not*/
	if (NO_FILE_PRESENT != fhFile.FileSpecific.DF.fhaChild) { //DF conatins child

		/*get Child file header address*/
		*pfhaFile = fhFile.FileSpecific.DF.fhaChild;

		do {
			DBG_FUNC("Searching...\n");

			/*Created file would be child of given DF*/
			mm_ReadUpdate(*pfhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);
			
			if (MODE_FIND_SFID == umbSearchMode)
			{
				if ((!IS(DF, fhFile.umbFileDesc)) // if its EF
					&&							//AND
					(fhFile.FileSpecific.EF.umbSfi == (uMonoByte)udbFileId)){ //SFI matches
					DBG_FUNC("[7816_4] _7816_4_ifFileExists: Given SFID matches with EF %2 \n", *pfhaFile);
					DBG_FUNC_EXIT("<<[7816_4] _7816_4_ifFileExists() \n");
					return;
				}
			}
			else if (fhFile.udbFileId == udbFileId) //Mode File ID, so direct compare file id
			{
				/*File ID matched*/
				DBG_FUNC("[7816_4] _7816_4_ifFileExists: Given FID matches with File %2 \n", *pfhaFile);
				DBG_FUNC_EXIT("<<[7816_4] _7816_4_ifFileExists() \n");
				return;
			}

			/*Get sibling o*/
			*pfhaFile = fhFile.fhaSibling;

		} while (*pfhaFile != NO_FILE_PRESENT); // loop until no sibling found

	}
	DBG_FUNC("[7816_4] _7816_4_ifFileExists:File ID %2 not found \n", udbFileId);
	DBG_FUNC_EXIT("<<[7816_4] _7816_4_ifFileExists() \n");
}

/*
* It update/read the record data of given File header address
*
* @param fhaFile: Address of file that need to update
* @param pumData: If mode Update, Data will be read from this array
*					If mode Read, data will be stored in this array
* @param umbRecordNo: record to read/update
* @param amMode: Operation mode : read or update or increase
*
* @return Read/Updated logical record no.
* @precondition: fhaFile must be an Record File
*/
uMonoByte _7816_4_readUpdateRecordFile(	IN_ARG		tFileHeaderAddr fhaFile,
										INOUT_ARG	puMonoByte pumData,
										IN_ARG		uMonoByte umbRecordNo,
										IN_ARG		tAccessMode amMode) {

	uMonoByte	umbLastUpdatedRec;
	uMonoByte	umbRecLength;
	uDiByte		udbFileOffset;
	tFileHeader fhFile; //stores target EF

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_readUpdateRecordFile() \n");

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);
	assert(pumData != NULL);
	assert(umbRecordNo != RECORD_UNDEFINED);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(fhFile), MEM_MODE_READ);

	assert((IS(LF, fhFile.umbFileDesc)) || (IS(CF, fhFile.umbFileDesc))); // check it must be Record structured file

	/*Get last updated record*/
	umbLastUpdatedRec = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbLastUpdatedRecord;

	/*Get record length*/
	umbRecLength = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize;

	DBG_FUNC("[7816_4] _7816_4_readUpdateRecordFile: Targetting file %2 of record length %1\n", fhFile.udbFileId, umbRecLength);

	DBG_FUNC("[7816_4] _7816_4_readUpdateRecordFile: Performing operation on record no. %1\n", umbRecordNo);

	if ((MEM_MODE_UPDATE == amMode) // Update operation
		&& // AND
		(IS(CF, fhFile.umbFileDesc))) { // File is cyclic

		/*Record no. will be always oldest one, which is last record*/
		umbRecordNo = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec;
	}

	/*Caculate actual record no. from logical record no.*/
	umbLastUpdatedRec = ((umbLastUpdatedRec + umbRecordNo - 2) % fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec);

	/*calculate offset*/
	udbFileOffset = umbLastUpdatedRec * umbRecLength;

	if (ACC_MODE_FILE_READ == amMode) { // File in read mode
		amMode = (tAccessMode)MEM_MODE_READ;
		DBG_FUNC("[7816_4] _7816_4_readUpdateRecordFile: Reading record \n");
	}
	else { // File in case update or increase mode
		amMode = (tAccessMode)MEM_MODE_UPDATE;
		DBG_FUNC("[7816_4] _7816_4_readUpdateRecordFile: Updating record\n");
	}

	/*Read/update the file on given memory address of file data*/
	mm_ReadUpdate(fhFile.FileSpecific.EF.maFileData, pumData, udbFileOffset, umbRecLength, (tMemMode)amMode);

	if ((MEM_MODE_UPDATE == amMode) // Update operation
		&& // AND
		(IS(CF, fhFile.umbFileDesc))) { // File is cyclic

		DBG_FUNC("[7816_4] _7816_4_readUpdateRecordFile: Last updated record no %1 has been updated for this cyclic EF\n", umbLastUpdatedRec+1);

		/*Update last updated record in cyclic file*/
		fhFile.FileSpecific.EF.fileData.ofRecordFile.umbLastUpdatedRecord = umbLastUpdatedRec + 1;

		/*update the file header with new last updated record*/
		mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(fhFile), MEM_MODE_UPDATE);

		/*After updating, it becomes record no. 1 */
		umbRecordNo = RECORD_FIRST;
	}

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_readUpdateRecordFile() returns %1\n", umbRecordNo);
	return umbRecordNo; //return read/updated record no.
}

/*
* It update/read the data of given File header address 
*
* @param fhaFile: Address of file that need to update
* @param pumData: If mode Update, Data will be read from this array
*					If mode Read, data will be stored in this array
* @param udbFileOffset: offset from/to  where data read/update
* @param udbDataLength: Length of data to update/read
* @param amMode: Operation mode : read or update or increase
* 
* @precondition: fhaFile must be an EF
*/
void _7816_4_readUpdateFile(IN_ARG		tFileHeaderAddr fhaFile,
							INOUT_ARG	puMonoByte pumData,
							IN_ARG		uDiByte udbFileOffset,
							IN_ARG		uDiByte udbDataLength,
							IN_ARG		tAccessMode amMode) {

	tFileHeader fhFile; //stores file header

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_readUpdateFile() \n");

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);
	assert(pumData != NULL);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte) &fhFile, OFFSET_START, sizeof(fhFile), MEM_MODE_READ);

	if (ACC_MODE_FILE_READ == amMode){ // File in read mode
		DBG_FUNC("[7816_4] _7816_4_readUpdateFile: Reading File %2\n", fhFile.udbFileId);
		amMode = (tAccessMode)MEM_MODE_READ;
	}
	else { // File in case update or increase mode
		DBG_FUNC("[7816_4] _7816_4_readUpdateFile: Updating File %2\n", fhFile.udbFileId);
		amMode = (tAccessMode)MEM_MODE_UPDATE;
	}

	/*Read/update the file on given memory address of file data*/
	mm_ReadUpdate(fhFile.FileSpecific.EF.maFileData, pumData, udbFileOffset, udbDataLength, (tMemMode)amMode);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_readUpdateFile() \n");
}

/*
* It read the file header from given File header address
*
* @param fhaFile: Address of file whose header to read
* @param pfhFile: Read the file
*
* @precondition: fhaFile must be a valid file header address
*/
void _7816_4_getFileHeader(	IN_ARG		tFileHeaderAddr fhaFile,
							OUT_ARG		ptFileHeader	pfhFile) {
	
	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);
	assert(pfhFile != NULL);

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getFileHeader() \n");

	DBG_FUNC("[7816_4] _7816_4_getFileHeader: Read file header from File address %2\n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)pfhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getFileHeader() \n");

}

/*
* It read the file ID from given File header address
*
* @param fhaFile: Address of file whose header to read
* @param pfhFile: Read the file
*
* @precondition: fhaFile must be a valid file header address
*/
uDiByte _7816_4_getFileId(IN_ARG		tFileHeaderAddr fhaFile) {

	tFileHeader fhFile;

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getFileId() \n");

	DBG_FUNC("[7816_4] _7816_4_getFileId: Read file header from File address %2\n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getFileId() returns %2\n", fhFile.udbFileId);

	return fhFile.udbFileId;

}

/*
* It read the file data address from given File header address
*
* @param fhaFile: file whose data address to read
*
* @precondition: fhaFile must be a valid EF header address
*/
tMemAddr _7816_4_getFileDataAddress(IN_ARG tFileHeaderAddr fhaFile) {

	tFileHeader fhFile;

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getFileDataAddress() \n");

	DBG_FUNC("[7816_4] _7816_4_getFileDataAddress: Read file header from File address %2\n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	assert(!(IS(DF, fhFile.umbFileDesc)));

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getFileDataAddress() returns %2 \n", fhFile.FileSpecific.EF.maFileData);

	return fhFile.FileSpecific.EF.maFileData;

}

/*
* It returns the File descriptor byte of file 
* 
* @param fhaFile: Address of file header
* 
* @return File descriptor byte
* @precondition: fhaFile must be a valid file header address
*/
uMonoByte	_7816_4_getFileDescriptor(IN_ARG tFileHeaderAddr fhaFile) {

	tFileHeader fhFile; // Stores file header info.

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getFileDescriptor() \n");

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC("[7816_4] _7816_4_getFileDescriptor: Read file header from given file address '%2' \n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC("[7816_4] _7816_4_getFileDescriptor: Return File descriptor byte \n");

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getFileDescriptor() returns %1 \n", fhFile.umbFileDesc);
	return fhFile.umbFileDesc; // File descriptor byte
}
/*
* It returns the ARR file and Arr record
*
* @param fhaFile: Address of file header
* @param paiFile: Arr file id and record will be returned through this param
*
* @precondition: fhaFile must be a valid file header address
*/
void _7816_4_getArrInfomation(IN_ARG tFileHeaderAddr fhaFile, OUT_ARG ptArrInfo paiFile) {

	tFileHeader fhFile; // Stores file header info.

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getArrInfomation() \n");

	/*check addresses must not be null*/
	assert(fhaFile != EMPTY_OBJECT);
	assert(paiFile != NULL);

	DBG_FUNC("[7816_4] _7816_4_getArrInfomation: Read file header from given file address '%2' \n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC("[7816_4] _7816_4_getArrInfomation: Return File descriptor byte \n");

	/*save ARR infor*/
	paiFile->udbArrFile = fhFile.udbEFarrId;
	paiFile->umbArrRec = fhFile.umbEFarrRec;

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getArrInfomation()\n");
}


/*
* It returns the File size of file
*
* @param fhaFile: Address of file header
*
* @return File size
* @precondition: fhaFile must be a valid file header address of Transparent File
*/
uDiByte _7816_4_getFileSize(IN_ARG tFileHeaderAddr fhaFile) {
	
	tFileHeader fhFile; // Stores file header info.

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getFileSize() \n");

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC("[7816_4] _7816_4_getFileSize: Read file header from given file address '%2' \n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getFileSize() returns %2 \n", fhFile.FileSpecific.EF.fileData.udbFileSize);
	return fhFile.FileSpecific.EF.fileData.udbFileSize; // return File size
}

/*
* It returns the Record length of given file
*
* @param fhaFile: Address of file header
*
* @return Record length
* @precondition: fhaFile must be a valid file header address of Linear fixed or Cyclic File
*/
uMonoByte _7816_4_getRecordLength(IN_ARG tFileHeaderAddr fhaFile) {
	
	tFileHeader fhFile; // Stores file header info.
	
	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getRecordLength() \n");

	DBG_FUNC("[7816_4] _7816_4_getRecordLength: Read file header from given file address '%2' \n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getRecordLength() returns %1 \n", fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize);
	return fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize; // return record length
}
/*
* It returns the No. of records of given file
*
* @param fhaFile: Address of file header
*
* @return No of records
* @precondition: fhaFile must be a valid file header address of Linear fixed or Cyclic File
*/
uMonoByte _7816_4_getNoOfRecords(IN_ARG tFileHeaderAddr fhaFile) {

	tFileHeader fhFile; // Stores file header info.

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getNoOfRecords() \n");

	DBG_FUNC("[7816_4] _7816_4_getNoOfRecords: Read file header of given file address %2 \n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	assert((IS(LF, fhFile.umbFileDesc)) || (IS(CF, fhFile.umbFileDesc))); // check it must be Record structured file

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getNoOfRecords() returns %1 \n", fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec);
	return fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec; // return no. of records
}

/*
* It returns the last updated record of given file
*
* @param fhaFile: Address of file header
*
* @return last updated record
* @precondition: fhaFile must be a valid file header address of Linear fixed or Cyclic File
*/
uMonoByte _7816_4_getLastUpdatedRecord(IN_ARG tFileHeaderAddr fhaFile) {

	tFileHeader fhFile; // Stores file header info.
	
	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getLastUpdatedRecord() \n");

	DBG_FUNC("[7816_4] _7816_4_getLastUpdatedRecord: Read file header of given File address %2 \n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	assert((IS(LF, fhFile.umbFileDesc)) || (IS(CF, fhFile.umbFileDesc))); // check it must be Record structured file

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getLastUpdatedRecord() returns %1 \n", fhFile.FileSpecific.EF.fileData.ofRecordFile.umbLastUpdatedRecord);
	return fhFile.FileSpecific.EF.fileData.ofRecordFile.umbLastUpdatedRecord; // return last updated recprd
}
/*
* It Sets the LCSI byte for given EF
*
* @param fhaFile: Address of file header
* @param flState: Life cycle state to update in file
*
* @precondition: fhaFile must be a valid file header address of an EF
*/
void _7816_4_setLifeCycleStatus(IN_ARG tFileHeaderAddr fhaFile, IN_ARG tFileLCSI flState) {

	tFileHeader			fhFile; // Stores file header info.

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_setLifeCycleState() \n");

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC("[7816_4] _7816_4_setLifeCycleState: Read file header from given file address '%2' \n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	fhFile.flState = flState; // update new state

	/*Update file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_UPDATE);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_setLifeCycleState() \n");
}

/*
* It get the Life cycle state of given EF
*
* @param fhaFile: Address of file header
*
* @precondition: fhaFile must be a valid file header address of an EF
*/
tFileLCSI _7816_4_getLifeCycleStatus(IN_ARG tFileHeaderAddr fhaFile) {

	tFileHeader			fhFile; // Stores file header info.

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getLifeCycleStatus() \n");

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC("[7816_4] _7816_4_getLifeCycleStatus: Read file header from given file address '%2' \n", fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getLifeCycleStatus() returns %1\n", fhFile.flState);
	return fhFile.flState;
}

/*
* It searches the no. of records for given pattern in forward/backward direction
*
* @param fhaFile: Address of file header
* @param pumbPattern: contains search pattern
* @param umbPatternLen: search pattern length
* @param umbRecordNo: Record no. from where search starts
* @param smMode: Search mode : forward or backward
* @param soOccur: Search occurrence: First or ALL ?
* @param umbOffset: absoulte position in record for search
* @param pumbOutput: Stores list of records matched with given pattern
*
* @return Total no. of records found matching given pattern, List of those records in pumbOutput
* @precondition: fhaFile must be a valid file Record file, record no. must be valid , requstor has to ensure
*/
uMonoByte _7816_4_SearchRecordFile(	IN_ARG	tFileHeaderAddr		fhaFile,
									IN_ARG	puMonoByte			pumbPattern,
									IN_ARG	uMonoByte			umbPatternLen,
									IN_ARG	tSearchMode			smMode,
									IN_ARG	tSearchOccurrence	soOccur,
									IN_ARG	uMonoByte			umbRecordNo,
									IN_ARG	uMonoByte			umbOffset,
									OUT_ARG	puMonoByte			pumbOutput) {

	uMonoByte		umbRecordsProcessed;
	uMonoByte		umbOutputLen = 0x00;
	tFileHeader		fhFile;

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_SearchRecordFile() \n");

	/*Get file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(fhFile), MEM_MODE_READ);

	assert((IS(LF, fhFile.umbFileDesc)) || (IS(CF, fhFile.umbFileDesc))); // check it must be Record structured file
	assert((smMode == SEARCH_RECORD_BACKWARD) || (smMode == SEARCH_RECORD_FORWARD)); // Valid search mode

#ifdef _DEBUG
	if (smMode == SEARCH_RECORD_BACKWARD) { // backward search mode0

		DBG_FUNC("[7816_4] _7816_4_SearchRecordFile: Search record in backward direction\n");
	}
	else { // forward search mode

		DBG_FUNC("[7816_4] _7816_4_SearchRecordFile: Search record in forward direction\n");
	}
#endif // _DEBUG

	if (IS(LF, fhFile.umbFileDesc)) { // if linear fixed file

		DBG_FUNC("[7816_4] _7816_4_SearchRecordFile: File is linear fixed\n");

		/*for linear fixed file, assume all previous records have been searched*/
		umbRecordsProcessed = (SEARCH_RECORD_FORWARD == smMode) ? (umbRecordNo-1): (fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec - umbRecordNo);
	}
	else { // Cyclic file

		DBG_FUNC("[7816_4] _7816_4_SearchRecordFile: File is Cyclic\n");

		/*for Cyclic file,all records will be searched, because last record connected to first record*/
		umbRecordsProcessed = 0x00;
	}

	while (umbRecordsProcessed < fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec) { // check if all records have been searched

		DBG_FUNC("[7816_4] _7816_4_SearchRecordFile: Searching record %1\n", umbRecordNo);

		/*Search the whole record data*/
		if (TRUE == _7816_4_compareWithRecord(fhaFile, umbRecordNo, umbOffset, pumbPattern, umbPatternLen)) { // Pattern found in current record

			DBG_FUNC("[7816_4] _7816_4_SearchRecordFile: Pattern matched\n");
			pumbOutput[umbOutputLen++] = umbRecordNo;

			if (SEARCH_FIRST_OCCURRENCE == soOccur) { // If first occurence to return only

				DBG_FUNC_EXIT("<<[7816_4] _7816_4_SearchRecordFile() retruns %1\n", 0x01);
				return 0x01;
			}
		}

		/*get next record*/
		(SEARCH_RECORD_FORWARD == smMode) ? umbRecordNo++ : umbRecordNo--;

		umbRecordsProcessed++; //increment the processed no. of records

		/* Below check is added for cyclic file */
		if (umbRecordNo > fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec) { //This case is only applicable for Forward search

			umbRecordNo = RECORD_FIRST;
		}
		else if (umbRecordNo == 0x00) { // this case is applicable for backward search

			umbRecordNo = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec;
		}
	}

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_SearchRecordFile() retruns %1\n", umbOutputLen);
	return umbOutputLen;
}

/*
* It is enhanced search for the no. of records for given pattern in forward/backward direction
*
* @param fhaFile: Address of file header
* @param pumbPattern: contains search pattern
* @param umbPatternLen: search pattern length
* @param umbRecordNo: Record no. from where search starts
* @param smMode: Search mode : forward or backward
* @param esmMode: Search Mechanism for enhanced mode, Offset or First Byte Occurrence
* @param umbOffsetOrValue: absoulte position in record for search or the byte to search first occurrence
* @param pumbOutput: Stores list of records matched with given pattern
*
* @return Total no. of records found matching given pattern, List of those records in pumbOutput
* @precondition: fhaFile must be a valid file Record file, record no. must be valid , requstor has to ensure
*/
uMonoByte _7816_4_EnhancedSearchRecordFile( IN_ARG	tFileHeaderAddr				fhaFile,
											IN_ARG	puMonoByte					pumbPattern,
											IN_ARG	uMonoByte					umbPatternLen,
											IN_ARG	tSearchMode					smMode,
											IN_ARG	tEnhancedSearchMechanism	esmMode,
											IN_ARG	uMonoByte					umbRecordNo,
											IN_ARG	uMonoByte					umbOffsetOrValue,
											OUT_ARG	puMonoByte					pumbOutput ) {

	uMonoByte		umbLastUpdatedRec;
	uMonoByte		umbActualRecordToUpdate;
	uMonoByte		umbRecordsProcessed;
	uMonoByte		umbOutputLen = 0x00;
	tFileHeader		fhFile;
	uMonoByte		umbIndex;
	uMonoByte		umbRecordLength;
	uDiByte			udbFileOffset;

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_EnhancedSearchRecordFile() \n");

	if (esmMode == SEARCH_BY_OFFSET) { //Enhanced search mode with offset

		DBG_FUNC("[7816_4] _7816_4_EnhancedSearchRecordFile: Enhanced Search mode with Offset\n");
		umbOutputLen = _7816_4_SearchRecordFile(fhaFile, pumbPattern, umbPatternLen, smMode, SEARCH_ALL_OCCURRENCES, umbRecordNo, umbOffsetOrValue, pumbOutput);
	}

	else { //Enhanced search mode with first byte occurrence of value

		DBG_FUNC("[7816_4] _7816_4_EnhancedSearchRecordFile: Enhanced Search mode with First Byte Occurrence of Value\n");
		/*Get file header*/
		mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(fhFile), MEM_MODE_READ);

		assert((IS(LF, fhFile.umbFileDesc)) || (IS(CF, fhFile.umbFileDesc))); // check it must be Record structured file
		assert((smMode == SEARCH_RECORD_BACKWARD) || (smMode == SEARCH_RECORD_FORWARD)); // Valid search mode

#ifdef _DEBUG
		if (smMode == SEARCH_RECORD_BACKWARD) { // backward search mode

			DBG_FUNC("[7816_4] _7816_4_EnhancedSearchRecordFile: Search record in backward direction\n");
		}
		else { // forward search mode

			DBG_FUNC("[7816_4] _7816_4_EnhancedSearchRecordFile: Search record in forward direction\n");
		}
#endif // _DEBUG

		if (IS(LF, fhFile.umbFileDesc)) { // if linear fixed file

			DBG_FUNC("[7816_4] _7816_4_EnhancedSearchRecordFile: File is linear fixed\n");

			/*for linear fixed file, assume all previous records have been searched*/
			umbRecordsProcessed = (SEARCH_RECORD_FORWARD == smMode) ? (umbRecordNo - 1) : (fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec - umbRecordNo);
		}
		else { // Cyclic file

			DBG_FUNC("[7816_4] _7816_4_EnhancedSearchRecordFile: File is Cyclic\n");

			/*for Cyclic file,all records will be searched, because last record connected to first record*/
			umbRecordsProcessed = 0x00;
		}

		umbRecordLength = _7816_4_getRecordLength(fhaFile);

		/*Get last updated record*/
		umbLastUpdatedRec = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbLastUpdatedRecord;

		while (umbRecordsProcessed < fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec) { // check if all records have been searched

			DBG_FUNC("[7816_4] _7816_4_EnhancedSearchRecordFile: Searching record %1 for value %1\n", umbRecordNo, umbOffsetOrValue);

			/*Caculate actual record no. from logical record no.*/
			umbActualRecordToUpdate = ((umbLastUpdatedRec + umbRecordNo - 2) % fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec);

			/*calculate offset*/
			udbFileOffset = umbActualRecordToUpdate * umbRecordLength;

			for (umbIndex = 0x00; umbIndex < (umbRecordLength - umbPatternLen); umbIndex++) {// traverse through each byte of record 

				/*Compare record byte with given value*/
				if (TRUE == mm_cmpRam(fhFile.FileSpecific.EF.maFileData, &umbOffsetOrValue, udbFileOffset + umbIndex, 0x01)) { // if record data matched

					/*value byte matches in record*/
					DBG_FUNC("[7816_4] _7816_4_EnhancedSearchRecordFile: Value byte found inside the record at offset %1\n", umbIndex);

					/*Search the whole record data from the next offset*/
					if (TRUE == _7816_4_compareWithRecord(fhaFile, umbRecordNo, umbIndex + 1, pumbPattern, umbPatternLen)) { // Pattern found in current record

						DBG_FUNC("[7816_4] _7816_4_EnhancedSearchRecordFile: Pattern matched\n");
						pumbOutput[umbOutputLen++] = umbRecordNo;
					}
					break;
				}
			}
			/*get next record*/

			(SEARCH_RECORD_FORWARD == smMode) ? umbRecordNo++ : umbRecordNo--;

			umbRecordsProcessed++; //increment the processed no. of records

			/* Below check is added for cyclic file */
			if (umbRecordNo > fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec) { //This case is only applicable for Forward search

				umbRecordNo = RECORD_FIRST;
			}
			else if (umbRecordNo == 0x00) { // this case is applicable for backward search

				umbRecordNo = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec;
			}
		}
	}

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_EnhancedSearchRecordFile() retruns %1\n", umbOutputLen);
	return umbOutputLen;
}

/*
* It returns the Parent file address of given File (DF/EF)
*
* @param fhaFile: Address of file header
*
* @return Parent file header address
* @precondition: fhaFile must be a valid file header address
*/
tFileHeaderAddr	_7816_4_getParentFile(IN_ARG tFileHeaderAddr fhaFile) {

	tFileHeader fhFile; // Stores file header info.

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getParentFile() \n");

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC("[7816_4] _7816_4_getParentFile: Read file header from given file address '%2' \n", fhaFile);

	/*Reading file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getParentFile() returns %2 \n", fhFile.fhaParent);
	return  fhFile.fhaParent; // PArent file
}

/*
* It returns the Sibling of given File (DF/EF)
*
* @param fhaFile: Address of file header
*
* @return sibling file header address
* @precondition: fhaFile must be a valid file header address
*/
tFileHeaderAddr	_7816_4_getSiblingFile(IN_ARG tFileHeaderAddr fhaFile) {

	tFileHeader fhFile; // Stores file header info.

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getSiblingFile() \n");

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	DBG_FUNC("[7816_4] _7816_4_getSiblingFile: Read file header from given file address '%2' \n", fhaFile);

	/*Reading file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getSiblingFile() returns %2 \n", fhFile.fhaSibling);
	return fhFile.fhaSibling; // Sibling file
}

/*
* It compares the given record with presented RAM data
* 
* @param fhaFile: Record file header address
* @param umbRecordNo: Record to search
* @param umbRecordOffset: Offset from where compare starts
* @param pumbData: Data to compare
* @param umbDataLength: Length of data to compare
* 
* @return TRUE if given data string present in record, otherwise FALSE.
*/
boolean _7816_4_compareWithRecord(tFileHeaderAddr fhaFile, uMonoByte umbRecordNo, uMonoByte umbRecordOffset, puMonoByte pumbData, uMonoByte umbDataLength) {

	uMonoByte		umbIndex;
	uMonoByte		umbRecordLength;
	uDiByte			udbFileOffset;
	tFileHeader		fhFile;

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_compareWithRecord() \n");

	/*check address must not be null*/
	assert(fhaFile != EMPTY_OBJECT);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC("[7816_4] _7816_4_compareWithRecord: Searching record %1 of File %2\n", umbRecordNo, fhFile.udbFileId);

	umbRecordLength = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize;

	/*Caculate actual record no. from logical record no. and then calculate offset*/
	udbFileOffset = (((fhFile.FileSpecific.EF.fileData.ofRecordFile.umbLastUpdatedRecord + umbRecordNo - 2) % fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec) * umbRecordLength) + umbRecordOffset;

	for (umbIndex = 0x00; umbIndex <= (umbRecordLength - (umbRecordOffset + umbDataLength)); umbIndex++) {// traverse through each byte of record 

		/*Compare memory data with given pattern*/
		if (TRUE == mm_cmpRam(fhFile.FileSpecific.EF.maFileData, pumbData, udbFileOffset + umbIndex, umbDataLength)) { // if record data matched

			DBG_FUNC("[7816_4] _7816_4_compareWithRecord: Data found inside the record\n");
			DBG_FUNC_EXIT("<<[7816_4] _7816_4_compareWithRecord() returns TRUE %1\n", TRUE);
			return TRUE;
		}
	}

		DBG_FUNC_EXIT("<<[7816_4] _7816_4_compareWithRecord() returns FALSE %1\n", FALSE);
		return FALSE;
}

/*
* It retrieves the Card Life Cycle State
*
* @return tCardLifeCycleState card state
*/
tCardLifeCycleState _7186_4_getCardState(void) {
	
	uMonoByte umbState = 0x00;

	DBG_FUNC_ENTRY(">>[7816_4] _7186_4_getCardState()\n");

	/*Read Card life cycle state*/
	baseReadNBytes(ADDRESS_CARD_STATE, &umbState, SIZE_OF_CARD_STATE);

	DBG_FUNC_EXIT("<<[7816_4] _7186_4_getCardState() retruns %1\n", umbState);

	return (tCardLifeCycleState)umbState;
}

/*
* It changes the Card Life Cycle State
*
* @param umbState (tCardLifeCycleState): new card state 
*/
void _7186_4_changeCardState(uMonoByte umbState) {

	DBG_FUNC_ENTRY(">>[7816_4] _7186_4_changeCardState()\n");

	/*Update Card life cycle state*/
	baseWriteNBytes(ADDRESS_CARD_STATE, &umbState, SIZE_OF_CARD_STATE);

	DBG_FUNC_EXIT("<<[7816_4] _7186_4_changeCardState() \n");
}

/*
* It searches the given File ID up in Hierarchy till we hit deadend i.e. MF or ADF
*
* @param udbFileId: file to search
* @param fhaCurrentDf: DF from where search starts
*
* @return File header address if found, otherwise NO_FILE_PRESENT
*/
tFileHeaderAddr  _7186_4_searchFileUpInHierarchy(IN_ARG uDiByte udbFileId, IN_ARG tFileHeaderAddr fhaCurrentDf) {

	tFileHeaderAddr fhaFile;
	DBG_FUNC_ENTRY(">>[7816_4] _7186_4_searchFileUpInHierarchy()\n");

	/*keep checking the file from current DF to root df (i.e. MF or ADF)*/
	while (NO_FILE_PRESENT != fhaCurrentDf) { //if there is still parent file.

		/*search for given file ID*/
		_7816_4_ifFileExists(udbFileId, fhaCurrentDf, MODE_FIND_FID, &fhaFile);

		if (fhaFile != NO_FILE_PRESENT) { // check if file found

			/*get out of the loop*/
			break;
		}

		/*get parent of current file header*/
		fhaCurrentDf = _7816_4_getParentFile(fhaCurrentDf);
	}

	DBG_FUNC_EXIT("<<[7816_4] _7186_4_searchFileUpInHierarchy() returns %2\n", fhaFile);
	return fhaFile;
}

/*
* It tells the security condition on presented file for given operation
*
* @param fhaFile: Presented File whose security to check
* @param amOperation: operation to perform  on file
*
* @returns PIN_**** (check constants.h) PIN_NEVER for Never
*										PIN_ALWAYS for Always and
*										XY for key reference (PIN_APP_1 - PIN_APP_8, PIN_ADM_1 - PIN_ADM_5, PIN_SECOND_APP_1 - PIN_SECOND_APP_8)
*/
uMonoByte _7816_4_getSecurityCondition(IN_ARG tFileHeaderAddr fhaFile, tAccessMode amOperation) {

	uMonoByte		umbIndex;
	uMonoByte		umbReturnCode = PIN_NEVER;
	tArrInfo		aiTargetFile;

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_getSecurityCondition() \n");

	/*get ARR file ID and record*/
	_7816_4_getArrInfomation(fhaFile, &aiTargetFile);

	/*Check ARR file from parent of given file*/
	fhaFile = _7816_4_getParentFile(fhaFile);

	DBG_FUNC("[7816_4] _7816_4_getSecurityCondition: Search for EFarr file %2 \n", aiTargetFile.udbArrFile);

	/*searching EFarr file*/
	fhaFile = _7186_4_searchFileUpInHierarchy(aiTargetFile.udbArrFile, fhaFile);

	/*if file not found*/
	assert(NO_FILE_PRESENT != fhaFile);

	/*ARR file must be a linear fixed file*/
	assert(IS(LF, _7816_4_getFileDescriptor(fhaFile)));

	/*if file doesnt have enough records to read the given record*/
	assert(aiTargetFile.umbArrRec <= _7816_4_getNoOfRecords(fhaFile));

	/*Read the whole record in working array buffer "gumbWorkingBuffer"*/
	_7816_4_readUpdateRecordFile(fhaFile, gumbWorkingBuffer, aiTargetFile.umbArrRec, ACC_MODE_FILE_READ);

	/*start searching given Access mode in the working buffer*/
	umbIndex = 0x00;
	while (umbIndex < (_7816_4_getRecordLength(fhaFile) - LENGTH_MINIMUM_FOR_AM_SC_DO)) {

		/*Access condition of increase operation only come inside TAG_SECURITY_AMDO_INSTRUCTION*/
		if (gumbWorkingBuffer[umbIndex] == TAG_SECURITY_AMDO_INSTRUCTION) {

			assert(gumbWorkingBuffer[umbIndex + 0x01] == LENGTH_ACCESS_MODE_DATA_OBJECT);
			umbIndex += 0x03; // skip to SC_DO

			if ((amOperation == ACC_MODE_FILE_INCREASE) && (gumbWorkingBuffer[umbIndex - 0x01] == INS_INCREASE)) { // check if value contains Increase instruction and Operation is increase
				break; //Access mode found
			}

		}
		else if (gumbWorkingBuffer[umbIndex] == TAG_SECURITY_AMDO_ACCESS_MODE_BYTE) { // TAG_SECURITY_AMDO_ACCESS_MODE_BYTE for Read/Update/deactivate and Activate

			assert(gumbWorkingBuffer[umbIndex + 1] == LENGTH_ACCESS_MODE_DATA_OBJECT);
			umbIndex += 0x03; // skip to SC_DO

			if ((ACC_MODE_FILE_INCREASE != amOperation) && (gumbWorkingBuffer[umbIndex - 0x01] & (0x01 << amOperation))) { // check if corresponding access mode bit set acc. to requested operation
				break;//Access mode found
			}

		}
		else {
			DBG_WARN("[7816_4] _7816_4_getSecurityCondition: Invalid tag %1 in ARR record\n", gumbWorkingBuffer[umbIndex]);
			umbIndex = 0x00;
			break;//Access mode not found
		}

		umbIndex += gumbWorkingBuffer[umbIndex + 1] + 0x02; // Skip SC_DO of current access mode
	}

	/*if access mode found*/
	if (umbIndex) {

		DBG_FUNC("[7816_4] _7816_4_getSecurityCondition: Access Mode Data object found \n");

		/*parse security condition*/
		if (TAG_SECURITY_SCDO_ALWAYS == gumbWorkingBuffer[umbIndex]) { // Always Access mode data object

			DBG_FUNC("[7816_4] _7816_4_getSecurityCondition: SC_DO indicates ALWAYS\n");
			umbReturnCode = PIN_ALWAYS;
		}
		else if (TAG_SECURITY_SCDO_CRT == gumbWorkingBuffer[umbIndex]) { //Control Reference Template

			/*extract key reference*/
			assert(0x06 == gumbWorkingBuffer[umbIndex + 0x01]);
			assert(TAG_KEY_REFERENCE == gumbWorkingBuffer[umbIndex + 0x02]);
			assert(LENGTH_KEY_REFERENCE == gumbWorkingBuffer[umbIndex + 0x03]);

			DBG_FUNC("[7816_4] _7816_4_getSecurityCondition: SC_DO indicates Key reference %1\n", gumbWorkingBuffer[umbIndex + 0x04]);
			umbReturnCode = gumbWorkingBuffer[umbIndex + 0x04];

		} // NEVER access mode data object or other SC_DO: results in PIN_NEVER

	}// If access mode is not found: results in PIN_NEVER

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getSecurityCondition() returns %1\n", umbReturnCode);
	return umbReturnCode;
}
/*
* It searches for given Address in the ADF List file, and return its AID.
* 
* @param fhaADF: Contains address of ADF.
* @param pumbAid: contains an array.
* 
* @return If Address matches return AID. 
*		  if Address doesnt match, return false 00.
*/ 
boolean _7816_4_getAidOfAdf(tFileHeaderAddr fhaADF, puMonoByte pumbAid)
{
	uMonoByte	      umbFileRecord;
	uMonoByte	      umbRecord;
	tFileHeaderAddr   fhaFile;
	/*Check address must not be NULL*/
	assert(fhaADF != NULL);
	assert(pumbAid != NULL);

	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);
	
	/*get memory reference of ADF list ef*/
	_7816_4_ifFileExists(FID_PROP_ADF_LIST, fhaFile, MODE_FIND_FID, &fhaFile);

	/*here we use _7816_4_SearchRecordFile() for searching AID in Prop.file */
	
	umbRecord = _7816_4_SearchRecordFile(fhaFile, //Address of file header
							(puMonoByte)&fhaADF,  //contains search pattern
							SIZE_OF_FILE_HEADER_ADDR, //search pattern length
							SEARCH_RECORD_FORWARD,    //Record no. from where search starts
							SEARCH_FIRST_OCCURRENCE,  //forward or backward
							RECORD_FIRST,       //Search occurrence: First or ALL
							OFFSET_ADDR_FILE_HEADER,      //absoulte position in record for search
							&umbFileRecord);
	
	/*if pattern matched with any record*/
	if (0x00 != umbRecord)
	{
		/*Read record*/
		_7816_4_readUpdateRecordFile(fhaFile, pumbAid, umbFileRecord, ACC_MODE_FILE_READ);
		DBG_FUNC("[7816_4] _7816_4_getAidOfAdf: Data found inside the record\n");
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_getAidOfAdf() returns TRUE %1\n", TRUE);
		return TRUE;
	}

	/*pattern not matched with any record*/
	DBG_FUNC_EXIT("<<[7816_4] _7816_4_getAidOfAdf() returns FALSE %1\n", FALSE);
	return FALSE;
		
		
}