#include "102_226.h"

//Variables definition
#define TAR_LENGTH								(0x03)
#define FULL_ACCESS_DOMAIN						(0xFFFFFF)
#define NO_ACCESS_DOMAIN						(0x000000)
#define OFFSET_START_TAR						(0x04)

uDiByte gudbCommandIndex;

/*
*RFM Parameters
*It search the given TAR with Different TAR's and return Acess_Domain and MSL
*@fhaTAR It's Containes the TAR
*@pumbrfmpr It's Containes the Access_Domain, MSL
*@pumbrfmprRecord It's Containes the Access_Domain, MSL and TAR
*/
boolean _102226_RfmParameters(IN_ARG puMonoByte fhaTAR, OUT_ARG puMonoByte pumbrfmpr) {

	tFileHeaderAddr   fhaFile;
	uMonoByte	      umbRecord;
	uMonoByte	      umbFileRecord;
	uMonoByte		  pumbrfmprRecord[0x07];
	uMonoByte		  umbIndex;

	/*Check address must not be NULL*/
	assert(fhaTAR != NULL);
	assert(pumbrfmpr != NULL);

	DBG_FUNC_ENTRY(">>[102226] _102226_RfmParameters() \n");

	/*Get file address of Prop. DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	/*Get file address of Prop. Rfm Parameters file*/
	_7816_4_ifFileExists(FID_PROP_RFM_PARAMETER, fhaFile, MODE_FIND_FID, &fhaFile);

	DBG_FUNC(">>[102226] _102226_RfmParameters() Get Rfm Parameters file \n");

	umbRecord = _7816_4_SearchRecordFile(fhaFile,						//Address of file header which contains TAR
										fhaTAR,							//contains TAR
										(uMonoByte)TAR_LENGTH,			//search pattern length of TAR
										SEARCH_RECORD_FORWARD,			//Search mode : forward or backward
										SEARCH_FIRST_OCCURRENCE,		//Search occurrence: First or ALL
										RECORD_FIRST,					//Record no. from where search starts
										(uMonoByte)OFFSET_START_TAR,	//absoulte position in record for search
										&umbFileRecord);				//Stores list of records matched with given pattern

	/*If pattern(TAR) is not match with any record*/
	if (0x00 == umbRecord)
	{
		DBG_WARN("<<[102226] _102226_RfmParameters() returns FALSE %1\n", FALSE);
		return FALSE;
	}
	/*If TAR matched then Read Record( that's Contained Access Domain, MSL and TAR) */
	_7816_4_readUpdateRecordFile(fhaFile, pumbrfmprRecord, umbFileRecord, ACC_MODE_FILE_READ);
	DBG_FUNC("[102226] _102226_RfmParameters() Found RFM Parameters Access Domain, MSL and TAR \n");

	/*Store Access Domain and MSL in the @pumbrfmpr */
	
	for (umbIndex = 0x00; umbIndex <= 0x03;umbIndex++) {
		pumbrfmpr[umbIndex] = pumbrfmprRecord[umbIndex];		
	}

	DBG_FUNC("[102226] _102226_RfmParameters() Return Access Domain and MSL \n");	
	DBG_FUNC_EXIT("<<[102226] _102226_RfmParameters() returns TRUE %1\n", TRUE);
	return TRUE;
}