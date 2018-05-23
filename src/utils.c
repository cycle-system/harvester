/*!
 * @file utils.c
 * @brief Source file for utilities.
 *
 * @addtogroup STPM
 * @{
 */
 
#include <math.h>

#include "ch.h"
#include "hal.h"

#include "utils.h"
 
/*===========================================================================*/
/* Local variables                 .                                         */
/*===========================================================================*/

/*===========================================================================*/
/* General functions                                                         */
/*===========================================================================*/

/*!
 * \brief Process string into useful data in binary
 *
 * \param[in] 
 * \param[in] 
 * \return 
 */
 
void getLSBFirstBinary(char * data, int binData[]){

	int n, c, k, i;
	
	for(i = 0; i < (int) sizeof(data); i++){
		
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
	
	printIntArray(binData, 32);
	
}

/*!
 * \brief Process string into useful data in binary
 *
 * \param[in] 
 * \param[in] 
 * \return 
 */
 
unsigned long bin2Dec(int * array, int length){
	
	int i;
	unsigned long sum = 0;
	
	for(i = 0; i < length; i++)
		sum += array[i]*pow(2,i);
		
	return sum;
		
}

/*!
 * \brief Process string into useful data in binary
 *
 * \param[in] 
 * \param[in] 
 * \return 
 */
 
void printIntArray(int * array, int length){
	
	int i;
	
	for(i = length-1; i >= 0; i--)
		chprintf((BaseChannel *)&SD3, "%d", array[i]);
	
	chprintf((BaseChannel *)&SD3, "\n\r");	
		
}
