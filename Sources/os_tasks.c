/* ###################################################################
**     Filename    : os_tasks.c
**     Project     : lab_part2_serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-01-30, 16:17, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         serial_task - void serial_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif 



/* User includes (#include below this line is not maintained by Processor Expert) */
#include "constants.h"

_pool_id   message_pool;
MUTEX_STRUCT openR_mutex;
MUTEX_STRUCT openW_mutex;
read_list opened_for_read[10];
int opened_for_read_size = 0;
_task_id write_permission = 0;
_queue_id server_qid;

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data)
{
	// This is the "Handler"

	printf("serialTask Created!\n\r");

	char buffer[32] = "";
	char desired_output[32] = "";
	int count = 0;

   /* open a message queue */
   server_qid = _msgq_open(INPUT_QUEUE, 0);

   if (server_qid == 0) {
	  printf("\nCould not open the server message queue\n");
	  _task_block();
   }

   printf("\nMessage Queue Created!\n");

   /* create a message pool */
   message_pool = _msgpool_create(sizeof(CHARACTER_MESSAGE),
	  INITIAL_NUM_MESSAGES, 0, 0);

   if (message_pool == MSGPOOL_NULL_POOL_ID) {
	  printf("\nCount not create a message pool\n");
	  _task_block();
   }


   MUTEX_ATTR_STRUCT mutexattr1;

   /* Initialize mutex attributes */
   if (_mutatr_init(&mutexattr1) != MQX_OK) {
      printf("Initialize mutex attributes failed.\n");
      _task_block();
   }

   /* Initialize the mutex */
   if (_mutex_init(&openR_mutex, &mutexattr1) != MQX_OK) {
      printf("Initialize read mutex failed.\n");
      _task_block();
   }

   printf("\nRead Mutex initialized\n");

   MUTEX_ATTR_STRUCT mutexattr2;

   /* Initialize mutex attributes */
   if (_mutatr_init(&mutexattr2) != MQX_OK) {
      printf("Initialize mutex attributes failed.\n");
      _task_block();
   }

   /* Initialize the mutex */
   if (_mutex_init(&openW_mutex, &mutexattr2) != MQX_OK) {
      printf("Initialize write mutex failed.\n");
      _task_block();
   }

   printf("\nWrite Mutex initialized\n");

  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    
	CHARACTER_MESSAGE_PTR msg_ptr;
//	printf("Waiting for message\n");
	msg_ptr = _msgq_receive(server_qid, 0);

	if (msg_ptr == NULL) {
	   printf("\nCould not receive a message\n");
	   _task_block();
	}

	// Check if a user task has called OpenR
	if (opened_for_read_size < 1) {
		printf("\nA user task hasn't called OpenR\n");
		// Don't add to buffer
		continue;
	}

	int input = msg_ptr->DATA;

	switch(input){
		case 8:
			printf("Backspace\n");

			UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA), 1000);
			UART_DRV_SendDataBlocking(myUART_IDX, " ", sizeof(msg_ptr->DATA), 1000);
			UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA), 1000);

			if (count > 0) {
				count--;
			}

			break;
		case 23:
			printf("Erase Word\n");

			while (buffer[count] != ' ') {
				UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA), 1000);
				UART_DRV_SendDataBlocking(myUART_IDX, " ", sizeof(msg_ptr->DATA), 1000);
				UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA), 1000);

				if (count > 0) {
					count--;
				}
			}

			break;
		case 21:
			printf("Erase Line\n");


			while (count > 0) {
				UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA), 1000);
				UART_DRV_SendDataBlocking(myUART_IDX, " ", sizeof(msg_ptr->DATA), 1000);
				UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA), 1000);

				count--;
			}

			break;
		case 13:
		case 10:
			for (int i = 0; i < opened_for_read_size; i++) {
				/*allocate a message*/
				BUFFER_MESSAGE_PTR msg_ptr = (BUFFER_MESSAGE_PTR)_msg_alloc(message_pool);

				if (msg_ptr == NULL) {
					printf("\nCould not allocate a message\n");
					_task_block();
				}

				msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, opened_for_read[i].queueId);
				msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(char) * 32;
				strncpy(msg_ptr->DATA, buffer, 32);

				_msgq_send(msg_ptr);
				_msg_free(msg_ptr);
			}

			// Clear buffer
			memset(&buffer[0], 0, sizeof(buffer));

			// Clear line from screen
			while (count > 0) {
				UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA), 1000);
				UART_DRV_SendDataBlocking(myUART_IDX, " ", sizeof(msg_ptr->DATA), 1000);
				UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA), 1000);

				count--;
			}

			break;
		default:
			if (count > 31) {
				printf("Line at max length!\n");
			} else {
				UART_DRV_SendDataBlocking(myUART_IDX, &msg_ptr->DATA, sizeof(msg_ptr->DATA), 1000);

				buffer[count] = msg_ptr->DATA;
				count++;

				strncpy (desired_output, buffer, count);
				desired_output[count] = '\0';
			}
	}

	_msg_free(msg_ptr);

#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : Task1_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void Task1_task(os_task_param_t task_init_data)
{
	bool result = OpenR(_task_get_id());

	if (result == false) {
	   printf("\nOpenR failed task 1...\n");
	}

	char str[32];

	while(1) {
		result = _getline(str);

		if (result == false) {
			printf("\_getline failed task 1...\n");
			break;
		}

		printf("Task one received: %s\n", str);

		// Clear string
		memset(&str[0], 0, sizeof(str));
	}
}

/*
** ===================================================================
**     Callback    : Task2_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void Task2_task(os_task_param_t task_init_data)
{
	_queue_id writeQ;

	writeQ = OpenW();

	if (writeQ == 0) {
		printf("\nError getting write permission task 2\n");
	}

	char str[30];
	strcpy(str, "This was sent from Task 2");

	_putline(writeQ, str);

	bool result = Close();
	printf(result ? "Successfully closed task 2\n" : "Error closing task 2\n");
}

/*
** ===================================================================
**     Callback    : Task3_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void Task3_task(os_task_param_t task_init_data)
{
	bool result;

	result = OpenR(_task_get_id());

	if (result == false) {
	   printf("\nOpenR failed task 3...\n");
	}

	_queue_id writeQ;
	writeQ = OpenW();

	if (writeQ == 0) {
		printf("\nError getting write permission task 3\n");
	}

	char str[32];
	result = _getline(str);

	if (result == false) {
		printf("\_getline failed...\n");
	}

	printf("Task three received: %s\n", str);

	// Clear string
	memset(&str[0], 0, sizeof(str));

	result = Close();
	printf(result ? "Successfully closed task 3\n" : "Error closing task 3\n");
}

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
