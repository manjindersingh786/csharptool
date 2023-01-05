
#ifndef _7816_4_EXTERN_H
#define _7816_4_EXTERN_H
#include "types.h"
#include "MemoryManager_extern.h"

/*
* Macro defines
*/
#define LENGTH_WORKING_BUFFER										(0x100)
#define DEFAULT_BYTE_IN_FILE										(0xFF) //Default byte of memory
#define NO_FILE_PRESENT												(EMPTY_OBJECT)
#define OFFSET_START												(0x00)
#define MODE_FILE_READ												(MEM_MODE_READ)
#define MODE_FILE_UPDATE											(MEM_MODE_UPDATE)
#define SIZE_OF_FILE_HEADER_ADDR									(0x02) // must be sync with tFileHeaderAddr
#define SIZE_OF_CARD_STATE											(0x01)
#define RECORD_UNDEFINED											(0x00)
#define RECORD_FIRST												(0x01)
#define MODE_FIND_FID												(0x00)
#define MODE_FIND_SFID												(0x01)
#define FILE_DESC_DF												(0x38)
#define FILE_DESC_CF												(0x06)
#define FILE_DESC_LF												(0x02)
#define FILE_DESC_TF												(0x01)
#define MASK_FILE_DF												(0xBF)
#define MASK_FILE_EF												(0x87)
#define MASK_FILE_CF												(MASK_FILE_EF)
#define MASK_FILE_LF												(MASK_FILE_EF)
#define MASK_FILE_TF												(MASK_FILE_EF)
#define LENGTH_MINIMUM_FOR_AM_SC_DO									(0x06)

//Value of below Depends on tAccessMode enum
#define NO_OF_FILE_OPERATIONS										(0x05)

#define SFI_NOT_SUPPORTED											(0x1F)
//conditional macros
// x can be DF,CF,TF and LF only
#define IS(x,y)														((y & MASK_FILE_##x) == FILE_DESC_##x)

/*
* Typedefs
*/
/*
* Mapping of key references to below type variable
* Bits-> |  08  |  07  |  06  |  05  |  04  |  03  |  02  |  01  |
* Keys-> |  08  |  07  |  06  |  05  |  04  |  03  |  02  |  01  |
*
* Bits-> |  16  |  15  |  14  |  13  |  12  |  11  |  10  |  09  |
* Keys-> |  XX  |  XX  |  0E  |  0D  |  0C  |  0B  |  0A  |  XX  |
*
* Bits-> |  24  |  23  |  22  |  21  |  20  |  19  |  18  |  17  |
* Keys-> |  88  |  87  |  86  |  85  |  84  |  83  |  82  |  81  |
*
* Bits-> |  32  |  31  |  30  |  29  |  28  |  27  |  26  |  25  |
* Keys-> |  ALW |  XX  |  8E  |  8D  |  8C  |  8B  |  8A  |  XX  |
*/
typedef uQuadByte tPinData;
typedef tPinData* ptPinData;

extern tPinData	pdPinStatus;

/*Card life cycle state acc. to GP 2.3.1*/
typedef enum {
	STATE_OP_READY = 0x01,
	STATE_INITIALIZED = 0x07,
	STATE_SECURED = 0x0F,
	STATE_CARD_LOCKED = 0x7F,
	STATE_TERMINATED = 0xFF
}tCardLifeCycleState;

// Security condition on Access mode of Files
typedef enum {
	ACC_MODE_FILE_READ, // its value must be same as MEM_MODE_READ, if not please update code acc. where its used
	ACC_MODE_FILE_UPDATE, // its value must be same as MEM_MODE_UPDATE, if not please update code acc. where its used
	ACC_MODE_FILE_INCREASE,
	ACC_MODE_FILE_DEACTIVATE,
	ACC_MODE_FILE_ACTIVATE
} tAccessMode;

//File life cycle status inetger LCSI 
typedef enum {
	LCSI_CREATED = 0x01,
	LCSI_INIT = 0x03,
	LCSI_ACTIVATED = 0x05,
	LCSI_DEACTIVATED = 0x04,
	LCSI_TERMINATED = 0x0C
}tFileLCSI;

//File search modes
typedef enum {
	SEARCH_RECORD_FORWARD,
	SEARCH_RECORD_BACKWARD
}tSearchMode;

//Occurrences
typedef enum {
	SEARCH_FIRST_OCCURRENCE,
	SEARCH_ALL_OCCURRENCES
}tSearchOccurrence;

//Occurrences
typedef enum {
	SEARCH_BY_OFFSET,
	SEARCH_BY_VALUE
}tEnhancedSearchMechanism;

//Pin related 

/*Pin verification status*/
typedef enum {
	ACC_VERIFIED,
	ACC_NOT_VERIFIED
} tAccessResult;

/*Indicates status of pin if its enable or disable*/
typedef enum {
	PIN_ENABLED,
	PIN_DISABLED
}tPinState;

//Pin Operations
typedef enum{
	PIN_OP_VERIFY,
	PIN_OP_ENABLE,
	PIN_OP_DISABLE,
	PIN_OP_UNBLOCK,
	PIN_OP_CHANGE,
	PIN_OP_VERIFY_COUNTER,
	PIN_OP_UNBLOCK_COUNTER,
}tPinOperation;

//Pin Status code
typedef enum {
	PIN_SC_VERIFIED,
	PIN_SC_NOT_FOUND,
	PIN_SC_INVALID_STATE,
	PIN_SC_BLOCKED,
	PIN_SC_NOT_VERIFIED, // zero Attempts left
	/*Below status code onwards will not be sent from any of the function, they are just mentioned to explain the meaning of status code value if its more than above status code*/
	PIN_SC_NOT_VERIFIED_1_ATTEMPT_LEFT, // i.e. PIN_SC_NOT_VERIFIED + 0x01: 1 Attempt left
	PIN_SC_NOT_VERIFIED_2_ATTEMPTS_LEFT // i.e. PIN_SC_NOT_VERIFIED + 0x02: 2 Attempts left
	/*..... and so on*/
}tPinStatusCode;

/*Memory type address for File header*/
typedef tMemAddr	tFileHeaderAddr;
typedef tMemAddr*	ptFileHeaderAddr;

/*Security Reference format '8B'*/
typedef struct {
	uDiByte		udbArrFile;
	uMonoByte	umbArrRec;
}tArrInfo;
typedef tArrInfo* ptArrInfo;

//#pragma pack(1) // TODO: pack below structure members on 1-byte alignment

typedef struct {
	/*Record of EFarr poninting access conditions*/
	uMonoByte umbEFarrRec; //0

	/*File descriptor byte*/
	uMonoByte umbFileDesc; //1

	/*Life Cycle status integer*/
	tFileLCSI flState; //2

	/*Pointer to parent file header*/
	tFileHeaderAddr	fhaParent; //3

	/*Pointer to immediate sibling file header*/
	tFileHeaderAddr	fhaSibling; //5

	/*File ID*/
	uDiByte		udbFileId; //7

	/*File ID of EFarr which contains access conditions of this EF*/
	uDiByte		udbEFarrId; //9

	union {
		struct {
			/*Pointer to Child file*/
			tFileHeaderAddr	fhaChild; //11

			/*Pins used under this DF: Pin Status Template DO*/
			tPinData pdPstdo;

		}DF;

		/*EF attributes*/
		struct {
			/*Pointer to file data*/
			tMemAddr	maFileData; //11

			/*Store File size, record related data and Last udpated record*/
			union {
				/*stores file size of transparent file*/
				uDiByte udbFileSize; //13

				/*stores file size of Record file*/
				struct {

					/* Last updated record of Cyclic file*/
					uMonoByte	umbLastUpdatedRecord; //13

					/*Number of records in given file*/
					uMonoByte	umbNoOfRec; //14

					/*Size of each record*/
					uMonoByte	umbRecSize; //15
				}ofRecordFile;

			}fileData;

			/*Short file identfier*/
			uMonoByte	umbSfi : 5; //16

			/*Update Activity bit*/
			uMonoByte	umbUpdAct : 1; //16

			/*Read update when file is deactivated*/
			uMonoByte	umbReadUpdateWhenDeactivated : 1; //16

			/*If file is OTA (Over-the-air) updateable or not
			* 0: Not updateable ; 1: Updateable
			*/
			uMonoByte	umbOtaUpdateable : 1; //16

		}EF;

	}FileSpecific;

}tFileHeader, * ptFileHeader; //size: 17

//#pragma pack() // reset above packing setting

/*
* Variables declaration
*/
extern uMonoByte gumbWorkingBuffer[LENGTH_WORKING_BUFFER];
/*
* Functions declaration
*/
/*
* It searches for given AID in the ADF directory file, and return its header address
*
* @param pumbAid: Contains AID, 1st byte indicates length and remaining AID bytes
* @param pfhaFile: if an ADF found, it contains Address of that file.'
*
* @return If AID matches return record no.
*		  if EOF reached return return 0x00 i.e. invalid record no.
*		  if AID doesnt match and encounter FF before EOF, return record no.
*/
extern uMonoByte _7816_4_ifAdfExists(IN_ARG puMonoByte pumbAid, OUT_ARG ptFileHeaderAddr pfhaFile);

/*
* It searches for given File ID in the given, and return its header address
*
* @param udbFileId: Contains File ID
* @param fhaDf: Address of DF under which search being done
* @param pfhaFile: if a File found, it contains Address of that file.
*/
extern void _7816_4_ifFileExists(IN_ARG uDiByte udbFileId, IN_ARG tFileHeaderAddr fhaDf, IN_ARG uMonoByte umbSearchMode, OUT_ARG ptFileHeaderAddr pfhaFile);

/*
* It returns Address of MF
*
*/
extern tFileHeaderAddr _7816_4_getMfAddress(void);

/*
* It searches the given File ID up in Hierarchy till we hit deadend i.e. MF or ADF
*
* @param udbFileId: file to search
* @param fhaCurrentDf: DF from where search starts
*
* @return File header address if found, otherwise NO_FILE_PRESENT
*/
extern tFileHeaderAddr  _7186_4_searchFileUpInHierarchy(IN_ARG uDiByte udbFileId, IN_ARG tFileHeaderAddr fhaCurrentDf);

/*
* It read the file header from given File header address
*
* @param fhaFile: Address of file whose header to read
* @param pfhFile: Read the file
*
* @precondition: fhaFile must be a valid file header address
*/
extern void _7816_4_getFileHeader(IN_ARG		tFileHeaderAddr fhaFile, OUT_ARG		ptFileHeader	pfhFile);

/*
* It read the file ID from given File header address
*
* @param fhaFile: Address of file whose header to read
* @param pfhFile: Read the file
*
* @precondition: fhaFile must be a valid file header address
*/
extern uDiByte _7816_4_getFileId(IN_ARG		tFileHeaderAddr fhaFile);

/*
* It returns the File descriptor byte of file
*
* @param fhaFile: Address of file header
*
* @return File descriptor byte
*/
extern uMonoByte	_7816_4_getFileDescriptor(tFileHeaderAddr fhaFile);

/*
* It returns the File descriptor byte of file
*
* @param fhaFile: Address of file header
*
* @return File descriptor byte
* @precondition: fhaFile must be a valid file header address of Transparent File
*/
extern uDiByte _7816_4_getFileSize(tFileHeaderAddr fhaFile);

/*
* It update/read the data of given File header address
*
* @param fhaFile: Address of file that need to update
* @param pumData: If mode Update, Data will be read from this array
*					If mode Read, data will be stored in this array
* @param udbFileOffset: offset from/to  where data read/update
* @param udbDataLength: Length of data to update/read
* @param umbMode: Operation mode : read or update
*
* @precondition: fhaFile must be an EF
*/
extern void _7816_4_readUpdateFile(IN_ARG		tFileHeaderAddr fhaFile,\
					INOUT_ARG	puMonoByte pumData,\
					IN_ARG		uDiByte udbFileOffset,\
					IN_ARG		uDiByte udbDataLength,\
					IN_ARG		tAccessMode amMode);

/*
* It returns the Record length of given file
*
* @param fhaFile: Address of file header
*
* @return Record length
* @precondition: fhaFile must be a valid file header address of Transparent File
*/
extern uMonoByte _7816_4_getRecordLength(tFileHeaderAddr fhaFile);

/*
* It returns the No. of records of given file
*
* @param fhaFile: Address of file header
*
* @return No of records
* @precondition: fhaFile must be a valid file header address of Linear fixed or Cyclic File
*/
extern uMonoByte _7816_4_getNoOfRecords(tFileHeaderAddr fhaFile);

/*
* It returns the last updated record of given file
*
* @param fhaFile: Address of file header
*
* @return last updated record
* @precondition: fhaFile must be a valid file header address of Linear fixed or Cyclic File
*/
extern uMonoByte _7816_4_getLastUpdatedRecord(tFileHeaderAddr fhaFile);

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
extern uMonoByte _7816_4_readUpdateRecordFile(	IN_ARG		tFileHeaderAddr fhaFile,
							INOUT_ARG	puMonoByte pumData,
							IN_ARG		uMonoByte umbRecordNo,
							IN_ARG		tAccessMode amMode);
/*
* It Sets the LCSI byte for given EF
*
* @param fhaFile: Address of file header
* @param flState: Life Cycle state integer
*
* @precondition: fhaFile must be a valid file header address of an EF
*/
extern void _7816_4_setLifeCycleStatus(tFileHeaderAddr fhaFile, tFileLCSI flState);

/*
* It get the Life cycle state of given EF
*
* @param fhaFile: Address of file header
*
* @precondition: fhaFile must be a valid file header address of an EF
*/
extern tFileLCSI _7816_4_getLifeCycleStatus(tFileHeaderAddr fhaFile);

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
extern uMonoByte _7816_4_SearchRecordFile(	IN_ARG	tFileHeaderAddr		fhaFile,
											IN_ARG	puMonoByte			pumbPattern,
											IN_ARG	uMonoByte			umbPatternLen,
											IN_ARG	tSearchMode			smMode,
											IN_ARG	tSearchOccurrence	soOccur,
											IN_ARG	uMonoByte			umbRecordNo,
											IN_ARG	uMonoByte			umbOffset,
											OUT_ARG	puMonoByte			pumbOutput);

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
extern uMonoByte _7816_4_EnhancedSearchRecordFile(	IN_ARG	tFileHeaderAddr				fhaFile,
													IN_ARG	puMonoByte					pumbPattern,
													IN_ARG	uMonoByte					umbPatternLen,
													IN_ARG	tSearchMode					smMode,
													IN_ARG	tEnhancedSearchMechanism	esmMode,
													IN_ARG	uMonoByte					umbRecordNo,
													IN_ARG	uMonoByte					umbOffsetOrValue,
													OUT_ARG	puMonoByte					pumbOutput );

/*
* It returns the Sibling of given File (DF/EF)
*
* @param fhaFile: Address of file header
*
* @return sibling file header address
* @precondition: fhaFile must be a valid file header address
*/
extern tFileHeaderAddr	_7816_4_getSiblingFile(IN_ARG tFileHeaderAddr fhaFile);

/*
* It returns the Parent file address of given File (DF/EF)
*
* @param fhaFile: Address of file header
*
* @return Parent file header address
* @precondition: fhaFile must be a valid file header address
*/
extern tFileHeaderAddr	_7816_4_getParentFile(IN_ARG tFileHeaderAddr fhaFile);
/*
* It read the file data address from given File header address
*
* @param fhaFile: file whose data address to read
*
* @precondition: fhaFile must be a valid EF header address
*/
extern tMemAddr _7816_4_getFileDataAddress(IN_ARG tFileHeaderAddr fhaFile);

/*
* It returns the ARR file and Arr record
*
* @param fhaFile: Address of file header
* @param paiFile: Arr file id and record will be returned through this param
*
* @precondition: fhaFile must be a valid file header address
*/
extern void _7816_4_getArrInfomation(IN_ARG tFileHeaderAddr fhaFile, OUT_ARG ptArrInfo paiFile);
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
extern boolean _7816_4_compareWithRecord(tFileHeaderAddr fhaFile, uMonoByte umbRecordNo, uMonoByte umbRecordOffset, puMonoByte pumbData, uMonoByte umbDataLength);

/*
* It retreievs the Card Life Cycle State
*
* @return tCardLifeCycleState card state
*/
extern tCardLifeCycleState _7186_4_getCardState(void);


/*
* It changes the Card Life Cycle State
*
* @param umbState (tCardLifeCycleState): new card state
*/
extern void _7186_4_changeCardState(uMonoByte umbState);


/*
* It tells the security condition on presented file for given operation
*
* @param fhaFile: Presented File whose security to check
* @param amOperation: operation to perform  on file
*
* @returns PIN_**** (check constants.h) FF for Never 90 for Always and XY for key reference
*/
extern uMonoByte _7816_4_getSecurityCondition(IN_ARG tFileHeaderAddr fhaFile, tAccessMode amOperation);
//***************************************************** Pin functions

/*
* Check the reference data or P2 parameter if its invalid acc. to 102221
*
* @param umbReferenceData: Key reference data
*
* @retruns TRUE if key reference is invalid, FALSE if its not
*/
extern uMonoByte _7816_4_isInvalidReferenceData(uMonoByte umbReferenceData);

/*
* Set the corresponding bit in give Pin data variable as per key reference
*
* @param uqbPinData: PIN data
* @param umbKeyRef: Key reference
*
* @precondition Must be a valid key reference i.e. 01-08, 81-88, 0A-0E and 8A-8E
*/
extern void _7816_4_setKeyRef(puQuadByte puqbPinData, uMonoByte umbKeyRef);

/*
* Update the status of given PIN as verified
*
* @param umbKeyRef: Key reference
*
* @precondition Must be a valid key reference i.e. 01-08, 81-88, 0A-0E and 8A-8E
*/
extern void _7816_4_markKeyAsVerified(uMonoByte umbKeyRef);

/*
* It implements the Change and Unblock Pin function
*
* @param umbKeyReference: identifies the key reference, upon which operation to perform.
* @param poMode: Pin operation
* @param pumbPin: Stores Current pin value in case of PIN_OP_CHANGE operation
*					Stores Unblock PIN value in case of PIN_OP_UNBLOCK operation
* @param pumbNewKey: New key to set corresponding to given key reference
*
* @return tPinStatusCode tells if operation succedded or not
* @precondition: assuming length of pumbPin and pumbNewKey is enough to read data
*/
extern tPinStatusCode _7816_4_changeUnblockPin(IN_ARG uMonoByte umbKeyReference,
												IN_ARG tPinOperation poMode,
												IN_ARG puMonoByte pumbPin,
												IN_ARG puMonoByte pumbNewKey);

/*
* It implements the Verify Pin function
*
* @param umbKeyReference: identifies the key reference, upon which operation to perform
* @param psExpected: What state does Requester expect from the PIN to perform an operation
* @param pumbPin: Stores presented pin value
* @param poFor: Pin operation  PIN_OP_VERIFY or PIN_OP_VERIFY_COUNTER
*
* @return tPinStatusCode tells if operation succedded or not
* @precondition: assuming length of pumbPin is enough to read data
*/
extern tPinStatusCode _7816_4_verifyPin(IN_ARG uMonoByte	umbKeyReference,
										IN_ARG tPinState	psExpected,
										IN_ARG puMonoByte	pumbPin,
										IN_ARG tPinOperation poFor);

/*
* It implements the Enable & Disable PIN function
*
* @param umbKeyReference: identifies the key reference, upon which operation to perform.
* @param poMode: Pin operation : PIN_OP_ENABLE or PIN_OP_DISABLE
* @param pumbPin: Pin value presented
*
* @return tPinStatusCode tells if operation succedded or not
* @precondition: assuming length of array pumbPin is enough to read data, operation can only be PIN_OP_ENABLE or PIN_OP_DISABLE
*/
extern tPinStatusCode _7816_4_changePinState(IN_ARG	uMonoByte		umbKeyReference,
											IN_ARG	tPinOperation	poCommand,
											IN_ARG	puMonoByte		pumbPin);

/*
* It returns the remaining attempts of given key reference
*
* @param umbKeyRef: Key reference
* @param poFor: P
*
* @return remaining attempts
* @precondition umbKeyReference must be valid key reference
*/
extern uMonoByte _7816_4_remainingAttemptsOfPin(IN_ARG uMonoByte umbKeyReference, IN_ARG tPinOperation poFor);

/*
* It returns the activation status of the corresponding pin
*
* @param umbKeyRef: Key reference
*
* @return Enabled pin: PIN_ENABLED otherwise PIN_DISABLED
*/
extern tPinState _7816_4_isPinActive(uMonoByte umbKeyReference);

/*
* Returns the the verification status of given key reference
*
* @param umbKeyRef: Key reference
*
* @return ACC_VERIFIED if pin verified, otherwise ACC_NOT_VERIFIED
* @precondition Must be a valid key reference i.e. 01-08, 81-88, 0A-0E, 8A-8E and 90
*/
extern tAccessResult _7816_4_isKeyRefVerified(uMonoByte umbKeyRef);
/*
* It searches for given Address in the ADF List file, and return its AID.
*
* @param fhaADF: Contains address of ADF.
* @param pumbAid: contains an array.
*
* @return If Address matches return AID.
*		  if Address doesnt match, return false 00.
*/
extern boolean _7816_4_getAidOfAdf(tFileHeaderAddr fhaADF, puMonoByte pumbAid);
#endif // !_7816_4_EXTERN_H
