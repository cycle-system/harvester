/*!
 * @file stpm.c
 * @brief Source file for stpm3x device for energy sensing.
 *
 * @addtogroup STPM
 * @{
 */
 
#include "ch.h"
#include "hal.h"

#include "stpm.h"
#include "utils.h"
 
/*===========================================================================*/
/* Local variables                 .                                         */
/*===========================================================================*/

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

void queryAllRegisters(int * registersAddress){
	
	int counter;
	
	char comm_base[] = "\x04\xff\xff\xff\x83";
	char comm_next[] = "\xff\xff\xff\xff\x7b";
	char rsp[5];
	
	int rspValBin[32];
	
	int * ptr;
	
	for(counter = 0; counter < 69; counter++){
	
		// Address to write the value
		ptr = registersAddress + (counter*4)/(sizeof(int));
		
		if(counter == 0){
			sdWrite(&SD2, (uint8_t *) comm_base, 5);
		}
		else{ 
			sdWrite(&SD2, (uint8_t *) comm_next, 5);
		}
		
		sdReadTimeout(&SD2, (uint8_t *) rsp, sizeof(rsp), 2000);
			
		chprintf((BaseChannel *)&SD3, "Registro %d \n\r", counter);

		getLSBFirstBinary(rsp, rspValBin);

		*ptr = (uint32_t) bin2Dec(rspValBin, sizeof(rspValBin)/sizeof(rspValBin[0]));

		chThdSleepMilliseconds(500);
		
	}
	
};
