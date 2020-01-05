/*
 * moonlight.c
 *
 *  	Created on: 1 jan 2020
 *      Author: Leshy Azure
 *      www.azurescens.eu
 *
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
int getThreshold()
{
	return threshold;
}

bool getPreviewEnable()
{
	return enablePreview;
}

int getPreviewTime()
{
	return previewTime;
}

int* getLEDOn()
{
	static int led[4];
	led[0] = redOn;
	led[1] = greenOn;
	led[2] = blueOn;
	led[3] = whiteOn;
	return led;
}

int* getLEDOff()
{
	static int led[4];
	led[0] = redOff;
	led[1] = greenOff;
	led[2] = blueOff;
	led[3] = whiteOff;
	return led;
}

void setLEDOn(int r, int g, int b, int w)
{
	redOn = r;
	greenOn = g;
	blueOn = b;
	whiteOn = w;
}
void setLEDOff(int r, int g, int b, int w)
{
	redOff = r;
	greenOff = g;
	blueOff = b;
	whiteOff = w;
}
void setTiming(int fin, int fout, int dur)
{
	fadeIn = fin;
	fadeOut = fout;
	duration = dur;

}
void setPreview(bool en, int time)
{
	enablePreview = en;
	previewTime = time;
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
	ESP_LOGI(TAG, "Fade out to R:%d G:%d B:%d W:%d, with time %d ", redOff, greenOff, blueOff, whiteOff, fadeOut);
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
