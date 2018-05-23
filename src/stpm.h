/*!
 * @file stpm.h
 * @brief Header file for stpm3x device for energy sensing.
 *
 * @addtogroup STPM
 * @{
 */

#include <stdint.h>
 
/*===========================================================================*/
/* Structures & Type Def           .                                         */
/*===========================================================================*/

 /**
  * @brief METROLOGY hardware Device type
  *
  */

typedef struct
{
	/* RW reg */
	uint32_t DSPCTRL1;                       /*!< DSP Control Register 1,	                        Address offset: 0x000 */
	uint32_t DSPCTRL2;                       /*!< DSP Control Register 2,	                        Address offset: 0x004 */
	uint32_t DSPCTRL3;                       /*!< DSP Control Register 3,	                        Address offset: 0x008 */
	uint32_t DSPCTRL4;                       /*!< DSP Control Register 4,	                        Address offset: 0x00C */
	uint32_t DSPCTRL5;                       /*!< DSP Control Register 5,	                        Address offset: 0x010 */
	uint32_t DSPCTRL6;                       /*!< DSP Control Register 6,	                        Address offset: 0x014 */
	uint32_t DSPCTRL7;                       /*!< DSP Control Register 7,	                        Address offset: 0x018 */
	uint32_t DSPCTRL8;                       /*!< DSP Control Register 8,	                        Address offset: 0x01C */
	uint32_t DSPCTRL9;                       /*!< DSP Control Register 9,	                        Address offset: 0x020 */
	uint32_t DSPCTRL10;                      /*!< DSP Control Register 10,	                        Address offset: 0x024 */
	uint32_t DSPCTRL11;                      /*!< DSP Control Register 11,	                        Address offset: 0x028 */
	uint32_t DSPCTRL12;                      /*!< DSP Control Register 12,	                        Address offset: 0x02C */
	uint32_t DFECTRL1;                       /*!< DFE Control Register 1,	                        Address offset: 0x030 */
	uint32_t DFECTRL2;                       /*!< DFE Control Register 2,	                        Address offset: 0x034 */
	uint32_t DSPIRQ1;                        /*!< DSP Interrupt Register 1,	                        Address offset: 0x038 */
	uint32_t DSPIRQ2;                        /*!< DSP Interrupt Register 2,	                        Address offset: 0x03C */
	uint32_t DSPSR1;                         /*!< DSP Status Register 1,	                        Address offset: 0x040 */
	uint32_t DSPSR2;                         /*!< DSP Status Register 2,	                        Address offset: 0x044 */
	uint32_t UARTSPICR1;                     /*!< UART/SPI Control Register 1,	                    Address offset: 0x048 */
	uint32_t UARTSPICR2;                     /*!< UART/SPI Control Register 2,	                	Address offset: 0x04C */
	uint32_t UARTSPISR;                      /*!< UART/SPI Status Register ,	                    Address offset: 0x050 */

	/* RO regs */ 
	uint32_t DSPEVENT1;                      /*!< DSP Events Register 1,	                        Address offset: 0x054 */ 
	uint32_t DSPEVENT2;                      /*!< DSP Events Register 2,	                        Address offset: 0x058 */
	uint32_t DSP_REG1;                       /*!< DSP Register 1,                                   Address offset: 0x05C */
	uint32_t DSP_REG2;                       /*!< DSP Register 2,                                   Address offset: 0x060 */
	uint32_t DSP_REG3;                       /*!< DSP Register 3,                                   Address offset: 0x064 */
	uint32_t DSP_REG4;                       /*!< DSP Register 4,                                   Address offset: 0x068 */
	uint32_t DSP_REG5;                       /*!< DSP Register 5,                                   Address offset: 0x06C */
	uint32_t DSP_REG6;                       /*!< DSP Register 6,                                   Address offset: 0x070 */
	uint32_t DSP_REG7;                       /*!< DSP Register 7,                                   Address offset: 0x074 */
	uint32_t DSP_REG8;                       /*!< DSP Register 8,                                   Address offset: 0x078 */
	uint32_t DSP_REG9;                       /*!< DSP Register 9,                                   Address offset: 0x07C*/
	uint32_t DSP_REG10;                      /*!< DSP Register 10,                                  Address offset: 0x080*/
	uint32_t DSP_REG11;                      /*!< DSP Register 11,                                  Address offset: 0x084*/
	uint32_t DSP_REG12;                      /*!< DSP Register 12,                                  Address offset: 0x088*/
	uint32_t DSP_REG13;                      /*!< DSP Register 13,                                  Address offset: 0x08C*/
	uint32_t DSP_REG14;                      /*!< DSP Register 14,                                  Address offset: 0x090*/
	uint32_t DSP_REG15;                      /*!< DSP Register 15,                                  Address offset: 0x094*/
	uint32_t DSP_REG16;                      /*!< DSP Register 16,                                  Address offset: 0x098 */
	uint32_t DSP_REG17;                      /*!< DSP Register 17,                                  Address offset: 0x09C */
	uint32_t DSP_REG18;                      /*!< DSP Register 18,                                  Address offset: 0x0A0*/
	uint32_t DSP_REG19;                      /*!< DSP Register 19,                                  Address offset: 0x0A4*/
	uint32_t CH1_REG1;                       /*!< Channel 1 Register 1,                             Address offset: 0x0A8*/
	uint32_t CH1_REG2;                       /*!< Channel 1 Register 2,                             Address offset: 0x0AC*/
	uint32_t CH1_REG3;                       /*!< Channel 1 Register 3,                             Address offset: 0x0B0 */
	uint32_t CH1_REG4;                       /*!< Channel 1 Register 4,                             Address offset: 0x0B4*/
	uint32_t CH1_REG5;                       /*!< Channel 1 Register 5,                             Address offset: 0x0B8*/
	uint32_t CH1_REG6;                       /*!< Channel 1 Register 6,                             Address offset:  */
	uint32_t CH1_REG7;                       /*!< Channel 1 Register 7,                             Address offset:  */
	uint32_t CH1_REG8;                       /*!< Channel 1 Register 8,                             Address offset:  */
	uint32_t CH1_REG9;                       /*!< Channel 1 Register 9,                             Address offset:  */
	uint32_t CH1_REG10;                      /*!< Channel 1 Register 10,                            Address offset:  */
	uint32_t CH1_REG11;                      /*!< Channel 1 Register 11,                            Address offset:  */
	uint32_t CH1_REG12;                      /*!< Channel 1 Register 12,                            Address offset:  */
	uint32_t CH2_REG1;                       /*!< Channel 2 Register 1,                             Address offset:  */
	uint32_t CH2_REG2;                       /*!< Channel 2 Register 2,                             Address offset:  */
	uint32_t CH2_REG3;                       /*!< Channel 2 Register 3,                             Address offset:  */
	uint32_t CH2_REG4;                       /*!< Channel 2 Register 4,                             Address offset:  */
	uint32_t CH2_REG5;                       /*!< Channel 2 Register 5,                             Address offset:  */
	uint32_t CH2_REG6;                       /*!< Channel 2 Register 6,                             Address offset:  */
	uint32_t CH2_REG7;                       /*!< Channel 2 Register 7,                             Address offset:  */
	uint32_t CH2_REG8;                       /*!< Channel 2 Register 8,                             Address offset:  */
	uint32_t CH2_REG9;                       /*!< Channel 2 Register 9,                             Address offset:  */
	uint32_t CH2_REG10;                      /*!< Channel 2 Register 10,                            Address offset:  */
	uint32_t CH2_REG11;                      /*!< Channel 2 Register 11,                            Address offset:  */
	uint32_t CH2_REG12;                      /*!< Channel 2 Register 12,                            Address offset:  */
	uint32_t TOT_REG1;                       /*!< Total  Register 1,                                Address offset:  */
	uint32_t TOT_REG2;                       /*!< Total  Register 2,                                Address offset:  */
	uint32_t TOT_REG3;                       /*!< Total  Register 3,                                Address offset:  */
	uint32_t TOT_REG4;                       /*!< Total  Register 4,                                Address offset:  */
	
} STPM_REG_TypeDef;

/*===========================================================================*/
/* General functions                                                         */
/*===========================================================================*/

void initAllRegisters(int * registersAddress);
void queryAllRegisters(int * registersAddress);
