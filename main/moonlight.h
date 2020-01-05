/*
 *
 *  	Created on: 29 dec 2019
 *      Author: Leshy Azure
 *      www.azurescens.eu
 */
#include <stdbool.h>

#ifndef MOONLIGHT_H_
#define MOONLIGHT_H_

void fade_in();
void fade_out();
void previewColor();
int measureAmbientLight();

int* getLEDOn();
int* getLEDOff();
int getFadeIn();
int getFadeOut();
int getDuration();
int getThreshold();
bool getPreviewEnable();
int getPreviewTime();

void setTiming(int fin, int fout, int dur);
void setLEDOn(int r, int g, int b, int w);
void setLEDOff(int r, int g, int b, int w);
void setPreview(bool en, int time);

#endif /* MOONLIGHT_H_ */


