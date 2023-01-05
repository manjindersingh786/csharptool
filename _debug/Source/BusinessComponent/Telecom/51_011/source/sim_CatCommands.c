#include"51_011.h"

/*
* It implements the Terminal Profile command
*/
void _51011_terminalProfile(void) {

	//TODO  CAT commands
}

/*
* It implements the Terminal Response command
*/
void _51011_terminalResponse(void) {

	//TODO  CAT commands
}

/*
* It implements the Envelope command
*/
void _51011_envelope(void) {

	DBG_FUNC_ENTRY(">>[102221] _51011_envelope() \n");

	DBG_FUNC("[102221] _51011_envelope: ENVELOPE command will be executed as per UICC\n");
	_102221_envelope();

	DBG_FUNC_EXIT("<<[102221] _51011_envelope() \n");
}

/*
* It implements the Fetch command
*/
void _51011_fetch(void) {

	//TODO  CAT commands
}