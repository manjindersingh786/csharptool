#include "31_111.h"

/*
* It handles Envelope Command
* 
* @param pumbEnvelopeData: Envelope Data
*/
void _31111_Envelope(IN_ARG puMonoByte pumbEnvelopeData) {

	uMonoByte	umbLengthSmsPp;
	uMonoByte	umbLengthSmsTpdu;
	uMonoByte   umbIndex;
	uMonoByte   umbRSC = 0x00;

	DBG_FUNC_ENTRY(">>[31_111] _31111_Envelope() \n");

	/*check type of Envelope Data*/
	if (TAG_ENV_SMS_PP_DATA_DOWNLOAD == pumbEnvelopeData[0x00]) {

		DBG_FUNC("[31_111] _31111_Envelope: Found 'D1' Tag which indicates SMS pp data download\n");

		if (LENGTH_OF_TWO_BYTES == pumbEnvelopeData[0x01]) {
			/*Check if length tag is present*/

			umbLengthSmsPp = pumbEnvelopeData[0x02]; // next byte will be length
			DBG_FUNC("[31_111] _31111_Envelope: Found '81' tag for larger length of 'D1' tag which indicates %1 bytes length of SMS pp data download\n", umbLengthSmsPp);
			umbIndex = 0x03;
		}
		else {

			umbLengthSmsPp = pumbEnvelopeData[0x01]; // same byte will be length
			DBG_FUNC("[31_111] _31111_Envelope: Length for 'D1' tag which indicates %1 bytes length of SMS pp data download\n", umbLengthSmsPp);
			umbIndex = 0x02;
		}

		if ((P3_BYTE - umbIndex) != umbLengthSmsPp) {
			/*Wrong length of 'D1' tag*/
			DBG_ERR("[31_111] _31111_Envelope: Wrong Length for 'D1' tag of SMS pp data download. Correct length will be %1\n", P3_BYTE - umbIndex);
			DBG_FUNC_EXIT("<<[31_111] _31111_Envelope() \n");
			THROW(EXCEP_TECHNICAL_PROBLEM);
		}

		if (TAG_OF_DEVICE_IDENTITIES == pumbEnvelopeData[umbIndex]) {
			/*Check the tag '82' of device identities*/

			DBG_FUNC("[31_111] _31111_Envelope: Found '82' Tag which indicates Device Identities\n");
			DBG_FUNC("[31_111] _31111_Envelope: Length byte which indicates %1 length for Device Identities tag\n", LENGTH_OF_DEVICE_IDENTITIES);

			/*adding the length of Device Identities TLV to current index*/
			umbIndex = umbIndex + (0x02 + LENGTH_OF_DEVICE_IDENTITIES); //2 for device identities tag and device identities length 
		}

		if (TAG_OF_ADDRESS == pumbEnvelopeData[umbIndex]) {
			/*Check the tag '86' of address*/

			DBG_FUNC("[31_111] _31111_Envelope: Found '86' Tag which indicates Address\n");

			umbIndex++; //index at length byte
			DBG_FUNC("[31_111] _31111_Envelope: Length byte which indicates %1 length for Address tag\n", pumbEnvelopeData[umbIndex]);

			/*adding the length of Address TLV to current index*/
			umbIndex = umbIndex + (0x01 + pumbEnvelopeData[umbIndex]); //1 for address tag and address length
			//pumbEnvelopeData[umbIndex] is length of address
		}

		if (TAG_OF_SMS_TPDU == pumbEnvelopeData[umbIndex]) {
			/*Check the tag '8B' of SMS TPDU*/
			DBG_FUNC("[31_111] _31111_Envelope: Found '8B' Tag which indicates SMS TPDU\n");
			umbIndex++;

			if (LENGTH_OF_TWO_BYTES == pumbEnvelopeData[umbIndex]) {
				/*Check if length tag is present*/

				umbLengthSmsTpdu = pumbEnvelopeData[++umbIndex]; // next byte will be length
				DBG_FUNC("[31_111] _31111_Envelope: Found '81' tag for larger length of '8B' tag which indicates %1 bytes length of SMS TPDU\n", umbLengthSmsTpdu);
			}
			else {

				umbLengthSmsTpdu = pumbEnvelopeData[umbIndex]; // same byte will be length
				DBG_FUNC("[31_111] _31111_Envelope: Length for '8B' tag which indicates %1 bytes length of SMS TPDU\n", umbLengthSmsTpdu);
			}
			umbIndex++;

			if ((P3_BYTE - umbIndex) != umbLengthSmsTpdu) {
				/*Wrong length of '8B' tag*/
				DBG_ERR("[31_111] _31111_Envelope: Wrong Length for '8B' tag of SMS TPDU. Correct length will be %1\n", P3_BYTE - umbIndex);
				DBG_FUNC_EXIT("<<[31_111] _31111_Envelope() \n");
				THROW(EXCEP_TECHNICAL_PROBLEM);
			}
			/*function calling for the parsing of secured command packet*/
			_31115_SmsppImplementation((pumbEnvelopeData + umbIndex), umbLengthSmsTpdu);
			
		}
	}
	DBG_FUNC_EXIT("<<[31_111] _31111_Envelope() \n");
}

