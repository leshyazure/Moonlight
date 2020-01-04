/*
 * moonlight.c
 *
 *  Created on: 1 sty 2020
 *      Author: HP
 */
#include <config.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/adc.h"

static const char *TAG = "moonlight";

int redOn = RED_ON;
int greenOn = GREEN_ON;
int blueOn = BLUE_ON;
int whiteOn = WHITE_ON;

int redOff = RED_OFF;
int greenOff = GREEN_OFF;
int blueOff = BLUE_OFF;
int whiteOff = WHITE_OFF;

int duration = DURATION;
int fadeIn = COMMON_FADE_IN;
int fadeOut = COMMON_FADE_OUT;

int threshold = THRESHOLD;

bool enablePreview = ENABLE_PREVIEW;
int previewTime = PREVIEW_TIME;


int getRon()
{
	return redOn;
}

int getGon()
{
	return greenOn;
}

int getBon()
{
	return blueOn;
}

int getWon()
{
	return whiteOn;
}
int getFadeIn()
{
	return fadeIn;
}
int getFadeOut()
{
	return fadeOut;
}
int getDuration()
{
	return duration;
}

void setLEDOn(int r, int g, int b, int w)
{
	redOn = r;
	greenOn = g;
	blueOn = b;
	whiteOn = w;
}

int measureAmbientLight()
{
	return adc1_get_raw(PHOTORESISTOR);
}


void fade_in()
{
	/*
	int red_ms = red_fade * 1000;
	int green_ms = green_fade * 1000;
	int blue_ms = blue_fade * 1000;
	int white_ms = white_fade * 1000;
*/


	ESP_LOGI(TAG, "Fade in to R:%d G:%d B:%d W:%d, with time %d", redOn, greenOn, blueOn, whiteOn, fadeIn);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, redOn, (fadeIn * 1000));
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, greenOn, (fadeIn * 1000));
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, blueOn, (fadeIn * 1000));
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, whiteOn, (fadeIn * 1000));

	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);

}

void fade_out()
{
	/*
	int red_ms = red_fade * 1000;
	int green_ms = green_fade * 1000;
	int blue_ms = blue_fade * 1000;
	int white_ms = white_fade * 1000;
*/
	ESP_LOGI(TAG, "Fade out to R:%d G:%d B:%d W:%d, with time %d ", redOff, greenOn, blueOn, whiteOn, fadeOut);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, redOff, (fadeOut * 1000));
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, greenOff, (fadeOut * 1000));
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, blueOff, (fadeOut * 1000));
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, whiteOff, (fadeOut * 1000));

	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
	ledc_fade_start(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);

}

void previewColor()
{
	if(enablePreview) {
		ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, redOn);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
		ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, greenOn);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
		ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, blueOn);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
		ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, whiteOn);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);

		vTaskDelay((previewTime * 1000) / portTICK_RATE_MS);

		ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, redOff);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
		ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, greenOff);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
		ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, blueOff);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
		ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, whiteOff);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
	}
}
/*
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
*/


