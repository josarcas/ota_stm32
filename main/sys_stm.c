/*
 * sys_stm.c
 *
 *  Created on: 24 feb. 2024
 *      Author: JoseArratia
 */
/*INCLUDES*********************************************************************************************/
#include "sys_stm.h"

#include "driver/uart.h"
#include "driver/gpio.h"

/*GLOBAL VARIABLES*************************************************************************************/
static const uint16_t support_chips[] = {
		0x0410,
};

static const uint32_t sec_prog[] = {
		0x08000000, 0x08000100, 0x08000200, 0x08000300, 0x08000400, 0x08000500, 0x08000600, 0x08000700,
		0x08000800, 0x08000900, 0x08000A00, 0x08000B00, 0x08000C00, 0x08000D00, 0x08000E00, 0x08000F00,
		0x08001000, 0x08001100, 0x08001200, //For chip ID 0x0410
};

/*PROTOTYPES*******************************************************************************************/
static uint8_t stm_cmp_buff(uint8_t *buff, uint16_t size);
//static esp_err_t stm_tx_ack(uint8_t *buff, uint16_t size);
static esp_err_t stm_tx(uint8_t *buff, uint16_t size);
static int16_t stm_rx(uint8_t *buff, uint16_t size, uint32_t delay);
static void stm_flush();

static esp_err_t stm_cmp_chip(uint16_t id);

static esp_err_t stm_write_cmd(uint8_t cmd, uint8_t *rbuff);

/*FUNCTIONS********************************************************************************************/
static uint8_t stm_cmp_buff(uint8_t *buff, uint16_t size)
{
	uint8_t res;
	if(size == 1)
		res = 0xFF-buff[0];
	else
	{
		res = buff[0];
		for(uint16_t i=1; i<size; i++)
			res ^=buff[i];
	}

	return res;
}

//static esp_err_t stm_tx_u8(uint8_t buff)
//{
//	uart_write_bytes(STM_UART_PORT, &buff, 1);
//	if(uart_wait_tx_done(STM_UART_PORT, STM_MAX_DELAY)!= ESP_OK)
//		return ESP_FAIL;
//	return ESP_OK;
//}

//static esp_err_t stm_tx_ack(uint8_t *buff, uint16_t size)
//{
//	uint8_t rx;
//	uart_write_bytes(STM_UART_PORT, buff, size);
//	if(uart_wait_tx_done(STM_UART_PORT, STM_MAX_DELAY*size)!= ESP_OK)
//		return ESP_FAIL;
//
//	if(uart_read_bytes(STM_UART_PORT, &rx, 1, STM_MAX_DELAY)!=1 || rx !=STM_ACK)
//		return ESP_FAIL;
//	return ESP_OK;
//}

static esp_err_t stm_tx(uint8_t *buff, uint16_t size)
{
	if(size != 0)
		buff[size] = stm_cmp_buff(buff, size);
	size++;

	printf("Transmitido %d: ", size);
	for(uint16_t i=0; i<size; i++)
		printf("0x%.2X ", buff[i]);
	printf("\n\r");

	if(uart_write_bytes(STM_UART_PORT, buff, size) != size) return ESP_FAIL;
	if(uart_wait_tx_done(STM_UART_PORT, STM_MAX_DELAY*size)!= ESP_OK)
		return ESP_FAIL;
	return ESP_OK;
}

static int16_t stm_rx(uint8_t *buff, uint16_t size, uint32_t delay)
{
	int16_t len;
	len = uart_read_bytes(STM_UART_PORT, buff, size, delay);
	if(len > 0)
	{
		printf("Recibido: ");
		for(uint16_t i=0; i<len; i++)
			printf("0x%.2X ", buff[i]);
	}
	printf("\n\r");
	return len;
}

static void stm_flush()
{
	for(uint8_t i=0; i<10; i++)
	{
		uart_flush(STM_UART_PORT);
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}
static esp_err_t stm_cmp_chip(uint16_t id)
{
	for(uint16_t i=0; i<sizeof(support_chips)/2; i++)
	{
		if(id == support_chips[i])
			return ESP_OK;
	}
	return ESP_FAIL;
}

static esp_err_t stm_write_cmd(uint8_t cmd, uint8_t *rbuff)
{
#warning "This function modifies the data in rbuff in some cases"
	uint8_t *buff = NULL;
	esp_err_t status = ESP_FAIL;
	uint16_t len;

//	switch(cmd)
//	{
//	case STM_START_CMD:
//	{
//		buff = (char *)malloc(8);
//		buff[0] = STM_START_CMD;
//		if((status=stm_tx(buff, 1)) != ESP_OK) break;
//		if(stm_rx(buff, 1) != 1 || buff[0] != STM_ACK) status = ESP_FAIL;
//	}
//	break;
//
//	case STM_GET:
//	{
//		buff = (char *)malloc(16);
//		buff[0] = STM_GET;
//		if((status=stm_tx(buff, 1)) != ESP_OK) break;
//
//	}
//	break;
//
//	case STM_GET_ID:
//	{
//		buff = (char *)malloc(8);
//		buff[0] = STM_START_CMD;
//		if((status=stm_tx(buff, 1)) != ESP_OK) break;
//		if(stm_rx(buff, 5) == 5)
//		{
//			if(buff[0]==STM_ACK && buff[5]==STM_ACK && buff[1]==0x01)
//			{
//				rbuff[0] = buff[2];
//				rbuff[1] = buff[3];
//			}
//		}
//		else
//			status = ESP_FAIL;
//	}
//	break;
//
//	case STM_START_CMD:
//	{
//
//	}
//	break;
//
//	default:
//		status = ESP_FAIL;
//		break;
//	}

	do
	{
		if(cmd == STM_START_CMD)
		{
			buff = (uint8_t *)malloc(8);
			buff[0] = STM_START_CMD;
			if((status=stm_tx(buff, 0)) != ESP_OK) break;
			if(stm_rx(buff, 1, 500/portTICK_PERIOD_MS) != 1) status = ESP_FAIL;
			if(buff[0]!=STM_ACK && buff[0]!=STM_NACK) status = ESP_FAIL;
		}
		else if(cmd==STM_GET || cmd==STM_GET_ID)
		{
			buff = (uint8_t *)malloc(16);
			buff[0] = cmd;
			if((status=stm_tx(buff, 1)) != ESP_OK) break;
			if((len=stm_rx(buff, 16, 500/portTICK_PERIOD_MS)) > 1)
			{
				if(buff[0]==STM_ACK && (buff[1]+4)==len && buff[len-1]==STM_ACK)
				{
					for(uint16_t i=0; i<buff[1]+1; i++)
						rbuff[i] = buff[i+2];
				}
			}
			else
				status = ESP_FAIL;
		}
		else if(cmd == STM_ERASE)
		{
			buff = (uint8_t *)malloc(rbuff[0]+3);
			buff[0] = STM_ERASE;
			if((status=stm_tx(buff, 1)) != ESP_OK) break;
			if(stm_rx(buff, 1, 500/portTICK_PERIOD_MS)==1 && buff[0] == STM_ACK)
			{
				for(uint16_t i=0; i<rbuff[0]+2; i++)
					buff[i] = rbuff[i];
				if((status=stm_tx(buff, rbuff[0]+2)) != ESP_OK) break;
				if(stm_rx(buff, 1, 2000/portTICK_PERIOD_MS)!=1 || buff[0]!=STM_ACK) status = ESP_FAIL;
			}
			else
				status = ESP_FAIL;

		}
		else if(cmd == STM_WRITE)
		{
			buff = (uint8_t *)malloc(8);
			buff[0] = STM_WRITE;
			if((status=stm_tx(buff, 1)) != ESP_OK) break;
			if(stm_rx(buff, 1, 900/portTICK_PERIOD_MS)==1 && buff[0] == STM_ACK)
			{
				for(uint16_t i=0; i<4; i++)
					buff[i] = rbuff[i];
				if((status=stm_tx(buff, 4)) != ESP_OK) break;
				if(stm_rx(buff, 1, 900/portTICK_PERIOD_MS)!=1 || buff[0]!=STM_ACK) status = ESP_FAIL;
				else
					if((status=stm_tx(&rbuff[4], rbuff[4]+2)) != ESP_OK) break;
				if(stm_rx(buff, 1, 20000/portTICK_PERIOD_MS)!=1 || buff[0]!=STM_ACK) status = ESP_FAIL;
			}
			else
				status = ESP_FAIL;
		}
	}while(0);

	if(buff != NULL)
		free(buff);

	return status;
}

void stm_init()
{
	//Init uart
    uart_config_t stm_uart_config = {
        .baud_rate = STM_UART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 64,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(STM_UART_PORT, STM_BUFF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(STM_UART_PORT, &stm_uart_config));
    ESP_ERROR_CHECK(uart_set_pin(STM_UART_PORT, STM_TX_PIN, STM_RX_PIN, -1, -1));

    //GPIO INIT
    gpio_set_direction(STM_BOOT0_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(STM_BOOT0_PIN, 0);
    gpio_set_direction(STM_RST_PIN, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(STM_RST_PIN, 0);
    vTaskDelay(100/portTICK_PERIOD_MS);
    gpio_set_level(STM_RST_PIN, 1);


}

esp_err_t stm_check_version(char *version)
{
	return ESP_OK;
}

esp_err_t stm_fupdate(char *filename)
{
	FILE *data = NULL;
	uint8_t buff[STM_BUFF_SIZE];
	int16_t len;
	esp_err_t status= ESP_FAIL;

	//Reset STM32 and init on boot0 mode
	gpio_set_level(STM_RST_PIN, 0);
	vTaskDelay(100/portTICK_PERIOD_MS);
	gpio_set_level(STM_BOOT0_PIN, 1);
	vTaskDelay(100/portTICK_PERIOD_MS);
	gpio_set_level(STM_RST_PIN, 1);
	vTaskDelay(100/portTICK_PERIOD_MS);

	stm_flush();
	for(len =0; len<10; len++)
	{
		if(stm_write_cmd(STM_START_CMD, NULL) == ESP_OK)
			break;
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
	if(len == 10)
	{
		printf("Error to connect\n\r");
		return ESP_FAIL;
	}

	for(len =0; len<10; len++)
	{
		if(stm_write_cmd(STM_GET_ID, buff)==ESP_OK && stm_cmp_chip(buff[0]<<8|buff[1])==ESP_OK)
			break;
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
	if(len==10)
	{
		printf("Chip not support 0x%.4X\n\r", buff[0]<<8|buff[1]);
		return ESP_FAIL;
	}

	data = fopen(filename, "r+b");
	if(data == NULL)
		return ESP_FAIL;

	/*Erase pages**********************************/
	buff[0] = 0x05;	//Number of pages to be erase-1;

	//Pages to be erase
	buff[1] = 0x00;
	buff[2] = 0x01;
	buff[3] = 0x02;
	buff[4] = 0x03;
	buff[5] = 0x04;
	buff[6] = 0x60;
	if(stm_write_cmd(STM_ERASE, buff) != ESP_OK)
	{
		printf("Error to erase memory\n\r");
		return ESP_FAIL;
	}

	/*Write memory**********************************/
	do
	{
//		//Init adresss to be write
//		buff[0] = 0x08;
//		buff[1] = 0x00;
//		buff[2] = 0x00;
//		buff[3] = 0x00;
//		buff[4] = 0x08;
//
//		len = fread(&buff[6], 1, 256, data);
//		if(len >0)
//		{
//			buff[5] = len-1; //Number of bytes to be write-1
//			if((status=stm_write_cmd(STM_ERASE, buff)) != ESP_OK) break;
//		}
//		else break;
//
//		//2nd address to be write
//		buff[0] = 0x08;
//		buff[1] = 0x00;
//		buff[2] = 0x01;
//		buff[3] = 0x00;
//		buff[4] = 0x09;
//
//		len = fread(&buff[6], 1, 256, data);
//		if(len >0)
//		{
//			buff[5] = len-1; //Number of bytes to be write-1
//			if((status=stm_write_cmd(STM_ERASE, buff)) != ESP_OK) break;
//		}
//		else break;
//
//		//3th address to be write
//		buff[0] = 0x08;
//		buff[1] = 0x00;
//		buff[2] = 0x02;
//		buff[3] = 0x00;
//		buff[4] = 0x0A;
//
//		len = fread(&buff[6], 1, 256, data);
//		if(len >0)
//		{
//			buff[5] = len-1; //Number of bytes to be write-1
//			if((status=stm_write_cmd(STM_ERASE, buff)) != ESP_OK) break;
//		}
//		else break;

		//Adresss to be write
		for(uint16_t i=0; i<19; i++)
		{
			buff[0] = sec_prog[i]>>24;
			buff[1] = sec_prog[i]>>16;
			buff[2] = sec_prog[i]>>8;
			buff[3] = sec_prog[i];

			len = fread(&buff[5], 1, 256, data);
			if(len >0)
			{
				buff[4] = len-1; //Number of bytes to be write-1
				if((status=stm_write_cmd(STM_WRITE, buff)) != ESP_OK) break;
			}
			else break;
		}

	}while(0);

	fclose(data);
	vTaskDelay(500/portTICK_PERIOD_MS);

	gpio_set_level(STM_RST_PIN, 0);
	vTaskDelay(100/portTICK_PERIOD_MS);
	gpio_set_level(STM_BOOT0_PIN, 0);
	vTaskDelay(100/portTICK_PERIOD_MS);
	gpio_set_level(STM_RST_PIN, 1);
	vTaskDelay(100/portTICK_PERIOD_MS);

	return status;
}

