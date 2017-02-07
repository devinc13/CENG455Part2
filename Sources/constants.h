#ifndef __constants_h__
#define __constants_h__


#include <message.h>
#include <mutex.h>

#define INPUT_QUEUE 8
#define ISR_QUEUE 9

#define INITIAL_NUM_MESSAGES 10


/* This structure contains a data field and a message header structure */
typedef struct character_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char DATA;
} CHARACTER_MESSAGE, * CHARACTER_MESSAGE_PTR;

typedef struct buffer_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char DATA[32];
} BUFFER_MESSAGE, * BUFFER_MESSAGE_PTR;

typedef struct read_list
{
   _task_id taskId;
   _queue_id queueId;
} read_list;

extern _pool_id message_pool;
extern read_list opened_for_read[10];
extern int opened_for_read_size;
extern MUTEX_STRUCT openR_mutex;


#endif
