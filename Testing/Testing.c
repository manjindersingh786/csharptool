#include<stdio.h>
#include<stdlib.h>
// Char Type
typedef unsigned char uMonoByte;
typedef unsigned char boolean;
typedef signed char sMonoByte;
typedef unsigned char* puMonoByte;
typedef signed char* psMonoByte;

// Short Type
typedef unsigned short uDiByte;
typedef signed short sDiByte;
typedef unsigned short* puDiByte;
typedef signed short* psDiByte;

// Long Type
typedef unsigned long uQuadByte;
typedef signed long sQuadByte;
typedef unsigned long* puQuadByte;
typedef signed long* psQuadByte;

// Long Long Type
typedef unsigned long long uOctaByte;
typedef signed long long sOctaByte;
typedef unsigned long long* puOctaByte;
typedef signed long long* psOctaByte;
/*
* Handle Types
*/
typedef puDiByte pmFileHandle;
typedef uDiByte mFileHandle;
typedef uQuadByte mHandle;
typedef uQuadByte T_HEAP_OFFSET_HANDLE;
typedef void (*T_FPTR)(void);
typedef uMonoByte(*T_FPTR_NUM_METHODS_IN_PKG)(uMonoByte);
typedef uDiByte(*T_DISPATCHER_ARRAY)(uMonoByte);

typedef uDiByte	tMemAddr; // address type of memory area
typedef tMemAddr	tFileHeaderAddr;
//#pragma push
//#pragma pack(1)
typedef struct {
	/*Record of EFarr poninting access conditions*/
	uMonoByte umbEFarrRec; //0

	/*File descriptor byte*/
	uMonoByte umbFileDesc; //1

	/*Life Cycle status integer*/
	uMonoByte umbLCSI; //2

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
			struct {
				/*Pin Application 1: 01-08*/
				uMonoByte	umbAppPin1; //13

				/*Universal Pin + ADM pin : 11 & 0A-0E 8A-8E*/
				uMonoByte	umbGlobalPin; //14
				uMonoByte	umbAdmPin;//15

				/*Pin Application 2: 81:88*/
				uMonoByte	umbAppPin2;//16
			}pstdo;

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
#pragma pack()
typedef struct {
	/*Record of EFarr poninting access conditions*/
	uMonoByte umbEFarrRec; //0
	/*Pointer to parent file header*/
	tFileHeaderAddr	fhaParent; //3
}tt;
typedef unsigned char u8;
extern void f1(u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2],
	u8 mac_a[8]);
extern void f2345(u8 k[16], u8 rand[16],
	u8 res[8], u8 ck[16], u8 ik[16], u8 ak[6]);
extern void f1star(u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2],
	u8 mac_s[8]);
extern void f5star(u8 k[16], u8 rand[16],
	u8 ak[6]);
extern void ComputeOPc(u8 op_c[16]);
extern void RijndaelKeySchedule(u8 key[16]);
extern void RijndaelEncrypt(u8 input[16], u8 output[16]);
extern void setOp(u8 input[16]);
extern void findXor(u8 l1[], u8 l2[], u8 l3[], short len);
void printArray(const u8* array, short len) {

	short index = 0;
	printf("\n");
	for (index = 0; index < len; index++) {
		printf("%x ",array[index]);
	}
	printf("\n");
}

extern uMonoByte algos_comp128v3(const puMonoByte pumbKi, const puMonoByte pumbRand, puMonoByte pumbSres, puMonoByte pumbKc);
int main()
{
	//unsigned char f[50];// = { 0xFF };
	//u8 key[] = { 0x46,0x5b,0x5c,0xe8,0xb1,0x99,0xb4,0x9f,0xaa,0x5f,0x0a,0x2e,0xe2,0x38,0xa6,0xbc };
	//u8 key[] = {0x18, 0x7B, 0x6E, 0xCD, 0x5D, 0x58, 0x36, 0x96, 0x48, 0x3A, 0x69, 0x0A, 0x7F, 0xB3, 0x85, 0x2B}; // decrypted with OP
	//u8 key[] = { 0x75, 0x00, 0x67, 0x6B, 0x55, 0x6E, 0x8B, 0xE6, 0x6A, 0x3D, 0xF0, 0x7C, 0x42, 0x92, 0xF9, 0xCC }; //RKEY
	//u8 key[] = {0xB2, 0x43, 0x68, 0xC2, 0x1B, 0xA3, 0x2C, 0xD4, 0x6E, 0x86, 0x9C, 0x3D, 0xAB, 0xB3, 0xAD, 0xA1}; //TKEY
	//u8 key[] ={0x4B, 0x79, 0xA1, 0xF0, 0xF4, 0xC3, 0xFE, 0x49, 0x03, 0xAD, 0x63, 0xBD, 0x9A, 0x76, 0x90, 0x3C}; //EKEY
	//u8 key[] = { 0x77, 0xAC, 0x67, 0xC4, 0x45, 0xC1, 0xC1, 0xFA, 0x87, 0x28, 0x5A, 0x01, 0x61, 0x5F, 0xC6, 0xCA }; // new Ki
	//u8 key[] = { 0x8D, 0x87,0xF9, 0x42,0x51, 0xF5,0xC2, 0xC7,0xB0, 0x41,0x08, 0x12,0xA7, 0xC2,0xEE, 0xAD };// new ki decrypted 3des
	u8 key[] = {0xF1, 0xC3, 0xE9, 0x9B, 0x9E, 0x7A, 0x9F, 0x3A, 0xCB, 0x87, 0x38, 0x74, 0x4F, 0x09, 0x83, 0xD3};
	//u8 key[] = { 0x96,0x9A,0x97,0x88,0x3A,0xBA,0x1F,0x44,0x6C,0x59,0x7C,0x0F,0xED,0x8B,0x22,0xA3 };
	//u8 RAND[] = {0x3B, 0x4D, 0x71, 0x5D, 0xA6, 0xD4, 0x86, 0x8F, 0xB7, 0x41, 0x6F, 0x72, 0x9A, 0x00, 0x6A, 0xB6};
	//u8 RAND[] = {0xC8, 0xCA, 0x3E, 0x57, 0xFF, 0xEE, 0xAE, 0x13, 0xD3, 0xD3, 0x65, 0x57, 0x51, 0xE4, 0x94, 0xD9}; //new rand
	u8 RAND[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}; 
	//u8 data[] = { 0xee,0x36,0xf7,0xcf,0x03,0x7d,0x37,0xd3,0x69,0x2f,0x7f,0x03,0x99,0xe7,0x94,0x9a };
	//u8 RAND[] = { 0x23,0x55,0x3c,0xbe,0x96,0x37,0xa8,0x9d,0x21,0x8a,0xe6,0x4d,0xae,0x47,0xbf,0x35 };
	u8 SQN[6];// = { 0xff,0x9b,0xb4,0xd0,0xb6,0x07 };
	u8 AMF[] = { 0x80,0x00 };
	//u8 op[] = { 0xcd,0xc2,0x02,0xd5,0x12,0x3e,0x20,0xf6,0x2b,0x6d,0x67,0x6a,0xc7,0x2c,0xb3,0x18 };
	//u8 op[] = {0xA3, 0x24, 0xF2, 0xF7, 0xCA, 0xA2, 0xD2, 0x48, 0xCA, 0x3A, 0x28, 0x92, 0xDC, 0xFF, 0x23, 0x4C};// real op
	u8 op[] = {0x2B, 0x51, 0xC3, 0xB8, 0x55, 0xAD, 0x0B, 0xBE, 0x8F, 0xEE, 0x88, 0xA8, 0x37, 0x06, 0x19, 0x99};
	//u8 op[] = {0xB2, 0x43, 0x68, 0xC2, 0x1B, 0xA3, 0x2C, 0xD4, 0x6E, 0x86, 0x9C, 0x3D, 0xAB, 0xB3, 0xAD, 0xA1};// TKEY
	u8 output[16]={0};
	u8 res[16] = { 0 };
	u8 ck[16] = { 0 };
	u8 ik[16] = { 0 };
	u8 ak[6] = { 0 };
	//u8 sqnXak[] = { 0x78, 0xDE, 0x35,0x55,0x46,0xF7 };
	u8 sqnXak[] = {0xED, 0xA8, 0xC8, 0xD5, 0x10, 0x99}; //new
	//RijndaelKeySchedule(key);
	//RijndaelEncrypt(data, output);

	//RijndaelKeySchedule(key);
	//RijndaelEncrypt(op, output);
	setOp(op);
	
	/*Milenage*/
#if 0
		f2345(key, RAND, res, ck, ik, ak);
		findXor(ak, sqnXak, SQN, 6);
		printf("SQN\n");
		printArray(SQN, 6);
		f1(key, RAND, SQN,AMF, output);
#endif
	/*GSM Milenage*/
#if 0
		f2345(key, RAND, res, ck, ik, ak);

		printf("Res\n");
		printArray(res, 16);
		findXor(res, res+4, res + 4, 4);
		findXor(res+4, res + 8, res + 8, 4);
		findXor(res + 8, res + 12, res, 4);
		printf("SRes\n");
		printArray(res, 4);

		printf("ck\n");
		printArray(ck, 16);

		printf("ik\n");
		printArray(ik, 16);
		findXor(ck, ck + 8, ck, 8);
		findXor(ck, ik, ik, 8);
		findXor(ik, ik + 8, ik, 8);
		printf("Kc\n");
		printArray(ik, 8);
#endif
	/*GSM comp128v3*/
		algos_comp128v3(key, RAND, res, ck);		
		printf("SRES\n");
		printArray(res, 4);

		printf("kc\n");
		printArray(ck, 8);

	//printf("Res\n")
	//printArray(res, 8);
	//printf("ck\n");
	//printArray(ck, 16);
	//printf("ik\n");
	//printArray(ik, 16);
	//printf("ak\n");
	//printArray(ak, 6);
	printf("MAC\n");
	printArray(output, 16);
	
}