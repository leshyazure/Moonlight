/*
 * moonlight.c
 *
 *  Created on: 1 sty 2020
 *      Author: HP
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/adc.h"

static const char *TAG = "moonlight";

int LEDRed = 0;
int LEDGreen = 1000;
int LEDBlue = 1000;
int LEDWhite = 0;
int duration = 5;
int fadein = 4;
int fadeout = 4;
int threshold = 1000;
int preview_time = 1;

int getR()
{
	return LEDRed;
}

int getG()
{
	return LEDGreen;
}

int getB()
{
	return LEDBlue;
}

int getW()
{
	return LEDWhite;
}
int getFadein()
{
	return fadein;
}
int getFadeout()
{
	return fadeout;
}
int getDuration()
{
	return duration;
}
int measureAmbientLight()
{
	return adc1_get_raw(ADC1_CHANNEL_7);
}


void fade_in(int red, int green, int blue, int white, int red_fade, int green_fade, int blue_fade, int white_fade)
{

	int red_ms = red_fade * 1000;
	int green_ms = green_fade * 1000;
	int blue_ms = blue_fade * 1000;
	int white_ms = white_fade * 1000;

	ESP_LOGI(TAG, "Fade in to R:%d G:%d B:%d W:%d, with times %d,%d,%d,%d ", red, green, blue, white, red_fade, green_fade, blue_fade, white_fade);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, red, red_ms);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, green, green_ms);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, blue, blue_ms);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, white, white_ms);

	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);

}

void fade_out(int red, int green, int blue, int white, int red_fade, int green_fade, int blue_fade, int white_fade)
{

	int red_ms = red_fade * 1000;
	int green_ms = green_fade * 1000;
	int blue_ms = blue_fade * 1000;
	int white_ms = white_fade * 1000;

	ESP_LOGI(TAG, "Fade out to R:%d G:%d B:%d W:%d, with times %d,%d,%d,%d ", red, green, blue, white, red_fade, green_fade, blue_fade, white_fade);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, red, red_ms);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, green, green_ms);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, blue, blue_ms);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, white, white_ms);

	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);

}

void preview_settings(int red, int green, int blue, int white, int time)
{

	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, red);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, green);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, blue);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, white);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);

	vTaskDelay((time * 1000) / portTICK_RATE_MS);

	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
}

void configure_moonlight(
		int red,
		int green,
		int blue,
		int white,
		int dur,
		int fin,
		int fout,
		int thresh,
		int prevtime) {

	LEDRed = red;
	LEDGreen = green;
	LEDBlue = blue;
	LEDWhite = white;
	duration = dur;
	fadein = fin;
	fadeout = fout;
	threshold = thresh;
	preview_time = prevtime;



}



