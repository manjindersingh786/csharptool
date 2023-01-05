#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define POWER_OFF                       (0x00)
#define POWER_ON                        (0x01)
#define RESET                           (0x02)
#define NVM_SIZE                        (0x55000)
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "35964"
int p;
int* c = &p;
extern "C" {
    extern void hwAbs_osMain(void);
    extern void Simul_sendByte(unsigned char);
    extern unsigned char Simul_sendLastByte(unsigned char);
    extern unsigned char Simul_receiveByte(void);
    extern void debugPrint(unsigned char color, const char* message, ...);
    extern unsigned char umbEEPRom[];
    extern void debugPrintArray(unsigned char color, const char* array, unsigned short len);
    extern void addAIndent(void);
    extern void removeAIndent(void);
    extern void backupIndent(void);
    extern void restoreIndent(void);
}

static void debugPrintWithoutIndent(unsigned char color, const char* message, ...);

int trycatchindex = 0; //stores active try catch index
int indentVal=0;

int backIndentVal[0x02]; // size of this array depends upon MAX_SIZE_JUMP_BUFF
extern void resetGlobalVariables(void);

static unsigned char BUFFER[DEFAULT_BUFLEN];
static unsigned short respIndex = 0;
static unsigned short cmdIndex = 0;
static unsigned short outLen;
unsigned char isLastByteSent = 0x00;
jmp_buf simul_ExceptionHandler;

//BOOL WINAPI DllMain(
//    HINSTANCE hinstDLL,  // handle to DLL module
//    DWORD fdwReason,     // reason for calling function
//    LPVOID lpReserved)  // reserved
//{
//    return TRUE;
//}

FILE* DbgFile;
static void convertToUpperCase(char arr[], size_t len) {
    char digit;
    while (len) {
        digit = arr[--len];
        if (digit >= 'a' && digit <= 'f') {
            digit = 'A' + digit - 'a';
            arr[len] = digit;
        }
    }
}
HANDLE hConsole;
SOCKET ConnectSocket = INVALID_SOCKET;
int __declspec(dllexport) main(void)
{
    WSADATA wsaData;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "this is a test";
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    bool sendResponse = false;
    
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    //TODO: eeprom to save in external bin file
    FILE* file = fopen("EEPROM.bin", "rb+");
    fread(umbEEPRom, sizeof(*umbEEPRom), NVM_SIZE, file);
    DbgFile = fopen("SIMOS.log", "a+");
    //fclose(file);

    //memset(umbEEPRom, 0xFF, NVM_SIZE);

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        debugPrint(0x00, "WSAStartup failed with error: %4\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        debugPrint(0x00, "getaddrinfo failed with error: %4\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            debugPrint(0x00, "getaddrinfo failed with error: %4\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        debugPrint(0x00, "Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        debugPrint(0x00, "1 send failed with error: %4\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 3;
    }

    debugPrint(0x00, "Connection Established\n");
    int i = setjmp(simul_ExceptionHandler);
    if (0 == i)
    {

        //send ATR
        //call main function of OS
        //Reset Global Variables
        resetGlobalVariables();
        hwAbs_osMain();
    }
    else if(i == 1) {
        //file = fopen("EEPROM.bin", "rb+");
        //fsetpos(file, 0);
        rewind(file); //reset cursor to the starting
       fwrite(umbEEPRom, sizeof(*umbEEPRom), NVM_SIZE, file);
       fclose(file);
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    debugPrint(0x00, "Card closed\n");

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

void Simul_sendByte(unsigned char value)
{
    BUFFER[respIndex++] = value;
    isLastByteSent = 0x00;

}

unsigned char Simul_sendLastByte(unsigned char value)
{
    int iResult;
    BUFFER[respIndex++] = value;

    //Send buffered data
    iResult = send(ConnectSocket, (const char*)BUFFER, respIndex, 0);

    if (iResult != respIndex) {
        //if data has not been sent
        debugPrint(0x00, "3 send failed with error: %4\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        longjmp(simul_ExceptionHandler, 2);
    }
    debugPrintWithoutIndent(0x00, "Response Sent: ");
    for (short i = 0;i < respIndex; i++)
    {
        debugPrintWithoutIndent(0x00, "%1 ", BUFFER[i] & 0xFF);
    }
    debugPrintWithoutIndent(0x00, "\n");
    //Reset response index
    respIndex = 0;
    iResult = recv(ConnectSocket, (char*)BUFFER, DEFAULT_BUFLEN, 0);
    if (iResult != 2)
    {
        debugPrint(0x00, "2 recv failed with error: %4\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        longjmp(simul_ExceptionHandler, 2);
    }
    outLen = recv(ConnectSocket, (char*)BUFFER, DEFAULT_BUFLEN, 0);
    if (outLen == 1)
    {
        if (POWER_OFF == BUFFER[0] || POWER_ON == BUFFER[0] || RESET == BUFFER[0])
        {
            //throw execption and recall osMain()
            longjmp(simul_ExceptionHandler, 1);
        }
    }
    else if(outLen < 1)
    {
        debugPrint(0x00, "2 recv failed with error: %4\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        longjmp(simul_ExceptionHandler, 2);
    }
    isLastByteSent = 0x01;
    //command recieved continue with processing in main
    debugPrintWithoutIndent(0x00, "Command Received: ");
    for (short i = 0;i < outLen; i++)
    {
        debugPrintWithoutIndent(0x00, "%1 ", BUFFER[i] & 0xFF);
    }
    cmdIndex = 0;
    debugPrintWithoutIndent(0x00, "\n");

    return BUFFER[cmdIndex++];
}

unsigned char Simul_receiveByte(void) {
    
    int iResult;
    if (0x00 == isLastByteSent) {
        //Send buffered data
        iResult = send(ConnectSocket, (const char*)BUFFER, respIndex, 0);

        if (iResult != respIndex) {
            //if data has not been sent
            debugPrint(0x00, "3 send failed with error: %4\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            longjmp(simul_ExceptionHandler, 2);
        }
        debugPrintWithoutIndent(0x00, "Response Sent: ");
        for (short i = 0;i < respIndex; i++)
        {
            debugPrintWithoutIndent(0x00, "%1 ", BUFFER[i] & 0xFF);
        }
        debugPrintWithoutIndent(0x00, "\n");

        //Reset response index
        respIndex = 0;
        iResult = recv(ConnectSocket, (char*)BUFFER, DEFAULT_BUFLEN, 0);
        if (iResult != 2)
        {
            debugPrint(0x00, "1 recv failed with error: %4\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            longjmp(simul_ExceptionHandler, 2);
        }
        outLen = recv(ConnectSocket, (char*)BUFFER, DEFAULT_BUFLEN, 0);
        if (outLen == 1)
        {
            if (POWER_OFF == BUFFER[0] || POWER_ON == BUFFER[0] || RESET == BUFFER[0])
            {
                //throw execption and recall osMain()
                longjmp(simul_ExceptionHandler, 1);
            }
        }
        else if (outLen < 1)
        {
            debugPrint(0x00, "2 recv failed with error: %4\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            longjmp(simul_ExceptionHandler, 2);
        }
        isLastByteSent = 0x01;
        //command recieved continue with processing in main
        debugPrintWithoutIndent(0x00, "Command Received: ");
        for (short i = 0;i < outLen; i++)
        {
            debugPrintWithoutIndent(0x00, "%1 ", BUFFER[i] & 0xFF);
        }
        cmdIndex = 0;
        debugPrintWithoutIndent(0x00, "\n");
    }
    return BUFFER[cmdIndex++];
}

void Simul_saveDataInBin(void) {
    FILE* file = fopen("EEPROM.bin", "wb");
    fwrite(umbEEPRom, sizeof(*umbEEPRom), NVM_SIZE, file);
    fclose(file);
}

void resetGlobalVariables(void) {
    //reset all global variables to their default values
    
}
static void debugPrintWithoutIndent(unsigned char color, const char* message, ...)
{
    va_list arg_list;
    int cursor = 0;
    int finalMessageLength = 0;
    char finalMessage[500] = { 0 };
    char tmp[20];
    errno = 0;
    if (NULL == DbgFile) {
        printf("Error %d \n", errno);
    }
    va_start(arg_list, message);
    while (message && message[cursor])
    {
        if (message[cursor] == '%')
        {
            cursor++;
            switch (message[cursor])
            {
            case '2':
            {
                itoa(va_arg(arg_list, unsigned short), tmp, 16);
                break;
            }
            case '4':
            {
                itoa(va_arg(arg_list, unsigned int), tmp, 16);
                break;
            }
            case '8':
            {
                itoa(va_arg(arg_list, unsigned long int), tmp, 16);
                break;
            }
            case '1':
            default:
            {
                itoa(va_arg(arg_list, unsigned char), tmp, 16);
            }
            }
            /*Chnage the hex characters to upper case*/
            convertToUpperCase(tmp, strlen(tmp));

            /*If no. of digits in hex string is odd i.e. 1,3, add '0' before the string of digits*/
            if (strlen(tmp) % 2 != 0) {
                strcpy(&finalMessage[finalMessageLength], "0");
                finalMessageLength++;
            }

            /*copy string of digits*/
            strcpy(&finalMessage[finalMessageLength], tmp);
            finalMessageLength += strlen(tmp);

        }
        else
        {
            finalMessage[finalMessageLength] = message[cursor];
            finalMessageLength++;
        }
        cursor++;
    }
    /*Enable coloring of text*/
    if (color != 0x00) {
        SetConsoleTextAttribute(hConsole, color);
        //printf(color);
    }
#ifndef DEBUG_MEMORY_MANAGER
    if (strstr(finalMessage, "[MemoryManager]") == NULL)
#endif
    {
        fwrite(finalMessage, finalMessageLength, 1, stdout);
        fwrite(finalMessage, finalMessageLength, 1, DbgFile);
        fflush(DbgFile);
    }


    /*disable coloring of text*/
    if (color != 0x00) {
        SetConsoleTextAttribute(hConsole, 7);
    }
    va_end(arg_list);
}
void debugPrintArray(unsigned char color, const char* array, unsigned short len) {

    unsigned short index = 0;
    debugPrintWithoutIndent(color, "\n");
    debugPrint(color, ":");
    for (index = 0; index < len; index++) {
        debugPrintWithoutIndent(color, "%1 ", array[index]);
        if(0 == (index+1) % 0x10)
        {
            debugPrintWithoutIndent(color, "\n");
            debugPrint(color, ":");
        }
    }
    debugPrintWithoutIndent(color, "\n");
}
void backupIndent(void) {
    backIndentVal[trycatchindex++] = indentVal;
}
void restoreIndent(void) {
    indentVal = backIndentVal[--trycatchindex];
}
void addAIndent(void) {
    indentVal++;
}
void removeAIndent(void) {
    indentVal--;
}
void debugPrint(unsigned char color, const char* message, ...)
{
    va_list arg_list;
    int cursor = 0;
    int finalMessageLength = 0;
    char finalMessage[500] = { 0 };
    char tmp[20];
    errno = 0;
    if (NULL == DbgFile) {
        printf("Error %d \n", errno); 
    }
    va_start(arg_list, message);
    while (cursor < indentVal) {
        finalMessage[finalMessageLength++] = ' ';
        finalMessage[finalMessageLength++] = ' ';
        finalMessage[finalMessageLength++] = ' ';
        cursor++;
    }
    cursor = 0;
    while (message && message[cursor])
    {
        if (message[cursor] == '%')
        {
            cursor++;
            switch (message[cursor])
            {
                case '2':
                {
                    itoa(va_arg(arg_list, unsigned short), tmp, 16);
                    break;
                }
                case '4':
                {
                    itoa(va_arg(arg_list, unsigned int), tmp, 16);
                    break;
                }
                case '8':
                {
                    itoa(va_arg(arg_list, unsigned long int), tmp, 16);
                    break;
                }
                case '1':
                default:
                {
                    itoa(va_arg(arg_list, unsigned char), tmp, 16);
                }
            }
            /*Chnage the hex characters to upper case*/
            convertToUpperCase(tmp, strlen(tmp));

            /*If no. of digits in hex string is odd i.e. 1,3, add '0' before the string of digits*/
            if (strlen(tmp) % 2 != 0) {
                strcpy(&finalMessage[finalMessageLength], "0");
                finalMessageLength++;
            }

            /*copy string of digits*/
            strcpy(&finalMessage[finalMessageLength], tmp);
            finalMessageLength += strlen(tmp);

        }
        else
        {
            finalMessage[finalMessageLength] = message[cursor];
            finalMessageLength++;
        }
        cursor++;
    }
    /*Enable coloring of text*/
    if (color != 0x00) {
        SetConsoleTextAttribute(hConsole, color);
        //printf(color);
    }
#ifndef DEBUG_MEMORY_MANAGER
    if (strstr(finalMessage, "[MemoryManager]") == NULL)
#endif
    {
        fwrite(finalMessage, finalMessageLength, 1, stdout);
        fwrite(finalMessage, finalMessageLength, 1, DbgFile);
        fflush(DbgFile);
    }


    /*disable coloring of text*/
    if (color != 0x00) {
        SetConsoleTextAttribute(hConsole, 7);
    }
    va_end(arg_list);
}
