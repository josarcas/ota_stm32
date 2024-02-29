/*
 * sys_wifi.c
 *
 *  Created on: 24 feb. 2024
 *      Author: JoseArratia
 */
/*INCLUDES************************************************************************************/
#include "sys_wifi.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_wifi.h"
#include "esp_mac.h"

#include "esp_tls.h"
#include "esp_http_client.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/*GLOBAL VARIABLES*****************************************************************************/
static uint16_t retry_count = 0;
static SemaphoreHandle_t wifi_con_sem = NULL;

/*PROTOTYPES***********************************************************************************/
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

/*FUNCTIONS***********************************************************************************/
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        esp_wifi_connect();
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (retry_count < MAX_TRY_RECONNECT)
        {
            esp_wifi_connect();
            retry_count++;
            printf("Retry to connect to the AP %d", retry_count);
        }
        else
        {
        	printf("Can not to connect to wifi station");
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
    	xSemaphoreGiveFromISR(wifi_con_sem, NULL);
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("Connected to wifi station with ip:" IPSTR, IP2STR(&event->ip_info.ip));
        retry_count = 0;
    }
}


void wifi_con(char *ssid, char *pass)
{
	wifi_con_sem = xSemaphoreCreateBinary();

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler,
    		NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler,
            NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };

    strcpy((char *)wifi_config.sta.ssid, ssid);
    strcpy((char *)wifi_config.sta.password, pass);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

esp_err_t wifi_wait_conn(uint32_t time_delay)
{
	if(xSemaphoreTake(wifi_con_sem, time_delay) == pdTRUE)
		return ESP_OK;

	return ESP_FAIL;
}

esp_err_t wifi_stop()
{
	return esp_wifi_stop();
}






