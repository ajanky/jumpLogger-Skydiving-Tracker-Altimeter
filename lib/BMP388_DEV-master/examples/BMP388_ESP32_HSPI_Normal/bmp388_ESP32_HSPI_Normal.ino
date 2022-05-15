//////////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV - ESP32 HSPI Communications, Default Configuration, Normal Conversion
//////////////////////////////////////////////////////////////////////////////////////

#include <BMP388_DEV.h>                            // Include the BMP388_DEV.h library

float temperature, pressure, altitude;            // Create the temperature, pressure and altitude variables
//BMP388_DEV bmp388(21);                            // Create BMP388_DEV object and set-up for VSPI operation, SCK 5, MOSI 18, MISO 19, SS 21
SPIClass SPI1(HSPI);                              // Create (instantiate) the SPI1 object for HSPI operation
BMP388_DEV bmp388(21, HSPI, SPI1);                // Create BMP388_DEV object and set-up for HSPI operation, SCK 14, MOSI 13, MISO 27, SS 21

void setup() 
{
  Serial.begin(115200);                           // Initialise the serial port
  bmp388.begin();                                 // Default initialisation, place the BMP388 into SLEEP_MODE 
  bmp388.setTimeStandby(TIME_STANDBY_1280MS);     // Set the standby time to 1.3 seconds
  bmp388.startNormalConversion();                 // Start NORMAL continuous conversion
  
  xTaskCreatePinnedToCore(                        // Kick-off "TaskOne" pinned to core 1
    taskOne,
    "TaskOne",
    10000,
    NULL,
    1,
    NULL,
    1);
}

void taskOne(void* parameter)
{
  while(true)
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
}
