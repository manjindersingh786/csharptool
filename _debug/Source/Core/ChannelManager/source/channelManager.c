#include "channelManager.h"

/*It states channels availablity: if bit is clear i.e. '0' channel already opened and if its set i.e. '1' channel is available to open
* b0:'1' Basic Channel (always '0')
* b1:'1' Channel 1 
* b2:'1' Channel 2 
* b3:'1' Channel 3 
* b4:'0' This context stores info of RFM channel
*/
uMonoByte gumbChannelsState = STATE_ONLY_BASIC_CHANNEL_OPENED;

//Stores context fo all logical channels
tChannelContext	ccContexts[NO_OF_LOGICAL_CHANNELS + NO_OF_RFM_CHANNELS];