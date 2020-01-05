/*
 *
 *  	Created on: 29 dec 2019
 *      Author: Leshy Azure
 *      www.azurescens.eu
 */

#ifndef MOONLIGHT_H_
#define MOONLIGHT_H_

void fade_in();
void fade_out();
void previewColor();
int measureAmbientLight();

int* getLEDOn();
int* getLEDOff();
int getFadeIn();
int getFadeout();
int getDuration();
int getThreshold();

void setLEDOn(int r, int g, int b, int w);
void setLEDOff(int r, int g, int b, int w);

#endif /* MOONLIGHT_H_ */


