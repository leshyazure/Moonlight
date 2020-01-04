/*
 *
 *	 Moonlight Setup
 *
 *	Default values & pin configuration
 *
 *   Created on: 4 sty 2020
 *      Author: Leshy Azure
 *      www.azurescens.eu
 *
 */

#ifndef MAIN_CONFIG_H_
#define MAIN_CONFIG_H_

// Photoresistor pin configuration
#define PHOTORESISTOR ADC1_CHANNEL_7 // ADC channel 7 (GPIO 35)
// LED pin configuration
#define LED_RED 23
#define LED_GREEN 22
#define LED_BLUE 21
#define LED_WHITE 19
// PIR pin configuration
#define PIR_PIN 5

// Default light on values
#define RED_ON 	0
#define GREEN_ON 300
#define BLUE_ON 3000
#define WHITE_ON 0

// Default light off values
#define RED_OFF 0
#define GREEN_OFF 0
#define BLUE_OFF 0
#define WHITE_OFF 0

// Timing values
#define DURATION 10
#define COMMON_FADE_IN 5
#define COMMON_FADE_OUT 5

// Threshold
#define THRESHOLD 1000

// Preview
#define ENABLE_PREVIEW true
#define PREVIEW_TIME 1



#endif /* MAIN_CONFIG_H_ */
