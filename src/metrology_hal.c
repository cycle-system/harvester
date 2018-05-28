/**
  ******************************************************************************
  * @file    metrology_hal.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    11-March-2016
  * @brief   This file provides The hardware abstraction for Metrology Block.
  * @brief
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* ==========================================================================
 Includes                      
============================================================================*/

#include <string.h>
#include <stdint.h>
 
#include "ch.h"
#include "hal.h"

#include "utils.h"
#include "main.h"
#include "stpm_metrology.h"
#include "metrology.h"
#include "metrology_hal.h"

/* ==========================================================================
 Private typedef                      
============================================================================*/

/* ==========================================================================
 Private define                      
============================================================================*/

#define METRO_BUFF_COM_MAXSIZE  ((uint8_t)(40))

#define CRC_8 (0x07)
#define STPM3x_FRAME_LEN (5)

#define WAIT_DURATION   500   /* 500 * 1 ms = 500 ms */

#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed.
  *   If expr is true, it returns no value.
  * @retval None
  */

#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))


/* ==========================================================================
 Exported functions                      
============================================================================*/

void assert_failed(uint8_t* file, uint32_t line);

#else
	#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */


/* ==========================================================================
 Private macro                      
============================================================================*/

/* ==========================================================================
 Private variables                      
============================================================================*/
 
char comm[][5] = {"\x04\xff\xff\xff\x83",
				  "\xff\xff\xff\xff\x7b",
				  "\xff\x05\x60\x00\xe7",
				  "\x00\xff\xff\xff\xf0"
				};
				
/* ==========================================================================
 Private function prototypes                      
============================================================================*/

#ifdef UART_XFER_STPM3X /* UART MODE */  

	static void Metro_HAL_usart_config(uint32_t in_baudrate);
	static void Metro_HAL_UsartTxStart(METRO_NB_Device_t in_Metro_Device_Id);
	static uint8_t UARTWrp_SendAndReceiveByte(UART_HandleTypeDef *huart,uint8_t data);
	static uint8_t Metro_HAL_byteReverse(uint8_t in_byte);

#endif

#ifdef SPI_XFER_STPM3X /* SPI MODE */ 

	static void Metro_HAL_Spi_config(void);
	static void Metro_HAL_SpiTxStart(METRO_NB_Device_t in_Metro_Device_Id);
	static uint8_t SPIWrp_SendAndReceiveByte(SPI_HandleTypeDef *hspi,uint8_t data);

#endif

/* ==========================================================================
 Global variables                     
============================================================================*/

volatile uint32_t waitDummyCounter;

METRO_Device_Config_t * p_Metro_Device_Config;
uint8_t Metro_Com_TxBuf[METRO_BUFF_COM_MAXSIZE];
uint8_t Metro_Com_RxBuf[METRO_BUFF_COM_MAXSIZE];

/* ==========================================================================
 Private functions                   
============================================================================*/

/** @defgroup Metrology_HAL_Private_Functions
  * @{
  */

/** @defgroup Metrology_Group1 Initialization and Configuration functions
  *  @brief   Initialization and Configuration functions
  *
  * 
	@verbatim
	===============================================================================
					functions
	===============================================================================

	This section provides a set of functions to make the porting of Metrology Block from STCOMET or STMET or STPM

	@endverbatim
  * @{
  */

/* ==========================================================================
 Custom functions                   
============================================================================*/

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

void queryAllRegisters(METRO_NB_Device_t Device, mutex_t * mtx){
	
	int counter;
	char rsp[5];
	int rspValBin[32];
	int * ptr;
	
	for(counter = 0; counter<70; counter++){
	
		ptr = (int *) &(p_Metro_Device_Config[Device].metro_stpm_reg) + (counter*4)/(sizeof(int));
	
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

/* ==========================================================================
 General functions                   
============================================================================*/

/**
  * @brief  HAL  metrology Config
  *
  *
  * @retval U32
  */
  
/* set metrology HAL Config  */

uint8_t Metro_HAL_Setup(METRO_Device_Config_t * in_p_Metro_Config) {

	/* Save the pointer of Config table */
	p_Metro_Device_Config = in_p_Metro_Config;

	return 0;

}


/**
  * @brief  Read Block Registers from device
  *
  *
  * @retval void
  */
  
uint8_t Metrology_HAL_ReadBlock(METRO_NB_Device_t in_Metro_Device_Id, uint8_t Offset, uint8_t BlockNum, uint32_t * out_p_Buffer) {
  
	uint32_t tmp_addr = 0x0;
	uint8_t  error=0;

	if ( in_Metro_Device_Id < NB_MAX_DEVICE) {
		
	/* Calculate the base address to read inisde STPM chip  */
	/* the offset should be provided (2 bytes -> 16 bits) format for STPM */
	
	tmp_addr = (uint32_t)&METRO_STPM->DSPCTRL1 + Offset;

	/* read blocks from external chip */
	
	Metro_HAL_Stpm_Read(in_Metro_Device_Id,(uint8_t*)&tmp_addr,BlockNum,out_p_Buffer);

	}
	
	return error;
}

/**
  * @brief  Write Block Registers to device
  *
  *
  * @retval void
  */

uint8_t Metrology_HAL_WriteBlock(METRO_NB_Device_t in_Metro_Device_Id, uint8_t Offset, uint8_t BlockNum, uint32_t * in_p_Buffer) {
  
	uint32_t tmp_addr = 0x0;
	uint32_t ret_size;

	/* Calculate the base address to read inisde STPM chip  */
	/* the offset should be provided in 2 bytes format (16 bits by 16 bits) for STPM */
	
	tmp_addr = (uint32_t)&METRO_STPM->DSPCTRL1 + (Offset);

	/* write blocks from external chip */
	
	ret_size = Metro_HAL_Stpm_write(in_Metro_Device_Id,(uint8_t*)&tmp_addr,BlockNum,in_p_Buffer,STPM_WAIT); 

	return(ret_size);
}


/**
  * @brief  Init Metrology devices
  *
  *
  * @retval void
  */
  
void Metro_HAL_init_device(METRO_NB_Device_t in_Metro_Device_Id) {
 
	#ifdef UART_XFER_STPM3X /* UART MODE */ 
	
		/* Config UART */
		
		// TODO
		
	#endif    

	#ifdef SPI_XFER_STPM3X /* UART MODE */   
		
		/* Config SPI */
		
		// TODO
		
	#endif 
  
    /* set good CS with good EXT chip */
    
    /* Reset EXT Chip */
    /* 3 pulses on SYN */
               
    /* Single pulse on CS rev BC */        
    
    // TODO
      
	/* Wait 20 ms */
   
	
	
	/* Make one access ( first reg u32 reg) to ext chip to check if it is available */
   
	// TODO
  
}

/**
  * @brief  Enable Metrology devices by setting up and down the EN pin
  *         When EN is low the CSS is set low or high depending of the 
  *         protocol used (SPI or UART)
  * @retval void
  */

void Metro_HAL_power_up_device(METRO_NB_Device_t in_Metro_Device_Id) {
  
	#ifdef UART_XFER_STPM3X /* UART MODE */
	
		/* set UART mode at STPM3x power up, we have to set SS pin */
		
		// TODO
	   
	#endif

	#ifdef SPI_XFER_STPM3X /* SPI MODE */
	   
		/* set SPI mode at STPM3x power up, we have to reset SS pin */
		
		// TODO
		
	#endif

	/* set ENable Pin configured as low in CubeMX*/
	
	// TODO 

}

/**
  * @brief      This function reset device requested
  * @param[in]   in_Metro_Device_Id (device ID), EXT1 to EXT4
  * @param[in]   Reset type
  * @param[out]  none
  * @retval
  */
  
void Metro_HAL_reset_device(METRO_ResetType_t in_MetroResetType,METRO_NB_Device_t in_Metro_Device_Id) {
	uint32_t tmp_addr = 0;

	if(in_Metro_Device_Id < NB_MAX_DEVICE) {
		
		if (in_MetroResetType ==  RESET_SW){
			
			/* Set the reset bit in the  DSP Control Register 3 of stpm requested(STPM) */
			/* First put the bit inside internal struct */
		  
			p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3 |= METRO_STPM_Reset_Bit;

			/* second : write into external the chip */
			/* Calculate the base address to read inisde STPM chip  */
			/* the offset should be provided in 2 bytes format (16 bits by 16 bits) for STPM */
			
			tmp_addr = (uint32_t)((uint8_t*)&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3 - (uint8_t*)&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL1)/2;

			/* Write blocks inside external chip */
			
			Metro_HAL_Stpm_write(in_Metro_Device_Id,(uint8_t*)&tmp_addr,1,&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3,STPM_WAIT);

		}
			
		/* reset SYN hardware is requested, send 3 pulses to SYN signal pin */
	
		else if (in_MetroResetType ==  RESET_SYN_SCS) {
		
			/* Reset ext STPMs with syn PIN : 3 pulses are needed to reset STPM chips */
			
		}
	}
}

/**
  * @brief      This function set latch inside Metrology devices
               Latch the device registers according to the latch type selection driving SYN pin
               or writing S/W Latchx bits in the DSP_CR3 register
               or setting auto-latch by S/W Auto Latch bit in the DSP_CR3 register
  * @param[in]   in_Metro_Device_Id (device ID), EXT1 to EXT4
  * @param[in]   Latch type
  * @param[out]  none
  * @retval
  */
void Metro_HAL_Set_Latch_device_type(METRO_NB_Device_t in_Metro_Device_Id, METRO_Latch_Device_Type_t in_Metro_Latch_Device_Type) {
	
	uint32_t tmp_addr = 0;

	if(in_Metro_Device_Id < NB_MAX_DEVICE) {
	
		switch (in_Metro_Latch_Device_Type) {

			case LATCH_AUTO:
			{

				/* reset latch 1 and 2 bits in the internal DSP Control Register 3 */
				p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3 &= ~ BIT_MASK_STPM_LATCH1;
				p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3 &= ~ BIT_MASK_STPM_LATCH2;


				/* Set  latch auto in the internal DSP Control Register 3*/
				p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3 |= BIT_MASK_STPM_AUTO_LATCH;

				/* Now send data to the external chip */
				/* Calculate the base address to read inisde STPM chip  */
				/* the offset should be provided in 2 bytes format (16 bits by 16 bits) for STPM */
				tmp_addr = (uint32_t)((uint8_t*)&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3 - (uint8_t*)&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL1)/2;

				/* Write register inside external chip */
				Metro_HAL_Stpm_write(in_Metro_Device_Id,(uint8_t*)&tmp_addr,1,&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3,STPM_WAIT);

			}
			break;
			case LATCH_SW:
			{
				/* Set  latch SW 1 et 2 for the Two channels  the internal DSP Control Register 3*/
				p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3 |= BIT_MASK_STPM_LATCH1|BIT_MASK_STPM_LATCH2;

				/* Now send data to the external chip */
				/* Calculate the base address to read inisde STPM chip  */
				/* the offset should be provided in 2 bytes format (16 bits by 16 bits) for STPM */
				tmp_addr = (uint32_t)((uint8_t*)&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3 - (uint8_t*)&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL1)/2;

				/* Write register inside external chip */
				Metro_HAL_Stpm_write(in_Metro_Device_Id,(uint8_t*)&tmp_addr,1,&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL3,STPM_WAIT);

			}
			break;
			case LATCH_SYN_SCS:
			{
				/* Latch external chip with syn PIN : 1 pulses is needed to latch */
				
			}
			break;
			}


			/* After latch with syn pin or SW reg , we have to retreive metrology data from STPM external chip requested */
			/* from DSPEVENT1 to TOT_REG4 : 49 U32 reg from STPM*/
			/* Calculate the base address of Metrology data to read inisde STPM chip  */
			/* the offset should be provided in 2 bytes format (16 bits by 16 bits) for STPM */
			tmp_addr = (uint32_t)((uint8_t*)&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.UARTSPISR - (uint8_t*)&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSPCTRL1)/2;

			Metro_HAL_Stpm_Read(in_Metro_Device_Id,(uint8_t*)&tmp_addr,METRO_STPM_DATA_REG_NB_BLOCKS,&p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.UARTSPISR);

	}

}

/**
  * @brief      This function read the Power of signal come from one Channel mapped in one device
* @param[in]   in_Metro_Device_Id : EXT1 to EXT4
  * @param[in]   in_Metro_int_Channel (Channel ID), Channel 1 or 2 ( according to device )
  * @param[in]   in_Metro_Power_Selection : W_ACTIVE , F_ACTIVE, REACTIVE, APPARENT_RMS, APPARENT_VEC, MOM_WIDE_ACT, MOM_FUND_ACT
  * @param[out]  None
  * @retval     int32_t raw_power according to power type  read inside device register
  */

int32_t Metro_HAL_read_power(METRO_NB_Device_t in_Metro_Device_Id,METRO_internal_Channel_t in_Metro_int_Channel,METRO_Power_selection_t in_Metro_Power_Selection) {
	int32_t raw_power = 0;

	switch (in_Metro_Power_Selection) {
		
		case (W_ACTIVE):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG5)&BIT_MASK_STPM_PRIM_CURR_ACTIVE_POW);
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG5)&BIT_MASK_STPM_SEC_CURR_ACTIVE_POW);
			}
		}
		break;
		case (F_ACTIVE):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG6)&BIT_MASK_STPM_PRIM_CURR_FUND_POW);
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG6)&BIT_MASK_STPM_SEC_CURR_FUND_POW);
			}
		}
		break;
		case (REACTIVE):
		{
			/* is Channel is the first or the second  channel of the chip */
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG7)&BIT_MASK_STPM_PRIM_CURR_REACT_POW);
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG7)&BIT_MASK_STPM_SEC_CURR_REACT_POW);
			}
		}
		break;
		case (APPARENT_RMS):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG8)&BIT_MASK_STPM_PRIM_CURR_RMS_POW);
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG8)&BIT_MASK_STPM_SEC_CURR_RMS_POW);
			}
		}
		break;
		case (APPARENT_VEC):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG9)&BIT_MASK_STPM_PRIM_CURR_VEC_POW);
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG9)&BIT_MASK_STPM_SEC_CURR_VEC_POW);
			}
		}
		break;
		case (MOM_WIDE_ACT):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG10)&BIT_MASK_STPM_PRIM_CURR_MOM_ACTIVE_POW);
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG10)&BIT_MASK_STPM_SEC_CURR_MOM_ACTIVE_POW);
			}
		}
		break;
		case (MOM_FUND_ACT):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG11)&BIT_MASK_STPM_PRIM_CURR_MOM_FUND_POW);
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_power = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG11)&BIT_MASK_STPM_SEC_CURR_MOM_FUND_POW);
			}
		}
		break;
		
	} /* end switch */

	raw_power <<= 4;  // handle sign extension as power is on 28 bits
	raw_power >>= 4;

	return raw_power;
}

/**
  * @brief      This function read the Energy of signal come from one Channel mapped in one device
  * @param[in]   in_Metro_Device_Id (device ID), HOST or EXT1 to EXT4
  * @param[in]   in_Metro_int_Channel (Channel ID), Channel 1 or 2 ( according to device )
  * @param[in]   in_Metro_Energy_Selection : E_W_ACTIVE , E_F_ACTIVE, E_REACTIVE, E_APPARENT
  * @param[out]  None
  * @retval     int32_t raw_nrj according to Energy type  read inside device register
  */

int64_t Metro_HAL_read_energy(METRO_NB_Device_t in_Metro_Device_Id,METRO_internal_Channel_t in_Metro_int_Channel,METRO_Energy_selection_t in_Metro_Energy_Selection) {
  
	int32_t raw_nrj = 0;

	switch (in_Metro_Energy_Selection) {
	
		case (E_W_ACTIVE):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_nrj = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG1));
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_nrj = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG1));
			}
		}
		break;
		case (E_F_ACTIVE):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_nrj = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG2));
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_nrj = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG2));
			}
		}
		break;
		case (E_REACTIVE):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_nrj = -((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG3));
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_nrj = -((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG3));
			}
		}
		break;
		case (E_APPARENT):
		{
			if (in_Metro_int_Channel == INT_CHANNEL_1) {
				/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
				raw_nrj = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH1_REG4));
			}
			else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
				/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
				raw_nrj = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.CH2_REG4));
			}
		}
		break;
	} /* end switch */

	return raw_nrj;
}

/**
  * @brief      This function read the momentary Cuurent from one device and one channel 
  * @param[in]   in_Metro_Device_Id (device ID), EXT1 to EXT4
  * @param[in]   in_Metro_int_Channel (Channel ID), Channel 1, 2 of device
  * @param[out]  None
  * @retval     uint32_t raw RMS voltage
  */
  
uint32_t Metro_HAL_read_RMS_Voltage(METRO_NB_Device_t in_Metro_Device_Id,METRO_internal_Channel_t in_Metro_int_Channel) {
  
	uint32_t RMS_voltage = 0;

	if (in_Metro_int_Channel == INT_CHANNEL_1) {
		/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
		RMS_voltage = (p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSP_REG14)&BIT_MASK_STPM_DATA_VRMS;
	}
	else if (in_Metro_int_Channel == INT_CHANNEL_2) {/* is channel 2 */
		/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
		RMS_voltage = (p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSP_REG15)&BIT_MASK_STPM_DATA_VRMS;
	}

	return (RMS_voltage);
}

/**
  * @brief      This function read the momentary Cuurent from one device and one channel 
  * @param[in]   in_Metro_Device_Id (device ID), EXT1 to EXT4
  * @param[in]   in_Metro_int_Channel (Channel ID), Channel 1, 2 of device
  * @param[out]  None
  * @retval     uint32_t raw RMS current
  */
  
uint32_t Metro_HAL_read_RMS_Current(METRO_NB_Device_t in_Metro_Device_Id,METRO_internal_Channel_t in_Metro_int_Channel) {
	
	uint32_t RMS_current = 0;

	if (in_Metro_int_Channel == INT_CHANNEL_1) {
		/* get directly from RAM, be carrefull : latch should be made before to have coherents values inside DSP data reg */
		RMS_current = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSP_REG14)&BIT_MASK_STPM_DATA_C1_RMS)>>BIT_MASK_STPM_DATA_C_RMS_SHIFT;
	}
	else if (in_Metro_int_Channel == INT_CHANNEL_2) { /* is channel 2 */
		/* get directly from RAM, be carrefull !!!!! : latch should be made before to have coherents values inside DSP data reg */
		RMS_current = ((p_Metro_Device_Config[in_Metro_Device_Id].metro_stpm_reg.DSP_REG15)&BIT_MASK_STPM_DATA_C2_RMS)>>BIT_MASK_STPM_DATA_C_RMS_SHIFT;
	}

	return (RMS_current);
}

#ifdef UART_XFER_STPM3X /* UART MODE */ 
  
/**
  * @brief      Reverse byte
  * @param      one Byte
  * @retval     Byte reversed
  */

static uint8_t Metro_HAL_byteReverse(uint8_t in_byte) {
	
    in_byte = ((in_byte >> 1) & 0x55) | ((in_byte << 1) & 0xaa);
    in_byte = ((in_byte >> 2) & 0x33) | ((in_byte << 2) & 0xcc);
    in_byte = ((in_byte >> 4) & 0x0F) | ((in_byte << 4) & 0xF0);

    return in_byte;
}

#endif 

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
