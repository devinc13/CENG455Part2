#include "access_functions.h"

// stream_no is task id
bool OpenR(uint16_t stream_no)  {
	printf("Starting OpenR");

	/* open a message queue */
	_queue_id task_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);

	if (task_qid == 0) {
	  printf("\nCould not open the task1 message queue\n");
	  _task_block();
	}

	printf("\nOpenR Message Queue Created!\n");


	if (_mutex_lock(&openR_mutex) != MQX_OK) {
	 printf("\nMutex lock failed.\n");
	 _task_block();
	}

	read_list new_read = {stream_no, task_qid};
	opened_for_read[opened_for_read_size] = new_read;
	opened_for_read_size++;

	printf("\nOpenR WORKS!\n");

	_mutex_unlock(&openR_mutex);
	return true;
}

bool _getline(char *string) {
	_queue_id queueId;
	_task_id taskId = _task_get_id();

	printf("\nStarting _getline\n");

	for (int i = 0; i < opened_for_read_size; i++) {
		if (opened_for_read[i].taskId == taskId) {
			printf("\nFound the queueId\n");
			queueId = opened_for_read[i].queueId;
		}
	}

	if (queueId == NULL) {
		return false;
	}

	printf("\nWaiting for message\n");
	BUFFER_MESSAGE_PTR msg_ptr;
	msg_ptr = _msgq_receive(queueId, 0);

	if (msg_ptr == NULL) {
	   printf("\nCould not receive a message\n");
	   return false;
	}

	strncpy(string, msg_ptr->DATA, 32);

	_msg_free(msg_ptr);

	return true;
}

_queue_id OpenW() {
	if (_mutex_lock(&openW_mutex) != MQX_OK) {
		printf("\nMutex lock failed.\n");
		return 0;
	}

	if (write_permission != 0) {
		printf("\nWrite request failed\n");
		_mutex_unlock(&openW_mutex);
		return 0;
	}

	write_permission = _task_get_id();

	_mutex_unlock(&openW_mutex);

	return _msgq_get_id(INPUT_QUEUE, 0);
}

bool _putline(_queue_id qid, char *string) {
	if (_mutex_lock(&openW_mutex) != MQX_OK) {
		printf("\nMutex lock failed.\n");
		return False;
	}

	_task_id task_id = _task_get_id();

	if (write_permission != task_id) {
		printf("\nTask doesn't have write permission\n");
		_mutex_unlock(&openW_mutex);
		return False;
	}

	strcat(string, "\n");

	for (int i = 0; i < string->size(); i++) {
		  /*allocate a message*/
		   CHARACTER_MESSAGE_PTR msg_ptr = (CHARACTER_MESSAGE_PTR)_msg_alloc(message_pool);

		  if (msg_ptr == NULL) {
			 printf("\nCould not allocate a message\n");
				_mutex_unlock(&openW_mutex);
				return False;
		  }

		  msg_ptr->HEADER.TARGET_QID = qid;
		  msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(char);
		  msg_ptr->DATA = string[i];

		  _msgq_send(msg_ptr);
		  _msg_free(msg_ptr);
	}

	_mutex_unlock(&openW_mutex);

	printf("_putline WORKS!");
	return True;
}

bool Close(void) {
	printf("Close WORKS!");
}
