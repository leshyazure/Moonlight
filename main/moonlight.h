/*
 * moonlight.h
 *
 *  Created on: 29 gru 2019
 *      Author: Leshy Azure
 */

#ifndef MOONLIGHT_H_
#define MOONLIGHT_H_

void previewColor();
void setLEDOn(int r, int g, int b, int w);

int getR();
int getG();
int getB();
int getW();
int getFadein();
int getFadeout();
int getDuration();
int measureAmbientLight();

void fade_in();
void fade_out();

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


#endif /* MOONLIGHT_H_ */


