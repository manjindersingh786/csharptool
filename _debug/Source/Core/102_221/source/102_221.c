#include "102_221.h"

//Variables definition

//Points to current channel
tChannelContext* pccCurrentChannel;

/*Stores currently selected NAA application i.e. UICC,USIM,SIM*/
//Default application for now is UICC
uMonoByte gumbCurrentNaaApp = NAA_UICC;

uMonoByte gumbcommandSource = SRC_TERMINAL;