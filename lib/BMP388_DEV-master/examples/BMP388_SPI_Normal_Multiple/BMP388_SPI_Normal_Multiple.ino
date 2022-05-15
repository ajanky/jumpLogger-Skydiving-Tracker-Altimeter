///////////////////////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV - SPI Communications, Default Configuration, Normal Conversion, Mulitple Devices
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <BMP388_DEV.h>                             // Include the BMP388_DEV.h library

float temperature, pressure, altitude;              // Create the temperature, pressure and altitude variables
BMP388_DEV bmp388_1(10);                            // Instantiate (create) a BMP388_DEV object and set-up for SPI operation on digital pin D10
BMP388_DEV bmp388_2(9);                             // Instantiate (create) a BMP388_DEV object and set-up for SPI operation on digital pin D9

void setup() 
{
  Serial.begin(115200);                             // Initialise the serial port
  bmp388_1.begin();                                 // Default initialisation, place the BMP388 into SLEEP_MODE 
  bmp388_1.setTimeStandby(TIME_STANDBY_1280MS);     // Set the standby time to 1.3 seconds
  bmp388_1.startNormalConversion();                 // Start BMP388 continuous conversion in NORMAL_MODE 
  bmp388_2.begin();                                 // Default initialisation, place the BMP388 into SLEEP_MODE 
  bmp388_2.setTimeStandby(TIME_STANDBY_1280MS);     // Set the standby time to 1.3 seconds
  bmp388_2.startNormalConversion();                 // Start BMP388 continuous conversion in NORMAL_MODE  
}

void loop() 
{
  if (bmp388_1.getMeasurements(temperature, pressure, altitude))    // Check if the measurement is complete
  {
    Serial.print(F("BMP388_1 "));                                   // Display the results   
    Serial.print(temperature);                       
    Serial.print(F("*C   "));
    Serial.print(pressure);    
    Serial.print(F("hPa   "));
    Serial.print(altitude);
    Serial.println(F("m"));  
  }
  if (bmp388_2.getMeasurements(temperature, pressure, altitude))    // Check if the measurement is complete
  {
    Serial.print(F("BMP388_2 "));                                   // Display the results
    Serial.print(temperature);                          
    Serial.print(F("*C   "));
    Serial.print(pressure);    
    Serial.print(F("hPa   "));
    Serial.print(altitude);
    Serial.println(F("m"));  
  }
}
