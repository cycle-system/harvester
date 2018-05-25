/*!
 * @file stpm.c
 * @brief Source file for stpm3x device for energy sensing.
 *
 * @addtogroup STPM
 * @{
 */
 
#include <string.h> 
 
#include "ch.h"
#include "hal.h"

#include "stpm.h"
#include "utils.h"
 
/*===========================================================================*/
/* Definitions                     .                                         */
/*===========================================================================*/
 
#define DELAY_UART 500;
 
/*===========================================================================*/
/* Local variables                 .                                         */
/*===========================================================================*/

char comm[][5] = {"\x04\xff\xff\xff\x83",
				  "\xff\xff\xff\xff\x7b",
				  "\xff\x05\x60\x00\xe7",
				  "\x00\xff\xff\xff\xf0"
				};

/*===========================================================================*/
/* General functions                                                         */
/*===========================================================================*/

void initAllRegisters(int * registersAddress){
	
	int counter;
	int * ptr;
	
	for(counter = 0; counter < 69; counter++){
	
		// Address to write the value
		ptr = registersAddress + (counter*4)/(sizeof(int));		
		*ptr = (uint32_t) 0x0000;

	}
	
};

void readInitSequence(void){
	
	int i;
	char rsp[5];
	
	for(i = 0; i<4; i++){
		
		sdWrite(&SD2, (uint8_t *) comm[i], 5);
		chThdSleepMilliseconds(50);
		sdReadTimeout(&SD2, (uint8_t *) rsp, sizeof(rsp), 2000);
		chThdSleepMilliseconds(50);
		
	};
	
};

void queryAllRegisters(int * registersAddress, mutex_t * mtx){
	
	int counter;
	char rsp[5];
	int rspValBin[32];
	int * ptr;
	
	for(counter = 0; counter<70; counter++){
	
		ptr = registersAddress + (counter*4)/(sizeof(int));
	
		sdWrite(&SD2, (uint8_t *) comm[1], 5);
		
		chThdSleepMilliseconds(50);
		
		sdReadTimeout(&SD2, (uint8_t *) rsp, sizeof(rsp), 2000);
        
		getLSBFirstBinary(rsp, rspValBin);
		
		chMtxLock(mtx);
		
		*ptr = (uint32_t) bin2Dec(rspValBin, sizeof(rspValBin)/sizeof(rspValBin[0]));
		
		chMtxUnlock(mtx);
		
		memset(rsp, 0, sizeof(rsp));
	
	};
	
};
