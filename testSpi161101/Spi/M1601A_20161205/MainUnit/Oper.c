/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <includes.h>
#include <avr/wdt.h>
#include "cc1100.h"
#include "port.h"
#include "Oper.h"
#include "env.h"
#include "event.h"
#include "rf_task.h"
#include "menu.h"
#include "key.h"


OS_EVENT *      oper_wait;



oper_t		gOper[MAX_OPER];





void OperTask(void *p_arg)
{
	oper_t * oper;


}


