;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                                       ATmega128  Specific code
;                                           GNU AVR Compiler
;
;
; File     : avr_isr.s (originally bsp_isr.s)
; By       : JJL
;          : FT
;          : [with modifications by Nick D'Ademo]
;********************************************************************************************************

#include  "os_cpu_i.h"


;********************************************************************************************************
;                                         PUBLIC DECLARATIONS
;********************************************************************************************************

		.global TickISR


;********************************************************************************************************
;                                         EXTERNAL DECLARATIONS
;********************************************************************************************************

		.extern OSIntNesting
		.extern OSTCBCur
		.extern OSTimeTick
		.extern OSIntExit


        .text

;/*$PAGE*/
;********************************************************************************************************
;                                           SYSTEM TICK ISR
;
; Description : This function is the Tick ISR.
;
;               The following C-like pseudo-code describe the operation being performed in the code below.
;
;               - Save all registers on the current task's stack:
;                      Use the PUSH_ALL macro
;               - OSIntNesting++;
;               if (OSIntNesting == 1) {
;                  OSTCBCur->OSTCBStkPtr = SP
;               }
;               Clear the interrupt;                  Not needed for the timer we used.
;               OSTimeTick();                         Notify uC/OS-II that a tick has occured
;               OSIntExit();                          Notify uC/OS-II about end of ISR
;               Restore all registers that were save on the current task's stack:
;                      Use the POP_ALL macro to restore the remaining registers
;               Return from interrupt
;********************************************************************************************************

               
TickISR:       
		PUSH_ALL                                                ; Save all registers and status register        	
		LDS     R16,OSIntNesting                                ; Notify uC/OS-II of ISR
        INC     R16                                             ;
        STS     OSIntNesting,R16                                ;

        CPI     R16,1                                           ; if (OSIntNesting == 1) {
        BRNE    TickISR_1

        SAVE_SP				                                    ; X = SP 		
		LDS     R28,OSTCBCur                                    ; OSTCBCur->OSTCBStkPtr = X
        LDS     R29,OSTCBCur+1                                  ;    
        
		ST      Y+,R26
        ST      Y+,R27                                          ; }

TickISR_1:
        CALL    OSTimeTick                                  	; Handle the tick ISR

        CALL    OSIntExit                                       ; Notify uC/OS-II about end of ISR
        		
        POP_ALL                                                 ; Restore all registers
        
        RETI

