/*
 * sys_stm.h
 *
 *  Created on: 24 feb. 2024
 *      Author: JoseArratia
 */

#ifndef MAIN_SYS_STM_H_
#define MAIN_SYS_STM_H_
/*INCLUDES********************************************************************************************************/
#include "esp_err.h"

/*DEFINES*********************************************************************************************************/
//STM32
#ifndef STM_BUS
#define STM_BUS
#define STM_UART_PORT				2
#define STM_TX_PIN					22
#define STM_RX_PIN					23
#define STM_UART_BAUD				115200
#define STM_BUFF_SIZE				512

#define STM_RST_PIN					25
#define STM_BOOT0_PIN				26
#endif

#ifndef STM_CMD_SET
#define STM_CMD_SET

#define STM_START_CMD				0x7F
#define STM_GET						0x00
#define STM_GET_ID					0x02
#define STM_ERASE					0x43
#define STM_WRITE					0x31
#define STM_ACK						0x79
#define STM_NACK					0x1F
#endif

#define STM_MAX_DELAY				500/portTICK_PERIOD_MS

/*PROTOTYPES******************************************************************************************************/
void stm_init();

esp_err_t stm_check_version(char *version);
void stm_check_update();
esp_err_t stm_fupdate(char *filename);


#endif /* MAIN_SYS_STM_H_ */
