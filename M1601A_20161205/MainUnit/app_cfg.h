/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                             Atmel Xmega128A1 Application Configuration File
*
*                                 (c) Copyright 2007; Micrium; Weston, FL
*                                           All Rights Reserved
*
* File    : APP_CFG.H
* By      : FK
*         : FT
*         : [with modifications by Nick D'Ademo]
*********************************************************************************************************
*/

#ifndef _APP_CFG_H_
#define _APP_CFG_H_
        
/*
**************************************************************************************************************
*                                             TASK PRIORITIES
**************************************************************************************************************
*/

#define  APP_CFG_TASK_START_PRIO                2

#define  OS_TASK_TMR_PRIO              			(OS_LOWEST_PRIO - 2)

/*
**************************************************************************************************************
*                                               STACK SIZES
**************************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE          	100
#define  APP_CFG_TASK_STK_SIZE					350

/*
**************************************************************************************************************
*                                             NUMBER OF TASKS
**************************************************************************************************************
*/

#define  APP_CFG_N_TASKS						5

#endif
