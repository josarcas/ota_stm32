/*
 * sys_sntp.h
 *
 *  Created on: 24 feb. 2024
 *      Author: JoseArratia
 */

#ifndef MAIN_SYS_SNTP_H_
#define MAIN_SYS_SNTP_H_
/*INCLUDES************************************************************************************/
#include <time.h>

#include <sys/time.h>
#include "esp_sntp.h"
#include "esp_attr.h"

/*DEFINES*************************************************************************************/
//SNTP DEFINES
#define SNTP_SERVER				"pool.ntp.org"
#define SNTP_SIZE_QUEUE			2
#define SNTP_SIZE_MSG			sizeof(time_t)

#define SEC_TO_HOUR				3600
#define TIME_ZONE				-6*(3600)//GMT-6[seconds]

/*TYPEDEFS************************************************************************************/
typedef void (*espios_sync_cb)(struct timeval *tv);

/*PROTOTYPES**********************************************************************************/
void sys_sntp_init(espios_sync_cb sync_cb);




#endif /* MAIN_SYS_SNTP_H_ */
