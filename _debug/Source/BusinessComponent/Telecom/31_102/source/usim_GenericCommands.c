#include"31_102.h"

/*
* Macros
*/
#define P2_USIM_GSM_CONTEXT																(0x00)
#define P2_USIM_3G_CONTEXT																(0x01)
#define P2_APPLICATION_SPECIFIC_3G_CONTEXT												(P2_APPLICATION_SPECIFIC|P2_USIM_3G_CONTEXT)
#define P2_APPLICATION_SPECIFIC_GSM_CONTEXT												(P2_APPLICATION_SPECIFIC|P2_USIM_GSM_CONTEXT)
#define LENGTH_AUTH_SQN																	(0x06)
#define LENGTH_AUTH_AMF																	(0x02)
#define LENGTH_AUTH_MAC																	(0x08)
#define LENGTH_AUTH_AK																	(0x08)
#define LENGTH_BYTE_OF_RANDOM_DATA														(0x01)
#define LENGTH_BYTE_OF_AUTN_DATA														(0x01)
#define LENGTH_AUTH_RES																	(0x08)
#define LENGTH_AUTH_KC																	(0x08)
#define LENGTH_AUTH_OPC																	(LENGTH_AUTH_KEY)
#define LENGTH_AUTH_CK																	(LENGTH_AUTH_KEY)
#define LENGTH_AUTH_IK																	(LENGTH_AUTH_KEY)
#define LENGTH_AUTH_AUTN																(LENGTH_AUTH_SQN + LENGTH_AUTH_AMF + LENGTH_AUTH_MAC)
#define P3_LENGTH_USIM_AUTHENTICATE														(LENGTH_BYTE_OF_RANDOM_DATA + LENGTH_CHALLENGE_DATA + LENGTH_BYTE_OF_AUTN_DATA + LENGTH_AUTH_AUTN)
#define LENGTH_OF_AUTS																	(LENGTH_AUTH_SQN + LENGTH_AUTH_MAC)

/*3G context parameters offset in Command data*/
#define OFFSET_AUTH_RAND																(LENGTH_BYTE_OF_RANDOM_DATA) // first byte states length of Random Data, hence Random starts from index 01
#define	OFFSET_AUTH_AUTN																(OFFSET_AUTH_RAND + LENGTH_CHALLENGE_DATA + LENGTH_BYTE_OF_AUTN_DATA)
#define OFFSET_AUTH_SQN_X_AK															(OFFSET_AUTH_AUTN)
#define OFFSET_AUTH_AMF																	(OFFSET_AUTH_SQN_X_AK + LENGTH_AUTH_SQN)
#define OFFSET_AUTH_MAC																	(OFFSET_AUTH_AMF + LENGTH_AUTH_AMF)

/*3G context parameters offset in Response data*/
#define OFFSET_AUTH_RESULT_TAG															(0x00)
#define OFFSET_AUTH_RES_LENGTH															(OFFSET_AUTH_RESULT_TAG + 0x01)
#define OFFSET_AUTH_RES																	(OFFSET_AUTH_RES_LENGTH + 0x01) // first byte states length of Random Data, hence Random starts from index 01
#define	OFFSET_AUTH_CK_LENGTH															(OFFSET_AUTH_RES + LENGTH_AUTH_RES)
#define OFFSET_AUTH_CK																	(OFFSET_AUTH_CK_LENGTH + 0x01)
#define OFFSET_AUTH_IK_LENGTH															(OFFSET_AUTH_CK + LENGTH_AUTH_CK)
#define OFFSET_AUTH_IK																	(OFFSET_AUTH_IK_LENGTH + 0x01)
#define OFFSET_AUTH_KC_LENGTH															(OFFSET_AUTH_IK + LENGTH_AUTH_IK)
#define OFFSET_AUTH_KC																	(OFFSET_AUTH_KC_LENGTH + 0x01)

#define LENGTH_AUTH_RESPONSE_WITHOUT_KC													(OFFSET_AUTH_IK + LENGTH_AUTH_IK)
#define LENGTH_AUTH_RESPONSE_WITH_KC													(OFFSET_AUTH_KC + LENGTH_AUTH_KC)

/*AUTS*/
#define OFFSET_AUTH_AUTS_LENGTH															(OFFSET_AUTH_RESULT_TAG + 0x01)
#define OFFSET_AUTH_AUTS_CONC_SQNMS														(OFFSET_AUTH_AUTS_LENGTH + 0x01)
#define OFFSET_AUTH_AUTS_MACS															(OFFSET_AUTH_AUTS_CONC_SQNMS + LENGTH_AUTH_SQN)
#define LENGTH_AUTH_AUTS																(OFFSET_AUTH_AUTS_MACS + LENGTH_AUTH_MAC)



#define INDEX_LAST_BYTE_OF_SQN															(0x05)
#define MASK_AUTH_SEQ_INDEX																(0x1F)

/*6F15 EF SQN array info file offsets*/
#define OFFSET_AUTH_EF_SQN_INFO															(0x00)
#define OFFSET_AUTH_EF_SQN_FIRST_SEQ													(0x01)

#define AUTH_NO_OF_SAVED_SQN															(0x20)
#define LENGTH_AUTH_EF_SQN_INFO															(0x01)
#define LENGTH_AUTH_EF_SQN_FILE_SIZE													(0xC1) // i.e. 193 = 36 Seqs (each of size 6 bytes) + 1 byte for SQN info

#define OFFSET_AUTH_SECOND_HALF															(0x08)
#define NAA_IS_USIM_GSM																	(0x00)
#define NAA_IS_ISIM																		(0x01)

#define LENGTH_MIN_ISIM_AID																(0x07)
/*
* Check present service number in UST if its available or not
* @param umbServiceNo: Service number to check
* @pre Service no. must be greater than 1 , ADF must be selected and UST must be available under it
* @ret TRUE if its available otherwise FALSE
*/
static boolean _31102_checkServiceInUST(uMonoByte umbServiceNo) {

	tFileHeaderAddr fhaUST;
	uMonoByte		umbServiceByte;

	DBG_FUNC_ENTRY(">>[31_102] _31102_checkServiceInUST() \n");

	assert((pccCurrentChannel->fhaCurrentDf != NO_FILE_PRESENT) && (umbServiceNo > 0x00));

	/*Get file address of EF UST file*/
	_7816_4_ifFileExists(FID_UST, pccCurrentChannel->fhaCurrentDf, MODE_FIND_FID, &fhaUST);

	assert(fhaUST != NO_FILE_PRESENT);

	/*in order to calculate offset, first decrement the given no. by 1*/
	umbServiceNo--;

	/*read service corresponding byte*/
	_7816_4_readUpdateFile(fhaUST, &umbServiceByte, SHIFT_RIGHT_BY_N_BITS(umbServiceNo, 0x03), 0x01, ACC_MODE_FILE_READ);

	/*check if service is set*/
	if (umbServiceByte & (SHIFT_LEFT_BY_N_BITS(0x01, (umbServiceNo & 0x07)))) {

		DBG_WARN("[31_102] _31102_checkServiceInUST: Requested Service No. %1 is available", (umbServiceNo+1));
		DBG_FUNC_EXIT("<<[31_102] _31102_checkServiceInUST() returns FALSE\n");
		return TRUE;
	}

	DBG_WARN("[31_102] _31102_checkServiceInUST: Requested Service No. %1 is not available", (umbServiceNo + 1));
	DBG_FUNC_EXIT("<<[31_102] _31102_checkServiceInUST() returns FALSE\n");
	return FALSE;
}


/*
* Add two SQNs
* @param pumbSQN1: SQN of size 6
* @param pumbSQN2: SQN of size 6
* @param pumbSQN2: Stores Result bytes on adding SQNs
* @ret void
*/
static void _31102_addSqns(puMonoByte pumbSQN1, puMonoByte pumbSQN2, puMonoByte pumbResult) {

	/*TODO this function is needed when applying Delta and Limit restriction*/
}

/*
* Compare two SQNs
* @param pumbSQN1: SQN of size 6
* @param pumbSQN2: SQN of size 6
* @ret returns TRUE if SQN1 > SQN2 otherwise FALSE
*/
static boolean _31102_compareSqn(puMonoByte pumbSQN1, puMonoByte pumbSQN2) {

	uMonoByte umbIndex;

	DBG_FUNC_ENTRY(">>[31_102] _31102_compareSqn() \n");

	/*Total No. of Bits in SQN is 43 i.e. first 5 bytes can be compared as it is but for last byte only 3 MSBs will be compared*/

	/*Compare first 5 bytes of SQN*/
	for (umbIndex = 0x00; umbIndex < (LENGTH_AUTH_SQN - 0x01); umbIndex++) {

		/*If SQN1 > SQN2*/
		if (pumbSQN1[umbIndex] > pumbSQN2[umbIndex]) {

			DBG_WARN("[31_102] _31102_compareSqn: Byte %1 indicates SQN1 > SQN2", umbIndex + 1);
			DBG_FUNC_EXIT("<<[31_102] _31102_compareSqn() returns TRUE\n");
			return TRUE;
		}
		/*If SQN1 < SQN2*/
		else if (pumbSQN1[umbIndex] < pumbSQN2[umbIndex]) {
			
			DBG_WARN("[31_102] _31102_compareSqn: Byte %1 indicates SQN1 < SQN2", umbIndex + 1);
			DBG_FUNC_EXIT("<<[31_102] _31102_compareSqn() returns FALSE\n");
			return FALSE;
		}
	}

	/*If control is here it means first 5 bytes are equal now final decision comes to last byte of SQN*/
	/*if SQN1(3 MS bits) > SQN2(3 MS bits) */
	if ((SHIFT_RIGHT_BY_N_BITS(pumbSQN1[INDEX_LAST_BYTE_OF_SQN], 0x05)) > (SHIFT_RIGHT_BY_N_BITS(pumbSQN2[INDEX_LAST_BYTE_OF_SQN], 0x05))) {

		DBG_WARN("[31_102] _31102_compareSqn: Byte %1 indicates SQN1 > SQN2", INDEX_LAST_BYTE_OF_SQN+1);
		DBG_FUNC_EXIT("<<[31_102] _31102_compareSqn() returns TRUE\n");
		return TRUE;
	}

	DBG_WARN("[31_102] _31102_compareSqn: SQN1 <= SQN2");
	DBG_FUNC_EXIT("<<[31_102] _31102_compareSqn() returns FALSE\n");
	return FALSE;
}

/*
* It checks the Freshness of SQN
* @param pumbSQN: Incoming SQN
* @ret returns: 
*				TRUE if SQN is verified successfully
*				FALSE if SQN is not verified and pumbSQN will be updated with SQNms
*/
static boolean _31102_verifyFreshnessOfSqn(puMonoByte pumbSQN) {

	uMonoByte			umbSqnMs[LENGTH_AUTH_SQN];
	uMonoByte			umbSqnMs_i[LENGTH_AUTH_SQN];
	uMonoByte			umbMaxIndex;
	tFileHeaderAddr		fhaPropDf;
	tFileHeaderAddr		fhaPropFile;

	DBG_FUNC_ENTRY(">>[31_102] _31102_verifyFreshnessOfSqn() \n");

	DBG_FUNC("[31_102] _31102_verifyFreshnessOfSqn: Searching prop. Auth Ki file  \n");

	/*Get file address of Prop. DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaPropDf);

	assert(fhaPropDf != NO_FILE_PRESENT);

	/*Get file address of Prop. Auth Sequence Array file*/
	_7816_4_ifFileExists(FID_PROP_AUTH_SEQ_ARRAY, fhaPropDf, MODE_FIND_FID, &fhaPropFile);

	assert((fhaPropFile != NO_FILE_PRESENT) && (LENGTH_AUTH_EF_SQN_FILE_SIZE == _7816_4_getFileSize(fhaPropFile)));

	DBG_FUNC("[31_102] _31102_verifyFreshnessOfSqn: Reading index of SQNms \n");

	/*Read the SQNms index*/
	_7816_4_readUpdateFile(fhaPropFile, &umbMaxIndex, OFFSET_AUTH_EF_SQN_INFO, LENGTH_AUTH_EF_SQN_INFO, ACC_MODE_FILE_READ);

	/*Read the SQNms(i) where i is index from incoming pumbSQN */
	_7816_4_readUpdateFile(fhaPropFile, umbSqnMs_i, (OFFSET_AUTH_EF_SQN_INFO + ((pumbSQN[INDEX_LAST_BYTE_OF_SQN] & MASK_AUTH_SEQ_INDEX) * LENGTH_AUTH_SQN)), LENGTH_AUTH_SQN, ACC_MODE_FILE_READ);

	DBG_FUNC("[31_102] _31102_verifyFreshnessOfSqn: SQNms(i):\n");
	DBG_PRINT_ARRAY(umbSqnMs_i, LENGTH_AUTH_SQN);

	/*Read the SQNms*/
	_7816_4_readUpdateFile(fhaPropFile, umbSqnMs, (OFFSET_AUTH_EF_SQN_INFO + (umbMaxIndex * LENGTH_AUTH_SQN)), LENGTH_AUTH_SQN, ACC_MODE_FILE_READ);

	DBG_FUNC("[31_102] _31102_verifyFreshnessOfSqn: SQNms:\n");
	DBG_PRINT_ARRAY(umbSqnMs, LENGTH_AUTH_SQN);

	/*Checking freshness as per C.2.2 of ETSI 33.102 */
	if (FALSE == _31102_compareSqn(pumbSQN, umbSqnMs_i)) {

		/*SQN <= SQNms(i)*/
		/*Resynch Failure*/
		DBG_ERR("[31_102] _31102_verifyFreshnessOfSqn: Resynch Failure as SQN <= SQNms(i)");
		DBG_FUNC_EXIT("<<[31_102] _31102_verifyFreshnessOfSqn() returns FALSE\n");
		/*Copy SqnMs in Sqn*/
		mem_cpy(pumbSQN, umbSqnMs, LENGTH_AUTH_SQN);
		return FALSE;
	}

	/*TODO: Delta check pending*/

	/*TODO: Limit check pending*/

	/*TODO: What if SQNms reach maximum value*/

	/*TODO: Enclose the writing in a transaction*/

	/*Check if incoming SQN is greater than SQNms*/
	if (
		(TRUE == _31102_compareSqn(pumbSQN, umbSqnMs)) //SQN > SQNms
		&&			//AND
		(umbMaxIndex != (pumbSQN[INDEX_LAST_BYTE_OF_SQN] & MASK_AUTH_SEQ_INDEX))	//incoming sqn index is different from previous max index
		){

		/*Max index will change to new one*/
		umbMaxIndex = pumbSQN[INDEX_LAST_BYTE_OF_SQN] & MASK_AUTH_SEQ_INDEX;

		DBG_FUNC("[31_102] _31102_verifyFreshnessOfSqn: Incoming SQN is greater than SQNms, hence change SQN max index to %1\n", umbMaxIndex);

		/*Update the SQNms index*/
		_7816_4_readUpdateFile(fhaPropFile, &umbMaxIndex, OFFSET_AUTH_EF_SQN_INFO, LENGTH_AUTH_EF_SQN_INFO, ACC_MODE_FILE_UPDATE);
		
	}
	else {
		/*get incoming index to update data there*/
		umbMaxIndex = pumbSQN[INDEX_LAST_BYTE_OF_SQN] & MASK_AUTH_SEQ_INDEX;
	}

	DBG_FUNC("[31_102] _31102_verifyFreshnessOfSqn: Update Incoming SQN at alloted index %1\n", umbMaxIndex);

	/*Read the SQNms(i) where i is index from incoming pumbSQN */
	_7816_4_readUpdateFile(fhaPropFile, pumbSQN, (OFFSET_AUTH_EF_SQN_INFO + ((umbMaxIndex) * LENGTH_AUTH_SQN)), LENGTH_AUTH_SQN, ACC_MODE_FILE_UPDATE);

	DBG_FUNC_EXIT("<<[31_102] _31102_verifyFreshnessOfSqn() returns TRUE\n");

	return TRUE;
}
/*
* It implements the USIM authenticate command
*/
void _31102_authenticate(void) {

	uMonoByte		umbIndex;
	uMonoByte		umbNAA = NAA_IS_USIM_GSM;
	tFileHeaderAddr fhaPropDf;
	tFileHeaderAddr fhaPropFile;
	uMonoByte		umbKi[LENGTH_AUTH_KEY + 0x08];
	uMonoByte		umbResponse[LENGTH_AUTH_RES];
	uMonoByte		umbCk[LENGTH_AUTH_CK];
	uMonoByte		umbIk[LENGTH_AUTH_IK];
	uMonoByte		umbAk[LENGTH_AUTH_AK];
	uMonoByte		umbSqn[LENGTH_AUTH_SQN];
	uMonoByte		umbOpc[LENGTH_AUTH_OPC];
	uMonoByte		umbXmac[LENGTH_AUTH_MAC];
	const uMonoByte	umbISIMAid[] = {0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x04};

	DBG_FUNC_ENTRY(">>[31_102] _31102_authenticate() \n");

	if (
		(0x00 != P1_BYTE) //check if p1 is non-zero
		||					//OR
		(
			(P2_APPLICATION_SPECIFIC_3G_CONTEXT != P2_BYTE) //If p2 neither state 3G context
			&&
			(P2_APPLICATION_SPECIFIC_GSM_CONTEXT != P2_BYTE) //Nor GSM context
		)
	   )
	{
		/*Wrong P1-P2*/
		DBG_ERR("[31_102] _31102_authenticate: Wrong P1 \n");
		DBG_FUNC_EXIT("<<[31_102] _31102_authenticate() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (P3_BYTE != P3_LENGTH_USIM_AUTHENTICATE) { // If P3 is not equal to expected length of Random Data
		/*Wrong P3*/
		DBG_ERR("[31_102] _31102_authenticate: Wrong P3: Length must be '%1' bytes \n", P3_LENGTH_USIM_AUTHENTICATE);
		DBG_FUNC_EXIT("<<[31_102] _31102_authenticate() \n");
		THROW2(EXCEP_WRONG_P3, P3_LENGTH_USIM_AUTHENTICATE);
	}

	/*Check if current ADF is not empty*/
	if ((NO_FILE_PRESENT == pccCurrentChannel->fhaCurrentAdf)
		||
		(pccCurrentChannel->fhaCurrentDf != pccCurrentChannel->fhaCurrentAdf)) {
		/*an ADF must be selected*/
		DBG_ERR("[31_102] _31102_authenticate: No NAA application is selected i.e. missing current ADF\n");
		DBG_FUNC_EXIT("<<[31_102] _31102_authenticate() \n");
		THROW(EXCEP_CONDITIONS_OF_USE_NOT_SATISFIED);
	}

	if (ACC_NOT_VERIFIED == _7816_4_isKeyRefVerified(PIN_APP_1)) { // If PIN1 is not verified

		/*PIN1 is mandatory to be verified for Network Authentication*/
		DBG_ERR("[31_102] _31102_authenticate: CHV1 is not verified\n");
		DBG_FUNC_EXIT("<<[31_102] _31102_authenticate() \n");
		THROW(EXCEP_SECURITY_NOT_SATISFIED);
	}



	/*Check if current APP is USIM or ISIM*/
	if (TRUE == _7816_4_getAidOfAdf(pccCurrentChannel->fhaCurrentAdf, umbKi)) {

		if (EQUAL == mem_cmp((umbKi + OFFSET_AID), umbISIMAid, LENGTH_MIN_ISIM_AID)) {

			umbNAA = NAA_IS_ISIM;
		}
	}


	DBG_FUNC("[31_102] _31102_authenticate: Searching prop. Auth Ki file  \n");
	/*Get file address of Prop. DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaPropDf);

	assert(fhaPropDf != NO_FILE_PRESENT);

	/*Get file address of Prop. Auth Ki file*/
	_7816_4_ifFileExists(FID_PROP_AUTH_KI, fhaPropDf, MODE_FIND_FID, &fhaPropFile);

	assert((fhaPropFile != NO_FILE_PRESENT) && (LENGTH_AUTH_KEY == _7816_4_getFileSize(fhaPropFile)));

	DBG_FUNC("[31_102] _31102_authenticate: Reading KI and OPC from %1 and %2 files respectively  \n", FID_PROP_AUTH_KI, FID_PROP_AUTH_OPC);

	/*Read the auth Ki from prop. file*/
	_7816_4_readUpdateFile(fhaPropFile, umbKi, OFFSET_START, LENGTH_AUTH_KEY, ACC_MODE_FILE_READ);

	/*Get file address of Prop. Auth OPC file*/
	_7816_4_ifFileExists(FID_PROP_AUTH_OPC, fhaPropDf, MODE_FIND_FID, &fhaPropFile);

	assert((fhaPropFile != NO_FILE_PRESENT) && (LENGTH_AUTH_OPC == _7816_4_getFileSize(fhaPropFile)));

	/*Read the OPC from prop. file*/
	_7816_4_readUpdateFile(fhaPropFile, umbOpc, OFFSET_START, LENGTH_AUTH_OPC, ACC_MODE_FILE_READ);

	/*If 3G context auth*/
	if (P2_APPLICATION_SPECIFIC_3G_CONTEXT == P2_BYTE) {
		
		DBG_FUNC("[31_102] _31102_authenticate: P2 states 3G context authentication \n");

		/*Perform 3G Context Authentication algo as per TS 31.102*/
		/*Get AK, CK, IK and RES*/
		f2345(umbKi, &COMMAND_DATA[OFFSET_AUTH_RAND], umbOpc, umbResponse, umbCk, umbIk, umbAk);

		DBG_FUNC("[31_102] _31102_authenticate: Generate AK using f5 function \n");

		/*Traverse through each element of AK for calculating actual SQN*/
		for (umbIndex = 0x00; umbIndex < LENGTH_AUTH_SQN; umbIndex++) {

			/*XORing of Incoming SQN(+)AK with calculated AK*/
			umbSqn[umbIndex] = COMMAND_DATA[OFFSET_AUTH_SQN_X_AK + umbIndex] ^ umbAk[umbIndex];
		}

		DBG_FUNC("[31_102] _31102_authenticate: Calculated SQN by XORing AN and SQNxAK data\n");

		/*Computing XMAC*/
		f1(umbKi, &COMMAND_DATA[OFFSET_AUTH_RAND], umbSqn, &COMMAND_DATA[OFFSET_AUTH_AMF], umbOpc, umbXmac);

		DBG_FUNC("[31_102] _31102_authenticate: Calculated XMAC using f1 function\n");

		if (EQUAL != mem_cmp(&COMMAND_DATA[OFFSET_AUTH_MAC], umbXmac, LENGTH_AUTH_MAC)) { //comparing XMAC and MAC and checking the return value if its not equal

			/*Incorrect MAC*/
			DBG_ERR("[31_102] _31102_authenticate: Presented MAC is not equal to calculated XMAC\n");
			DBG_FUNC_EXIT("<<[31_102] _31102_authenticate() \n");
			THROW(EXCEP_INCORRECT_MAC);
		}

		/*Verify SQN*/
		if(FALSE == _31102_verifyFreshnessOfSqn(umbSqn))/*If sqn not verified*/
		{
			/*if control is here umbSqn contains SQNms*/

			DBG_FUNC("[31_102] _31102_authenticate: SQN is not verified, so resynch error message will be sent to network\n");

			/*Generate Resynch MACS*/
			f1star(umbKi, &COMMAND_DATA[OFFSET_AUTH_RAND], umbSqn, &COMMAND_DATA[OFFSET_AUTH_AMF], umbOpc, umbXmac);

			/*Generate Resynch AK*/
			f5star(umbKi, &COMMAND_DATA[OFFSET_AUTH_RAND], umbOpc, umbAk);

			/*Traverse through each element of resynch AK for calculating conceal SQN*/
			for (umbIndex = 0x00; umbIndex < LENGTH_AUTH_SQN; umbIndex++) {

				/*XORing of Resynch AK and SQN*/
				umbSqn[umbIndex] ^= umbAk[umbIndex];
			}

			DBG_FUNC("[31_102] _31102_authenticate: Preparing AUTS\n");

			/*Prepare AUTS in response buffer*/
			/*Add Synch failure tag 'DC'*/
			RESPONSE_BUFF[OFFSET_AUTH_RESULT_TAG] = TAG_AUTH_SYNCH_FAILURE_TAG;

			/*Copy Length of AUTS*/
			RESPONSE_BUFF[OFFSET_AUTH_AUTS_LENGTH] = LENGTH_OF_AUTS;

			/*Copy Conc(SQNMS)*/
			mem_cpy((RESPONSE_BUFF + OFFSET_AUTH_AUTS_CONC_SQNMS), umbSqn, LENGTH_AUTH_SQN);

			/*Copy MACS where MACS = f1*K(SQNMS || RAND || AMF)*/
			mem_cpy((RESPONSE_BUFF + OFFSET_AUTH_AUTS_MACS), umbXmac, LENGTH_AUTH_MAC);

			/*Prepared response length*/
			gudbOutputLen = LENGTH_AUTH_AUTS;

			DBG_ERR("[31_102] _31102_authenticate: Resynch Failure\n");
			DBG_FUNC_EXIT("<<[102_221] _31102_authenticate() \n");
			return;
		}

		/*Prepare Successful Authentication response*/
		/*Store Auth Success tag*/
		RESPONSE_BUFF[OFFSET_AUTH_RESULT_TAG] = TAG_AUTH_SUCCESS_TAG;

		/*Store Length of RES*/
		RESPONSE_BUFF[OFFSET_AUTH_RES_LENGTH] = LENGTH_AUTH_RES;

		/*Copy RES*/
		mem_cpy((RESPONSE_BUFF + OFFSET_AUTH_RES), umbResponse, LENGTH_AUTH_RES);

		/*Store length of CK*/
		RESPONSE_BUFF[OFFSET_AUTH_CK_LENGTH] = LENGTH_AUTH_CK;

		/*Copy CK*/
		mem_cpy((RESPONSE_BUFF + OFFSET_AUTH_CK), umbCk, LENGTH_AUTH_CK);

		/*Store length of IK*/

		RESPONSE_BUFF[OFFSET_AUTH_IK_LENGTH] = LENGTH_AUTH_IK;

		/*Copy IK*/
		mem_cpy((RESPONSE_BUFF + OFFSET_AUTH_IK), umbIk, LENGTH_AUTH_IK);

		/*If ADF is USIM, Check for Service n27 in UST and add KC accordingly in the end*/
		if ((NAA_IS_USIM_GSM == umbNAA)
			&&
			(TRUE == _31102_checkServiceInUST(UST_SERIVCE_GSM_ACCESS))) {

			DBG_FUNC("[31_102] _31102_authenticate: GSM service is enabled so appending Kc in the response\n");

			/*GSM access service is available*/
			/*Store length of Kc*/
			RESPONSE_BUFF[OFFSET_AUTH_KC_LENGTH] = LENGTH_AUTH_KC;

			/*Traverse through each element of AK for calculating actual SQN*/
			for (umbIndex = 0x00; umbIndex < LENGTH_AUTH_KC; umbIndex++) {

				/*XORing of CK and IK*/
				/*As per TS 133 102 or 33.102: Kc[GSM] = CK1 xor CK2 xor IK1 xor IK2 where CKi and IKi are both 64 bits long and CK = CK1 || CK2 and IK = IK1 || IK2*/
				RESPONSE_BUFF[OFFSET_AUTH_KC + umbIndex] = umbCk[umbIndex] ^ umbCk[OFFSET_AUTH_SECOND_HALF + umbIndex] ^ umbIk[umbIndex] ^ umbIk[OFFSET_AUTH_SECOND_HALF + umbIndex];
			}

			gudbOutputLen = LENGTH_AUTH_RESPONSE_WITH_KC;
		} // if (_31102_checkServiceInUST(UST_SERIVCE_GSM_ACCESS))
		else {
			
			DBG_FUNC("[31_102] _31102_authenticate: GSM service is not enabled, response will not contain Kc\n");

			/*GSM access service is not available*/
			gudbOutputLen = LENGTH_AUTH_RESPONSE_WITHOUT_KC;
		}

	}
	else {

		DBG_FUNC("[31_102] _31102_authenticate: P2 states GSM context authentication \n");
		/*Perform 2G context Authentication algo as per TS 31.102*/
	}

	DBG_FUNC_EXIT("<<[102_221] _31102_authenticate() \n");

}