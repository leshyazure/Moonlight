/*
	Moonlight v1.0
*/

#include "esp_event_loop.h"
#include "driver/sdmmc_host.h"
#include "driver/gpio.h"
#include "esp_vfs_semihost.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "sdmmc_cmd.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mdns.h"
#include "protocol_examples_common.h"
#include "sdkconfig.h"
#include "driver/ledc.h"
#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#define ESP_INTR_FLAG_DEFAULT 0
#define MDNS_INSTANCE "moonlight server"

static const char *TAG = "moonlight";

int LEDRed = 0;
int LEDGreen = 100;
int LEDBlue = 0;
int LEDWhite = 0;
int duration = 5;
int fadein = 2;
int fadeout = 2;



static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {

        	int fadein_in_s = fadein *1000;
        	int fadeout_in_s = fadeout * 1000;

        	gpio_set_level(GPIO_NUM_2, 1);
        	ESP_LOGI(TAG, "Motion detected, fade in.");

        	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, LEDRed, fadein_in_s);
        	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, LEDGreen, fadein_in_s);
        	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, LEDBlue, fadein_in_s);
        	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, LEDWhite, fadein_in_s);

        	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
        	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
        	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);


        	ESP_LOGI(TAG, "Wait %d seconds", duration);
            int duration_in_s = duration * 1000;
            vTaskDelay(duration_in_s / portTICK_RATE_MS);
            gpio_set_level(GPIO_NUM_2, 0);

            ESP_LOGI(TAG, "Fade out.");
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0, fadeout_in_s);
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0, fadeout_in_s);
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0, fadeout_in_s);
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0, fadeout_in_s);

            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);


        }
    }
}

esp_err_t start_rest_server(const char *base_path);

static void initialise_mdns(void)
{
    mdns_init();
    mdns_hostname_set(CONFIG_MDNS_HOST_NAME);
    mdns_instance_name_set(MDNS_INSTANCE);

    mdns_txt_item_t serviceTxtData[] = {
        {"board", "esp32"},
        {"path", "/"}
    };

    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
                                     sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));
}


#if CONFIG_WEB_DEPLOY_SD
esp_err_t init_fs(void)
{
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    gpio_set_pull_mode(15, GPIO_PULLUP_ONLY); // CMD
    gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);  // D0
    gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);  // D1
    gpio_set_pull_mode(12, GPIO_PULLUP_ONLY); // D2
    gpio_set_pull_mode(13, GPIO_PULLUP_ONLY); // D3

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 4,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(CONFIG_WEB_MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }
    /* print card info if mount successfully */
    sdmmc_card_print_info(stdout, card);
    return ESP_OK;
}
#endif

#if CONFIG_WEB_DEPLOY_SF
esp_err_t init_fs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = CONFIG_WEB_MOUNT_POINT,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}
#endif
void PWMinit() {

	ledc_timer_config_t ledc_timer = {
	        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
	        .freq_hz = 5000,                      // frequency of PWM signal
	        .speed_mode = LEDC_HIGH_SPEED_MODE,           // timer mode
	        .timer_num = LEDC_TIMER_0,            // timer index
	        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
	    };
	    // Set configuration of timer0 for high speed channels
	    ledc_timer_config(&ledc_timer);

	    ledc_channel_config_t ledc_channel[4] = {
	    		{

	               .channel    = LEDC_CHANNEL_0,
	               .duty       = 0,
	               .gpio_num   = 23,
	               .speed_mode = LEDC_HIGH_SPEED_MODE,
	               .hpoint     = 0,
	               .timer_sel  = LEDC_TIMER_0
	    		},
				{

				.channel    = LEDC_CHANNEL_1,
				.duty       = 0,
				.gpio_num   = 22,
				.speed_mode = LEDC_HIGH_SPEED_MODE,
				.hpoint     = 0,
				.timer_sel  = LEDC_TIMER_0
				},
	    		{

	    		.channel    = LEDC_CHANNEL_2,
	    		.duty       = 0,
	    		.gpio_num   = 21,
	    		.speed_mode = LEDC_HIGH_SPEED_MODE,
	    		.hpoint     = 0,
	    		.timer_sel  = LEDC_TIMER_0
	    			    		},
	    		{

	    		.channel    = LEDC_CHANNEL_3,
	    		.duty       = 0,
	    		.gpio_num   = 19,
	    		.speed_mode = LEDC_HIGH_SPEED_MODE,
	    		.hpoint     = 0,
	    		.timer_sel  = LEDC_TIMER_0
	    			    		}};

	       // Set LED Controller with previously prepared configuration

	           ledc_channel_config(&ledc_channel[0]);
	           ledc_channel_config(&ledc_channel[1]);
	           ledc_channel_config(&ledc_channel[2]);
	           ledc_channel_config(&ledc_channel[3]);
}


void app_main()
{


	PWMinit();

	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_2, 0);
	gpio_config_t io_conf;
    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    //bit mask of the pins, use GPIO5 here
    io_conf.pin_bit_mask = 1ULL<<5;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);


    // Initialize fade service.
    ledc_fade_func_install(0);

    //change gpio intrrupt type for one pin
      //  gpio_set_intr_type(5, GPIO_INTR_ANYEDGE);

        //create a queue to handle gpio event from isr
        gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
        //start gpio task
        xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

        //install gpio isr service
        gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
        //hook isr handler for specific gpio pin
        gpio_isr_handler_add(5, gpio_isr_handler, (void*) 5);


        //remove isr handler for gpio number.
        gpio_isr_handler_remove(5);
        //hook isr handler for specific gpio pin again
        gpio_isr_handler_add(5, gpio_isr_handler, (void*) 5);




    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initialise_mdns();

    ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK(init_fs());
    ESP_ERROR_CHECK(start_rest_server(CONFIG_WEB_MOUNT_POINT));



}
