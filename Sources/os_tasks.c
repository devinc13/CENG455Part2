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

//	char buf[13];
//	sprintf(buf, "\n\rType here: ");
//	UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);

	char buffer[32] = "";
	char desired_output[32] = "";
	int count = 0;

   /* open a message queue */
   _queue_id server_qid = _msgq_open(INPUT_QUEUE, 0);

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



  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    
	CHARACTER_MESSAGE_PTR msg_ptr;
	printf("Waiting for message\n");
	msg_ptr = _msgq_receive(server_qid, 0);

	if (msg_ptr == NULL) {
	   printf("\nCould not receive a message\n");
	   _task_block();
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
		default:

			printf(" %c \n", msg_ptr->DATA);

			if (count > 31) {
				printf("Line at max length!\n");
			} else {
				UART_DRV_SendData(myUART_IDX, &msg_ptr->DATA, sizeof(msg_ptr->DATA));

				buffer[count] = msg_ptr->DATA;
				count++;

				strncpy (desired_output, buffer, count);
				desired_output[count] = '\0';

				printf("%s\n", desired_output);
			}
	}


	_msg_free(msg_ptr);

    
#ifdef PEX_USE_RTOS   
  }
#endif    
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
