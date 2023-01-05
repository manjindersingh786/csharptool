#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define INDEX_CLA											(0x00)
#define INDEX_INS											(0x01)
#define INDEX_P1											(0x02)
#define INDEX_P2											(0x03)
#define INDEX_P3											(0x04)
#define INDEX_CMD_DATA										(0x05)
#define RFU_BYTE											(0x00)
#define RFU_BIT												(0x00)

#define P1_SELECT_BY_AID									(0x04)
#define P1_SELECT_BY_FILE_ID								(0x00)

/*Search modes*/
#define SEARCH_FORWARD_MODE									(0x04)
#define SEARCH_BACKWARD_MODE								(0x05)
#define SEARCH_ENHANCED_MODE								(0x06)
#define SEARCH_FORWARD_FROM_NEXT_MODE						(0x06)
#define SEARCH_BACKWARD_FROM_PREVIOUS_MODE					(0x07)

#define MASK_HIGHER_NIBBLE									(0xF0)
#define MASK_LOWER_NIBBLE									(0x0F)

#define CLA_SIM												(0xA0)
#define CLA_UICC_1											(0x00)
#define CLA_UICC_2											(0x80)
#define MASK_CLA											(0x7C)

//For Select in UICC Different Matherds
#define P1_SELECT_BY_FILE_ID								(0x00)
#define P1_SELECT_BY_CHILD									(0x01)
#define P1_SELECT_BY_PARENT									(0x03)
#define P1_SELECT_BY_AID									(0x04)
#define P1_SELECT_BY_PATH_MF								(0x08)
#define P1_SELECT_BY_PATH_CURRENT_DF						(0x09)

#define BASIC_LOGICAL_CHANNEL								(0x00)
#define	RFM_LOGICAL_CHANNEL									(0x04)
#define LENGTH_FILE_ID										(0x02)
#define LENGTH_LCSI											(0x01)
#define LENGTH_SEC_ATTR_REFERENCED							(0x03)
#define LENGTH_TOTAL_FILE_SIZE								(0x02)
#define LENGTH_USAGE_QUALIFIER								(0x01)
#define LENGTH_KEY_REFERENCE								(0x01)
#define LENGTH_SFI											(0x01)
#define LENGTH_PROP_TLV										(0x03)
#define LENGTH_SPECIAL_FILE_INFO							(0x01)
#define LENGTH_OTA_FILE_INFO							    (0x01)
#define LENGTH_MAX_AID										(0x10)
#define LENGTH_MIN_AID										(0x05)
#define LENGTH_PIN_VALUE									(0x08)
#define EQUAL												(0x00)
#define MASK_LOGICAL_CHANNEL								(0x03)
#define LENGTH_ACCESS_MODE_DATA_OBJECT						(0x01)
#define LENGTH_SCDO											(0x01)
#define MASK_P2_BYTE                                        (0xB7)
#define P2_BYTE_VALIDATION_PARAMETER                        (0x04)
#define RETURN_FCP_TEMPLATE                                 (0x04)
#define NO_DATA_RETURN                                      (0x0C)
#define MASK_FCP_RETURN_BYTES								(0x0C)
#define LENGTH_AVAVIL_MEM								    (0x04)
#define LENGTH_UICC_CHAR                                    (0x01)
#define LENGTH_TAG_PROP_TLV                                 (0x0C)
#define LENGTH_SUPPORTED_COMMMANDS                          (0x01)
//Terminal Capability Not Supported
#define DATA_SUPPORTED_COMMMANDS                            (0x00)

// UICC Characterstics byte(Clock stop allowed,voltage class A,B and C supported)
#define DATA_UICC_CHAR                                      (0x71)




/*Key References*/
#define PIN_APP_1											(0x01)
#define PIN_APP_2											(0x02)
#define PIN_APP_3											(0x03)
#define PIN_APP_4											(0x04)
#define PIN_APP_5											(0x05)
#define PIN_APP_6											(0x06)
#define PIN_APP_7											(0x07)
#define PIN_APP_8											(0x08)

#define PIN_ADM_1											(0x0A)
#define PIN_ADM_2											(0x0B)
#define PIN_ADM_3											(0x0C)
#define PIN_ADM_4											(0x0D)
#define PIN_ADM_5											(0x0E)
#define PIN_ADM_6											(0x8A)
#define PIN_ADM_7											(0x8B)
#define PIN_ADM_8											(0x8C)
#define PIN_ADM_9											(0x8D)
#define PIN_ADM_10											(0x8E)


#define PIN_UNIVERSAL										(0x11)


#define PIN_SECOND_APP_1									(0x81)
#define PIN_SECOND_APP_2									(0x82)
#define PIN_SECOND_APP_3									(0x83)
#define PIN_SECOND_APP_4									(0x84)
#define PIN_SECOND_APP_5									(0x85)
#define PIN_SECOND_APP_6									(0x86)
#define PIN_SECOND_APP_7									(0x87)
#define PIN_SECOND_APP_8									(0x88)

#define PIN_ALWAYS											(0x90)
#define PIN_NEVER											(0xFF)

/*Standard File IDs*/
#define FID_MF												(0x3F00)
#define FID_CURRENT_ADF										(0x7FFF)
#define FID_GSM												(0x7F20)
#define FID_SST												(0x6F38)
#define FID_UST												(FID_SST)
#define FID_IMSI											(0x6F07)
#define FID_LOCI											(0x6F7E)
#define FID_TELECOM											(0x7F10)
#define FID_ADN												(0x6F3A)
#define FID_FDN												(0x6F3B)

/*Proprietary File IDs*/
#define FID_PROP_ADMIN_USE									(0x7F49) // Admin Use DF
#define FID_PROP_ADF_LIST									(0x6F10) // ADF List EF
#define FID_PROP_KEYS_CONFIG								(0x6F11) // Configuration of Key references
#define FID_PROP_KEYS_VALUE									(0x6F12) // PIN values of Key references
#define FID_PROP_AUTH_KI									(0x6F13) // Subscriber Authentication Key
#define FID_PROP_AUTH_OPC									(0x6F14) // Encrypted Operator Key
#define FID_PROP_AUTH_SEQ_ARRAY								(0x6F15) // Array of Sequence Numbers#define FID_PROP_AUTH_SEQ_ARRAY
#define FID_PROP_RFM_PARAMETER								(0x6F16) // RFM Parameters, IT's Containes Access_Domain, MSL and TAR
#define FID_PROP_TERMINAL_PROFILE							(0x6F17) // Store the CAT facilities that are supported by the terminal
#define FID_PROP_KEY_MANAGEMENT                             (0x6F18) // Store the Keys configuration
#define FID_PROP_KEY_VALUE							        (0x6F19) // Store the key value
#define FID_PROP_SECURED_MESSAGE							(0x6F1A) // Stores the secured message data


#define NO_OF_PROP_EF										(0x0B) // total number of proprietary EFs under FID_PROP_ADMIN_USE


/*ADF List file constants*/
#define OFFSET_LENGTH_AID									(0x00) // length of AID
#define OFFSET_AID											(OFFSET_LENGTH_AID + 0x01) // AID
#define OFFSET_ADDR_FILE_HEADER								(OFFSET_AID + LENGTH_MAX_AID) //ADF file header address
#define LENGTH_RECORD_ADF_LIST								(OFFSET_ADDR_FILE_HEADER + 0x02) 

/*File control parameters TLVs FCP*/
#define TAG_FCP_TEMPLATE									(0x62)
#define TAG_FILE_DESC										(0x82)
#define TAG_FILE_ID											(0x83)
#define TAG_DF_NAME											(0x84)
#define TAG_LCSI											(0x8A)
#define TAG_SEC_ATTR_REFERENCED								(0x8B)
#define TAG_TOTAL_FILE_SIZE									(0x81)
#define TAG_PIN_STATUS_TEMP_DO								(0xC6)
#define TAG_USAGE_QUALIFIER									(0x95)
#define TAG_KEY_REFERENCE									(0x83)
#define TAG_FILE_SIZE										(0x80)
#define TAG_SFI												(0x88)
#define TAG_PROP_TLV										(0xA5)
#define TAG_APP_DEPEND_TLV									(0x85)
#define TAG_SPECIAL_FILE_INFO								(0xC0)
#define TAG_OTA_FILE_INFO								    (0xCC)
#define DATA_CODE_BYTE                                      (0x21)
#define TAG_UICC_CHAR                                       (0x80)
#define TAG_AVAIL_MEM                                       (0x83)
#define TAG_SUPPORTED_COMMMANDS                             (0x87)


/*Security attributes TLVs*/
#define TAG_SECURITY_AMDO_ACCESS_MODE_BYTE					(0x80)
#define TAG_SECURITY_AMDO_INSTRUCTION						(0x84)
#define TAG_SECURITY_SCDO_ALWAYS							(0x90)
#define TAG_SECURITY_SCDO_NEVER								(0x97)
#define TAG_SECURITY_SCDO_CRT								(0xA4)
#define TAG_SECURITY_SCDO_OR								(0xA0)
#define TAG_SECURITY_SCDO_NOT								(0xA7)
#define TAG_SECURITY_SCDO_AND								(0xAF)


/*Terminal Response TLVs*/
#define TAG_COMMAND_DEATAILS							    (0x81)
#define TAG_DEVICE_IDENTITES								(0x82)
#define TAG_RESULT								            (0x83)
#define LEN_P3_TERMINAL_RESPONSE					        (0x0C)

#endif