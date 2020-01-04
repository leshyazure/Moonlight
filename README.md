# Moonlight 


Simple, cheap and easy to buid home LED ambient light system, activated by infrared motion detection. Availably as shield for ESP32, fully customizable over WiFi, dedicated for RGB/RGBW LED stripes.
Project described on [www.azurescens.eu](http://www.azurescens.eu "Azurescens.eu")

## Features


 * Fully customizable with any device equipped with internet browser (smartphone, PC, tablet etc) connected to home WiFI network. 
 * Based on ESP32 DevKit1 and FreeRTOS operating system.
 * Power supply 12V
 * Dedicated for RGB or RGBW LED stripes.
 * Brightness of each color is digitally controlled with PWM (Pulse Width Modulation).
 * Activated by PIR motion sensor.
 * Adjustable threshold of ambient light level control made by photoresistor assigned to internal analog to digital converter.

## Configuration

 * Folder **frontend** contains server files. Modify them, or use your own. Keep in mind that whole folder can't exceed 2MB if you're using internal memory. You can slightly increase this limit in partition table if sum of server partition size and program size doesn't exceed 4MB . Otherwise you should use external SD card as server host.
 * Connection configuration can be done under *idf.py menuconfig* or directly under *sdkconfig* in project tree on Eclipse. Look for ***Moonlight Connection Configuration***.
 * If your device has enabled mDNS protocol (e.g Win10 need to install external service like [Bonjour](https://support.apple.com/downloads/DL999/en_US/BonjourPSSetup.exe)), then you can simply cast your Moonlight device using a host name stored in **Moonlight Configuration** with additional **.local** domain name. By default it looks like **http://moonlight.local**
 * Otherwise to obtain an adress of Moonlight device you need to run an *idf.py monitor* or look for *espressif* device in router settings.
 Adress usually starts with **192.168.0.** or **192.168.1.** and for easier further use can be assigned to static in your router.
 * Range of avalaible values of each color depends on PWM duty resolution (By default is set to **13 bit**) and PWM frequency (Default **5kHz**) and in this case is equal to **(2^13)-1 = 8191**. Can be increased up to 20 bit (1048575 values) or even 24 bit (not tested) when you decrease frequency to **80000000 / 2^desiredBitResolution** (in Hz).
 * Remember that the human eye logarithmically perceives the intensity of colors, and this controller does not currently have gamma correction, so it changes the brightness linearly. What does it mean ? That a change in any color is less noticeable in the higher range of values and much more in the lower range.

## Wiring

  * Schematic diagram of controller:
  ![Schematic diagram](https://github.com/leshyazure/Moonlight/blob/master/scheme/schematic_diagram.png "Schematic diagram")