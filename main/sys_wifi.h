/*
 * sys_wifi.h
 *
 *  Created on: 24 feb. 2024
 *      Author: JoseArratia
 */

#ifndef MAIN_SYS_WIFI_H_
#define MAIN_SYS_WIFI_H_
/*INCLUDES************************************************************************************/
#include "stdint.h"
#include "esp_err.h"


/*DEFINES**************************************************************************************/
#define MAX_TRY_RECONNECT			5			//Intentos de conexion

/*PROTOTYPES************************************************************************************/
void wifi_con(char *ssid, char *pass);
esp_err_t wifi_wait_conn(uint32_t time_delay);
esp_err_t wifi_stop();




#endif /* MAIN_SYS_WIFI_H_ */
