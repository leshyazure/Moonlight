# Moonlight 


Simple, cheap and easy to buid home LED ambient light system, activated by infrared motion detection. Based on ESP32, fully customizable  over WiFi, dedicated for RGB/RGBW LED stripes.
Project described on [www.azurescens.eu](http://www.azurescens.eu "Azurescens.eu")

## Features


* Fully customizable with any device equipped with internet browser (smartphone, PC, tablet etc) connected to home WiFI network. 
* Based on ESP32 DevKit1
* Power supply 12V
* Dedicated for RGB or RGBW LED stripes.
* PWM controlled.
* Activated by PIR motion sensor.
* Adjustable threshold level control.

## Configuration

 * Folder **frontend** contains server files. Modify them, or use your own. Remember, that whole folder can't exceed 2MB if you're using internal memory. Otherwise you should use external SD card as server host.
 * Connection configuration can be done under *idf.py menuconfig* or directly under *sdkconfig* in project tree on Eclipse. Look for ***Moonlight Connection Configuration***.
 * If your device has enabled mDNS protocol, then you can simply cast your Moonlight device using a host name stored in **Moonlight Configuration**. By default it looks like *http://moonlight*
 * Otherwise to obtain an adress of Moonlight device you need to run an *idf.py monitor* or look for this in router settings.
 Adress usually starts with **192.168.0.** or **192.168.1.** and for easier further use can be assigned to static in your router.
