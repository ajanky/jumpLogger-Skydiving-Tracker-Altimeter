//////////////////////////////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV - ESP8266, I2C Communications, Default Configuration, Normal Conversion, User-Defined Pins
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <BMP388_DEV.h>                           // Include the BMP388_DEV.h library

float temperature, pressure, altitude;            // Create the temperature, pressure and altitude variables
BMP388_DEV bmp388(6, 7);                          // Instantiate (create) a BMP388 object and set-up for I2C operation on pins SDA: 6, SCL: 7

void setup() 
{
  Serial.begin(115200);                           // Initialise the serial port
  bmp388.begin();                                 // Default initialisation, place the BMP388 into SLEEP_MODE 
  bmp388.setTimeStandby(TIME_STANDBY_2000MS);     // Set the standby time to 2 seconds
  bmp388.startNormalConversion();                 // Start BMP388 continuous conversion in NORMAL_MODE
}

void loop() 
{
  if (bmp388.getMeasurements(temperature, pressure, altitude))    // Check if the measurement is complete
  {
    Serial.print(temperature);                    // Display the results    
    Serial.print(F("*C   "));
    Serial.print(pressure);    
    Serial.print(F("hPa   "));
    Serial.print(altitude);
    Serial.println(F("m"));  
  }
}
