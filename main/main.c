/* Uart Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "soc/uart_struct.h"



/**
 * This example shows how to use the UART driver to handle special UART events.
 *
 * It also reads data from UART0 directly, and echoes it to console.
 *
 * - port: UART0
 * - rx buffer: on
 * - tx buffer: on
 * - flow control: off
 * - event queue: on
 * - pin assignment: txd(default), rxd(default)
 */

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (1024)
static QueueHandle_t uart0_queue;

static void uart_event_task(void *pvParameters)
{
	uart_event_t event;
	while(1)
	{
		//Waiting for UART event.
		//if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY))
		{
			uint8_t* data = (uint8_t*) malloc(BUF_SIZE);
			int len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 100 / portTICK_RATE_MS);
			if(len > 0)
			{
				uart_write_bytes(EX_UART_NUM, (const char*)data, len);
			}
		}
	}
}

/* Configure the UART events example */
void app_main()
{
	/* Configure uart parameter */
	uart_config_t uart_config =
	{
			.baud_rate = 115200,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.rx_flow_ctrl_thresh = 122,
	};
	//Setup UART
	uart_param_config(EX_UART_NUM, &uart_config);

	//Install UART driver, and get the queue.
	uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 10, &uart0_queue, 0);

	//Set UART pins (using UART0 default pins ie no changes.)
	uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);


	//Create a task to handler UART event from ISR
	xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
}
