/*!
 * @file utils.c
 * @brief Source file for utilities.
 *
 * @addtogroup STPM
 * @{
 */

#include <stdio.h>
#include <math.h>
#include <cJSON.h>

#include "ch.h"
#include "hal.h"

#include "utils.h"
#include "metroTask.h"
 
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

/*!
 * \brief Create a JSON string with the measures
 *
 * \param[in] 
 * \param[in] 
 * \return 
 */
 
char * getJsonMeasures(void) {
	
    char * string = NULL;
    
    cJSON *monitor = cJSON_CreateObject();
    
    if (monitor == NULL){
        goto end;
    }
    
    /*** Active energy ***/
    
    if (cJSON_AddNumberToObject(monitor,"energy_active",metroData.energyActive) == NULL) {
		goto end;
	}
    
    /*** Apparent energy ***/
    
    if (cJSON_AddNumberToObject(monitor,"energy_apparent",metroData.energyApparent) == NULL) {
		goto end;
	}
    
    /*** Reactive energy ***/
    
    if (cJSON_AddNumberToObject(monitor,"energy_reactive",metroData.energyReactive) == NULL) {
		goto end;
	}
    
    
    /*** Active power ***/
    
    if (cJSON_AddNumberToObject(monitor,"power_active",metroData.powerActive) == NULL) {
		goto end;
	}
    
	/*** Apparent power ***/
    
    if (cJSON_AddNumberToObject(monitor,"power_apparent",metroData.powerApparent) == NULL) {
		goto end;
	}
    
    /*** Reactive power ***/
    
    if (cJSON_AddNumberToObject(monitor,"power_reactive",metroData.powerReactive) == NULL) {
		goto end;
	}
   
	/*** RMS Current ***/
    
    if (cJSON_AddNumberToObject(monitor,"curent_rms",metroData.rmscurrent) == NULL) {
		goto end;
	}
	
    /*** RMS Voltage ***/
    
    if (cJSON_AddNumberToObject(monitor,"voltage_rms", (double) metroData.rmsvoltage/1000) == NULL) {
		goto end;
	}
    
    string = cJSON_Print(monitor);
    
    if (string == NULL) {
		chprintf((BaseChannel *)&SD3, "Failed to print monitor. \n\r");
    }

    
end:
    
    cJSON_Delete(monitor);
    return string;
    
}
