/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_partition.h"

#include "led_strip.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

static led_strip_handle_t led_strip;

//------------------------------------------------------------------------
void print_partition_table() 
{
    // Obtém um iterador para listar todas as partições
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, 
                                                     ESP_PARTITION_SUBTYPE_ANY, 
                                                     NULL);
    
    ESP_LOGI("PARTITION_INFO", "---------------- Partition Table ----------------");
    ESP_LOGI("PARTITION_INFO", "%-16s %-10s %-10s %-10s %-10s", "Name", "Type", "Sub", "Offset", "Size");

    while (it != NULL) 
    {
        const esp_partition_t *p = esp_partition_get(it);        
        // Imprime os detalhes da partição atual
        ESP_LOGI("PARTITION_INFO", "%-16s 0x%-8X 0x%-8X 0x%08lX   0x%08lX (%ld KB)",
                 p->label, 
                 p->type, 
                 p->subtype, 
                 p->address, 
                 p->size,
                 p->size / 1024);

        it = esp_partition_next(it);
    }
    
    ESP_LOGI("PARTITION_INFO", "-------------------------------------------------");
    
    // Libera o iterador
    esp_partition_iterator_release(it);
}
//------------------------------------------------------------------------
static void blink_led(uint8_t myColor)
{
    /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
    switch (myColor)
    {
        case 0:
            led_strip_set_pixel(led_strip, 0, 16, 16, 16);      // white
            ESP_LOGI(TAG, "Turning the LED - White");
        break;
        case 1:
            led_strip_set_pixel(led_strip, 0, 16, 0, 0);      // red
            ESP_LOGI(TAG, "Turning the LED - Red");
        break;
        case 2:
            led_strip_set_pixel(led_strip, 0, 0, 16, 0);      // green
            ESP_LOGI(TAG, "Turning the LED - Green");
        break;
        case 3:
            led_strip_set_pixel(led_strip, 0, 0, 0, 16);      // blue
            ESP_LOGI(TAG, "Turning the LED - Blue");
        break;
        case 4:
            led_strip_set_pixel(led_strip, 0, 0, 0, 0);      // OFF
            ESP_LOGI(TAG, "Turning the LED - OFF");
        break;
        default:
            led_strip_set_pixel(led_strip, 0, 0, 0, 0);      // OFF
            ESP_LOGI(TAG, "Turning the LED ----- OFF");
        break;
    }
    /* Refresh the strip to send data */
    led_strip_refresh(led_strip);
}
//------------------------------------------------------------------------
static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };

    //*  CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void app_main(void)
{
    print_partition_table();

    uint8_t x=100;
    configure_led();

    while (1) 
    {
        if (++x > 5) 
            x=0;
        blink_led(x);
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
