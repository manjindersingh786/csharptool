#ifndef _31_115_EXTERN_H
#define _31_115_EXTERN_H
#include "types.h"
#include "102_225_extern.h"
#include "31_111_extern.h"
/*
* Macro Defines
*/
#define	FILE_OFFSET_SMS_TPDU_FIRST_VALUE_BYTE					(OFFSET_SECURED_MESSAGE)
#define	FILE_OFFSET_TP_OA_LEN									(OFFSET_SECURED_MESSAGE + 0x01)


#define TP_PID_USIM_DATA_DOWNLOAD		(0x7F)
#define TP_DCS_CLASS2_USIM				(0xF6)
#define TP_MTI_SMS_DELIVER				(0x00)
#define TP_MTI_SMS_SUBMIT				(0x01)
#define TP_UDHI_FOR_HEADER				(0x40)
#define LENGTH_OF_TP_TIME_STAMP			(0x07)
#define MASK_TP_MTI						(0x03)
#define MASK_TP_UDHI					(0x40)
#define TP_RD_REJECT_DUPLICATE			(0x00)
#define TP_VPF_NOT_PRESENT				(0x00)
#define TP_SRR_NOT_REQUESTED			(0x00)
#define TP_RP_NOT_SET					(0x00)
#define TP_MR_ZERO						(0x00)



#define TAG_PROACTIVE_COMMAND                               (0xD0)
#define TAG_COMMAND_DEATAILS                                (0x81)
#define LEN_COMMAND_DEATAILS                                (0x03)
#define TAG_DEVICE_IDENTITES                                (0x82)
#define LEN_DEVICE_IDENTITES                                (0x02)
#define TAG_RESULT                                          (0x83)
#define LEN_P3_TERMINAL_RESPONSE                            (0x0C)
#define SOURCE_TERMINAL                                     (0x82)
#define SOURCE_UICC                                         (0x81)
#define SOURCE_NETWORK                                      (0x83)



#define OFFSET_PROACT_TAG							(0x00)
#define OFFSET_PROACT_LEN							(OFFSET_PROACT_TAG + 0x01)
#define OFFSET_CMD_DETAILS_TAG						(OFFSET_PROACT_LEN + 0x01)
#define OFFSET_CMD_DETAILS_LEN						(OFFSET_CMD_DETAILS_TAG + 0x01)
#define OFFSET_CMD_NO								(OFFSET_CMD_DETAILS_LEN + 0x01)
#define OFFSET_TOC									(OFFSET_CMD_NO + 0x01)
#define OFFSET_CMD_QUAL								(OFFSET_TOC + 0x01)
#define OFFSET_DEVICE_ID_TAG						(OFFSET_CMD_QUAL + 0x01)
#define OFFSET_DEVICE_ID_LEN						(OFFSET_DEVICE_ID_TAG + 0x01)
#define OFFSET_DEVICE_ID_SRC						(OFFSET_DEVICE_ID_LEN + 0x01)
#define OFFSET_DEVICE_ID_DST						(OFFSET_DEVICE_ID_SRC + 0x01)
#define OFFSET_SMS_TPDU_TAG							(OFFSET_DEVICE_ID_DST + 0x01)
#define OFFSET_SMS_TPDU_LEN							(OFFSET_SMS_TPDU_TAG + 0x01)
#define OFFSET_SMS_TPDU_FIRST_BYTE					(OFFSET_SMS_TPDU_LEN + 0x01)
#define OFFSTE_TP_MR								(OFFSET_SMS_TPDU_FIRST_BYTE + 0x01)
#define OFFSET_TP_DA								(OFFSTE_TP_MR + 0x01)
#define COMMAND_NUMBER								(0x01)
#define QUALIFIER_SEND_SM_PACKAGE_NOT_REQUIRED		(0x00)
#define CMD_TYPE_SEND_SMS							(0x13)
#define	TP_OA_MAX_LEN								(0x05)	


#define CRC_16_For_CheckSum											(0x02)
#define CRC_32_For_CheckSum											(0x04)
#define AES_DES_For_CheckSum										(0x08)

#define POR_CRYPTOGRAPHIC_CHECKSUM									(0x08)
#define	POR_REDUNDANCY_CHECK										(0x04)
#define	POR_NO_RC_CC_DS												(0x00)

#define	POR_CIPHERING												(0x10)
#define	MASK_SPI2_CIPHERING											(0x10)
#define	MASK_SMS_SUBMIT												(0x20)
/*
* Functions declaration
*/
/*
* It parses the data acc. to 31.115
*
* @param pumbSmsData: SMS data received from Network
*/
extern void _31115_SmsppImplementation(IN_ARG puMonoByte pumbSmsData, IN_ARG uMonoByte umbLenSmsData);

extern void _31115_preparePOR(IN_ARG uMonoByte umbRSC);

#endif // !_31_115_EXTERN_H
