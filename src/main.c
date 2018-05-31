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
#include "metroTask.h"
#include "metrology.h"
#include "metrology_hal.h"
#include "stpm_metrology.h"

/* =================================================*/
/* General control variable                         */
/*                                                  */
/*===================================================*/

#define RUN_MAIN_THREAD                 TRUE

METRO_NB_Device_t Device = EXT1;

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
    
    while(true){
		
		readInitSequence();
		queryAllRegisters(Device, &stpm_mtx);
		
		chThdSleepMilliseconds(2000);
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

    /*******************************************************************
    *	Prepare the framework to manage the data 
    *******************************************************************/

	/* Init STPM with complete sequence and set the registers*/
	
	METRO_Init();

	while (RUN_MAIN_THREAD) {

		chThdSleepMilliseconds(4000);
		
		chMtxLock(&stpm_mtx);
		
		METRO_Update_Measures();
		
		char * measures = getJsonMeasures();
		
		chprintf((BaseChannel *)&SD3, "%s", measures);
		
        chMtxUnlock(&stpm_mtx);	
		
		free(measures);
        	
	}
        
}
