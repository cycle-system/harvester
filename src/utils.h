/*!
 * @file utils.h
 * @brief Header file for utilities.
 *
 * @addtogroup STPM
 * @{
 */

#include "ch.h"
#include "hal.h"

/*===========================================================================*/
/* Local variables                 .                                         */
/*===========================================================================*/

/*===========================================================================*/
/* General functions                                                         */
/*===========================================================================*/

void getLSBFirstBinary(char * data, int binData[]);
unsigned long bin2Dec(int * array, int length);
void printIntArray(int * array, int length);

