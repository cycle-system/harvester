/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/


#include "ch.h"
#include "hal.h"
#include "test.h"

#include <string.h>
#include <chmtx.h>

#include "main.h"
#include "utils.h"
#include "stpm.h"

/* =================================================*/
/* General control variable                         */
/*                                                  */
/*===================================================*/

#define RUN_MAIN_THREAD                 TRUE

char * energy_measure = "10";
STPM_REG_TypeDef registers;

static MUTEX_DECL(stpm_mtx);

/* =================================================*/
/* Thread definition                                */
/*                                                  */
/*===================================================*/

/*
 * Blinker thread #1.
 */

static THD_WORKING_AREA(waThread1, 64);
static THD_FUNCTION(Thread1, arg) {

    (void)arg;

    chRegSetThreadName("blinker");

    while (true) {
        palSetPad(GPIOD, 2);
        chThdSleepMilliseconds(150);
        palClearPad(GPIOD, 2);
        chThdSleepMilliseconds(150);
    }
}

/*
 * Bluetooth thread #2.
 */

static THD_WORKING_AREA(waThread2, 512);
static THD_FUNCTION(Thread2, arg) {

    (void)arg;

    chRegSetThreadName("query_stpm33");
    
    int * ptr = (int *) &registers;
    
    while(true){
		
		readInitSequence();
		queryAllRegisters(ptr,&stpm_mtx);
		
		chThdSleepMilliseconds(5000);
	}
    

}

/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

int main(void) {

    /*
	* Initialization of variables
	*/
	
	chMtxObjectInit(&stpm_mtx); 
		
    /*
		* System initializations.
		* - HAL initialization, this also initializes the configured device drivers
		*   and performs the board-specific initializations.
		* - Kernel initialization, the main() function becomes a thread and the
		*   RTOS is active.
		*/

    halInit();
    chSysInit();
    boardInit();

    /*
    * Activates the serial driver 1 using the driver default configuration.
    * PA9(TX) and PA10(RX) are routed to USART1.
    */

    sdStart(&SD2, NULL);
    sdStart(&SD3, NULL);

    /*
    * Creates the example threads.
    */

    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL);
    chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO+1, Thread2, NULL);

    /*
    * Normal main() thread activity, in this demo it does nothing except
    * sleeping in a loop and check the button state, when the button is
    * pressed the test procedure is launched.
    */

    // Change to

	//int a;
	
	//int rspValBin[32];	
	//char rsp[5];
	
	//char comm_1[] = "\x04\xff\xff\xff\x83";
	//char comm_2[] = "\xff\xff\xff\xff\x7b";
	//char comm_3[] = "\xff\x05\x60\x00\xe7";
	//char comm_4[] = "\x00\xff\xff\xff\xf0";
	//char comm_5[] = "\xff\xff\xff\xff\x7b";
			
	//sdWrite(&SD2, (uint8_t *) comm_1, 5);
	
	//chThdSleepMilliseconds(500);
		
	//sdReadTimeout(&SD2, (uint8_t *) rsp, sizeof(rsp), 2000);
	
	//chThdSleepMilliseconds(500);
	
	//sdWrite(&SD2, (uint8_t *) comm_2, 5);
	
	//chThdSleepMilliseconds(500);
		
	//sdReadTimeout(&SD2, (uint8_t *) rsp, sizeof(rsp), 2000);
	
	//chThdSleepMilliseconds(500);
	
	//sdWrite(&SD2, (uint8_t *) comm_3, 5);
	
	//chThdSleepMilliseconds(500);
		
	//sdReadTimeout(&SD2, (uint8_t *) rsp, sizeof(rsp), 2000);
	
	//chThdSleepMilliseconds(500);
	
	//sdWrite(&SD2, (uint8_t *) comm_4, 5);
	
	//chThdSleepMilliseconds(500);
		
	//sdReadTimeout(&SD2, (uint8_t *) rsp, sizeof(rsp), 2000);

	//chThdSleepMilliseconds(500);
	
	//for(a = 0; a<70; a++){
	
		//sdWrite(&SD2, (uint8_t *) comm_2, 5);
		
		//chThdSleepMilliseconds(500);
		
		//sdReadTimeout(&SD2, (uint8_t *) rsp, sizeof(rsp), 2000);
        
		//chThdSleepMilliseconds(500);
		
		//chprintf((BaseChannel *)&SD3, "R. %d: %X ", a, rsp[0]);
		//chprintf((BaseChannel *)&SD3, "%X ", rsp[1]);
		//chprintf((BaseChannel *)&SD3, "%X ", rsp[2]);
		//chprintf((BaseChannel *)&SD3, "%X ", rsp[3]);
		//chprintf((BaseChannel *)&SD3, "%X \n\r", rsp[4]);
	
		//chThdSleepMilliseconds(500);
	
		//memset(rsp, 0, sizeof(rsp));
	
	//};

	while (RUN_MAIN_THREAD) {

		// To Write the main thread
		
		chThdSleepMilliseconds(10000);
		
		chMtxLock(&stpm_mtx);
		
		chprintf((BaseChannel *)&SD3, "%Reg. 36: %X \n\r", registers.DSP_REG14);
        	
        chMtxUnlock(&stpm_mtx);	
        	
	}
        
}
