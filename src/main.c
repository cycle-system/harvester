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

#include <math.h>

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "main.h"

/* =================================================*/
/* General control variable                         */
/*                                                  */
/*===================================================*/

#define RUN_MAIN_THREAD                 TRUE

char * energy_measure = "10";

/* =================================================*/
/* Thread definition                                */
/*                                                  */
/*===================================================*/

/*
 * Blinker thread #1.
 */

static THD_WORKING_AREA(waThread1, 128);
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

static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {

    (void)arg;

    chRegSetThreadName("th_2");

}


void getLSBFirstBinary(char * data, int binData[]){

	int n, c, k, i;
	
	for(i = 0; i < sizeof(data); i++){
		
		chprintf((BaseChannel *)&SD3, "Value %d: %X.\n\r", i, data[i]);
		
		n = (int) data[i];
        
        for (c = 7; c >= 0; c--){
			
			k = n >> c;

			if (k & 1){
				binData[c+i*8] = 1;			
			}
			else {
				binData[c+i*8] = 0;		
			}	
		}
	
	}
	
}

unsigned long bin2Dec(int * array, int length){
	
	int i;
	unsigned long sum = 0;
	
	for(i = 0; i < length; i++)
		sum += array[i]*pow(2,i);
		
	return sum;
		
}

void printIntArray(int * array, int length){
	
	int i;
	
	for(i = length-1; i >= 0; i--)
		chprintf((BaseChannel *)&SD3, "%d", array[i]);
	
	chprintf((BaseChannel *)&SD3, "\n\r");	
		
}

/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

int main(void) {

    /*
    	* Initialization of variables
    	*/


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
    //chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO+1, Thread2, NULL);

    /*
    * Normal main() thread activity, in this demo it does nothing except
    * sleeping in a loop and check the button state, when the button is
    * pressed the test procedure is launched.
    */

    // Change to

    while (RUN_MAIN_THREAD) {
		
		// ...
        
        int rspValBin[32];
        char rsp[5];
        char testComm_1[] = "\x04\xff\xff\xff\x83";
        
        sdWrite(&SD2, (uint8_t *) testComm_1, 5);
		
		sdReadTimeout(&SD2, rsp, sizeof(rsp), 2000);
        
        getLSBFirstBinary(rsp, rspValBin);
        
        printIntArray(rspValBin, sizeof(rspValBin)/sizeof(rspValBin[0]));		
        
        unsigned long rspValDec = bin2Dec(rspValBin, sizeof(rspValBin)/sizeof(rspValBin[0]));
        
        chprintf((BaseChannel *)&SD3, "Value Dec: %lu \n\r", rspValDec);
        
        memset(rspValBin, 0, sizeof(rspValBin));
        memset(rsp, 0, sizeof(rsp));
        	
	}
        
}
