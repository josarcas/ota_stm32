/*INCLUDES****************************************************************************************/
#include "sys_wifi.h"
#include "sys_sntp.h"
#include "sys_stm.h"
#include "github_api.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_netif.h"

/*DEFINES****************************************************************************************/
#define STM_PART			"/D"
#define STM_FILE_NAME		STM_PART"/data.bin"

#define TIME_ZONE				-6*(3600)//GMT-6[seconds]

/*GLOBAL VARIABLES*******************************************************************************/
SemaphoreHandle_t stnp_sem = NULL;

/*PROTOTYPES*************************************************************************************/
static void sys_init();
static esp_err_t sys_fat_init();

void sntp_time_cb(struct timeval *tv);

/*FUNCTIONS**************************************************************************************/
static void sys_init()
{
	printf("Starting\n\r");
	//Init NVS
	ESP_ERROR_CHECK(nvs_flash_init());
//	ESP_ERROR_CHECK(esp_netif_init());
//	ESP_ERROR_CHECK(esp_event_loop_create_default());

	/*FAT*/
	sys_fat_init();

	/*STM32*/
	stm_init();

	/*Wi-Fi*/
	wifi_con("ae38ac", "273665629");
	wifi_wait_conn(portMAX_DELAY);

	/*SNTP*/
	stnp_sem = xSemaphoreCreateBinary();
	sys_sntp_init(sntp_time_cb);
	xSemaphoreTake(stnp_sem, portMAX_DELAY);
}

static esp_err_t sys_fat_init()
{
    esp_vfs_spiffs_conf_t fvs_config = {
        .base_path = STM_PART,
        .partition_label = NULL,
        .max_files = 2,
        .format_if_mount_failed = true
    };

    return esp_vfs_spiffs_register(&fvs_config);
}

void sntp_time_cb(struct timeval *tv)
{
	tv->tv_sec = tv->tv_sec+TIME_ZONE;
	settimeofday(tv, NULL);
	xSemaphoreGiveFromISR(stnp_sem, NULL);
}

void app_main(void)
{
	sys_init();

	if(git_save_content("josarcas", "blink", "Debug/blink_f103.bin", STM_FILE_NAME)!=ESP_OK)
		printf("Error to download bin\r\n");

	for(int8_t i=1; i>=0; i--)
	{
		printf("Update firmware in %d seconds...\n\r", i);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
	if(stm_fupdate(STM_FILE_NAME) == ESP_OK)
		printf("FIRMWARE UPDATE OK!!!\n\r");
	else
		printf("Error to update\n\r");

}
