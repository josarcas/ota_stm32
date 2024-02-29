/*
 * sys_sntp.c
 *
 *  Created on: 24 feb. 2024
 *      Author: JoseArratia
 */
/*INCLUDES***********************************************************************************************/
#include "sys_sntp.h"

/*FUNCTIONS**********************************************************************************************/
void sys_sntp_init(espios_sync_cb sync_cb)
{
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, SNTP_SERVER);
    sntp_set_time_sync_notification_cb(sync_cb);
    esp_sntp_init();
}



