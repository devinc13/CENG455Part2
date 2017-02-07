/*
 * access_functions.h
 *
 *  Created on: Feb 6, 2017
 *      Author: devinc
 */

#ifndef SOURCES_ACCESS_FUNCTIONS_H_
#define SOURCES_ACCESS_FUNCTIONS_H_

#include <stdbool.h>
#include <stdint.h>
#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "constants.h"

bool OpenR(uint16_t stream_no);
bool _getline(char *string);
_queue_id OpenW();
bool _putline(_queue_id qid, char *string);
bool Close(void);

#endif /* SOURCES_ACCESS_FUNCTIONS_H_ */
