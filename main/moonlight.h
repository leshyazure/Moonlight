/*
 * moonlight.h
 *
 *  Created on: 29 gru 2019
 *      Author: Leshy Azure
 */

#ifndef MOONLIGHT_H_
#define MOONLIGHT_H_


int getR();
int getG();
int getB();
int getW();
int getFadein();
int getFadeout();
int getDuration();
int measureAmbientLight();

void fade_in(int red, int green, int blue, int white, int red_fade, int green_fade, int blue_fade, int white_fade);
void fade_out(int red, int green, int blue, int white, int red_fade, int green_fade, int blue_fade, int white_fade);
void configure_moonlight(
		int red,
		int green,
		int blue,
		int white,
		int dur,
		int fin,
		int fout,
		int thresh,
		int prevtime);
void preview_settings(int red, int green, int blue, int white, int time);

#endif /* MOONLIGHT_H_ */


