/*demo proejct with oled display bu Ole Schultz 28 aug. 2020*/
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <string.h>
#include <driver/uart.h>
#include <driver/i2c.h>
#include <esp_err.h>
#include "ssd1306.h"
#include <bmp180.h>
#include "esp_heap_caps.h"
#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define SCL_GPIO          17 //2                /*!< gpio number for I2C master clock */
#define SDA_GPIO          16 //14               /*!< gpio number for I2C master data  */



#define DEFAULT_FONT FONT_FACE_BITOCRA_6X11//FONT_FACE_BITOCRA_4X7//FONT_FACE_BITOCRA_7X13//FONT_FACE_BITOCRA_4X7

#define LOAD_ICON_X 54
#define LOAD_ICON_Y 42
#define LOAD_ICON_SIZE 20

#define CIRCLE_COUNT_ICON_X 100
#define CIRCLE_COUNT_ICON_Y 52


#define SECOND (1000 / portTICK_PERIOD_MS)
SemaphoreHandle_t print_mux = NULL;
char tasklistbuf[20]={0};

char tempe[30];
char pres[30];
 
void bmp180_test(void *pvParameters)
{
    bmp180_dev_t dev;//i2c_cmd_link_create();
    memset(&dev, 0, sizeof(bmp180_dev_t)); // Zero descriptor

    ESP_ERROR_CHECK(bmp180_init_desc(&dev, 0, SDA_GPIO, SCL_GPIO));
    ESP_ERROR_CHECK(bmp180_init(&dev));

    while (1)
    {
        float temp;
        uint32_t pressure;
  xSemaphoreTake(print_mux, portMAX_DELAY);
        esp_err_t res = bmp180_measure(&dev, &temp, &pressure, BMP180_MODE_STANDARD);
        if (res != ESP_OK)
            printf("Could not measure: %d\n", res);
        else {
            /* float is used in printf(). you need non-default configuration in
             * sdkconfig for ESP8266, which is enabled by default for this
             * example. see sdkconfig.defaults.esp8266
             */
            printf("Temperature: %.2f degrees Celsius; Pressure: %d Pa\n", temp, pressure);
            sprintf(tempe,"Temp.: %.2f oC\n ", temp);
            sprintf(pres,"Pres: %d Pa\n", pressure);

        }
  xSemaphoreGive(print_mux);
        vTaskDelay(pdMS_TO_TICKS(500));
      
    }
}
static void ssd1306_task(void *pvParameters)
{
unsigned int count=0;
char arr[10];
  // init i2s
 //xSemaphoreTake(print_mux, portMAX_DELAY);
 //i2c_example_master_init();
 vTaskDelay(1000/portTICK_PERIOD_MS);
 InitializeDisplay();
reset_display();
 
//printBigTime("102");
  while (1)
    {
      xSemaphoreTake(print_mux, portMAX_DELAY);
  

sprintf(arr,"%u",count);
setXY(0,0);
sendStr("vejrstation");
 sendStrXY(tempe,1,0);
sendStrXY(pres,2,0);

    bool fwd = false;
    count++;
    printf("%s: Started user interface task %d\n", __FUNCTION__,count);
   xSemaphoreGive(print_mux);
    vTaskDelay(SECOND);
    }
vTaskDelete(NULL);
 
}
void heap_caps_alloc_failed_hook(size_t requested_size, uint32_t caps, const char *function_name)
{
  printf("%s was called but failed to allocate %d bytes with 0x%X capabilities. \n",function_name, requested_size, caps);
}

void app_main()
{

printf("init");

print_mux = xSemaphoreCreateMutex();
 esp_err_t error = heap_caps_register_failed_alloc_callback(heap_caps_alloc_failed_hook);
     printf("%u",error);
   // void *ptr = heap_caps_malloc(allocation_size, MALLOC_CAP_DEFAULT);
  ESP_ERROR_CHECK(i2cdev_init());
    //
ESP_ERROR_CHECK(i2c_example_master_init());  //i2cdev_init()
//InitializeDisplay();
printf("done\n");

    printf("done init display\n");
  
//InitializeDisplay();  
    vTaskDelay(SECOND);
  xTaskCreate(ssd1306_task, "ssd1306_task", 2048, NULL, 10, NULL);

    //configMINIMAL_STACK_SIZE * 15  APP_CPU_NUM
 xTaskCreatePinnedToCore(bmp180_test, "bmp180_test", 2048, NULL, 10, NULL,APP_CPU_NUM );
    // xTaskCreate(bmp180_test, "bmp180_test", 2048, NULL, 10, NULL);
    //font_timer_handle = xTimerCreate("font_timer", 5 * SECOND, pdTRUE, NULL, font_timer);
   // xTimerStart(font_timer_handle, 0);
 
}

