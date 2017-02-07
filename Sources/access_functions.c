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

	printf("\nWaiting for message\n");
	BUFFER_MESSAGE_PTR msg_ptr;
	msg_ptr = _msgq_receive(queueId, 0);

	printf("\nThis should be our string\n");
	printf("\n%s\n", msg_ptr->DATA);

	strncpy(string, msg_ptr->DATA, 32);

	printf("_getline WORKS! - message received and copied");
	_msg_free(msg_ptr);

	return true;
}

_queue_id OpenW() {
	printf("OpenW WORKS!");
}

bool _putline(_queue_id qid, char *string) {
	printf("_putline WORKS!");
}

bool Close(void) {
	printf("Close WORKS!");
}
