#ifndef __constants_h__
#define __constants_h__


#include <message.h>

#define INPUT_QUEUE 8
#define ISR_QUEUE 9

#define INITIAL_NUM_MESSAGES 10


/* This structure contains a data field and a message header structure */
typedef struct character_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char DATA;
} CHARACTER_MESSAGE, * CHARACTER_MESSAGE_PTR;

extern _pool_id message_pool;

#endif
