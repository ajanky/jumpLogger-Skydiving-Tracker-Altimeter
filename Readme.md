![jumpLogger rendering](/pics/JL1Render.jpg)
# jumpLogger 1.0
This is a next generation skydive jump logger: a combined digital altimeter, GPS tracker, motion sensor and a device that allows detailed afterflight/jump analysis.

Originally design for CRW/CF skydivers, all features are also usable for other disciplines. Especially wingsuiters can use the exact altitude and speed information to analyze flight performance.

## disclaimer
**Please be aware that you always should wear an additional trusted commercial altimeter – for your own safety!**\
**As it it open source, no one can guarantee perfect functionality under all circumstances.** \
**The display holds a lot of detailed information – nothing you can glance in the split-second you have, to make important decisions.**

## Hardware

The unit is based on the versatile ESP32 Wifi processor. This gives not only enough processing power to handle complex math transformation (more on that later) but also to offer a state-of-the-art elegant web dashboard on your mobile phone, pad or desktop
- ESP32 WROOM dual kernel 32bit processor
- GPS data is recorded with the BN220 GPS module using the ublox M8 chip generation
- Pressure – and accordingly altitude – is measured with the BOSCH BMP388 sensor
- The IMU ADXL345 provides acceleration data
- A microSD card is added to store all tracking data
- For displaying data, a 128x64 monochromic transflective LCD is used
- Powered by a 1100mAh LiPO battery plus charger, giving ca 10hrs continuous operating time

![hardware 1.0](/pics/2sides0.jpg)

## Software
Software is build with VSCode and the PlatformIO environment using the Arduino framework.

### Modes
Currently the jumpLogger has four modes: 
- **Altimeter and Logger** writing **GPX** files that can downloaded via WiFi and displayed in a Webbrowser
- **Altimeter and Logger** writing **CSV** files that can downloaded via WiFi. CSV is much more compact and supports higher data rates (5points/s) with more information included. CSV files are **Flysight** compatible
- **DATA mode** showing most of the sensor values, writing CSV files.
- **WiFi mode** enables the internal web server and the managing dashboard in a. Browser - responsive to use on phones, pads or desktops .

The software currently is in experimental status. Filter coefficients needs to be adjusted to get perfect smooth values. Complex math operations will also be implemented later.

## features already working

Many already implemented features are driven by canopy formation skydivers: 

- a navigation circle is shown on the display, giving the *relative bearing* back to the dropzone. Hold the unit into flight direction and see the direction and distance to go back home.
- current ground speed and glide-ratio(GND) is shown
- when logging, a double-tap to the side of the unit flags a trackpoint to mark a geographical position: this helps to find things you lost underway - shoes, helmets, freebags...

## things to do

Based on the existing hardware, jumpLogger can develop to a community driven next-gen flight computer.
- it is planned to set up a Firebase application to use jumpLogger as a general skydiving logbook and flight analysis tool
- for wingsuiters, the modern sensor setup can improve the quality of sampled flight data. Using barometric altitude (instead just GPS altitude) and acceleration data gives more detailed insight. The internal BlueTooth connection can be used to send the variometer tones directly into earbuds. Output data is Flysight compatible or can be analyzed in the Firebase App
- main reason for the jumpLogger project was to measure True Airspeed (TAS). This is a problem, since the typical pitot-tube is not feasible for skydiving nor canopy flying. However, math is king: you can, under certain conditions, calculate in-airspeed from groundspeed. This will be implemented soon.\
[windspeed estimation without airspeed sensor](https://diydrones.com/forum/topics/wind-estimation-without-an)

## more info

You can find a more detailed feature list at [jumpLogger features](https://crwdgs.com/jumplogger.html)

A preliminary manual is available at [jumpLogger manual](https://crwdgs.com/manual.html)

![jumpLogger in use](/pics/JL01-0.jpg)
![3D printes case, PA12](/pics/case0.jpg)
 

