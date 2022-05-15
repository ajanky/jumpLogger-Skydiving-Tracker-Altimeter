/////////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV - I2C Communications, Default Configuration, Normal Conversion, FIFO
/////////////////////////////////////////////////////////////////////////////////////

#include <BMP388_DEV.h>                             // Include the BMP388_DEV.h library

#define NO_OF_MEASUREMENTS   10                     // Number of measurements to be stored in the FIFO before reading

float temperature[NO_OF_MEASUREMENTS];              // Create the temperature, pressure and altitude array variables
float pressure[NO_OF_MEASUREMENTS];
float altitude[NO_OF_MEASUREMENTS];
uint32_t sensorTime;                                // Sensor time
BMP388_DEV bmp388;                                  // Instantiate (create) a BMP388_DEV object and set-up for I2C operation (address 0x77)

void setup() 
{
  Serial.begin(115200);                             // Initialise the serial port
  bmp388.begin();                                   // Default initialisation, place the BMP388 into SLEEP_MODE 
  bmp388.enableFIFO();                              // Enable the BMP388's FIFO
  bmp388.setFIFONoOfMeasurements(NO_OF_MEASUREMENTS);   // Store 10 measurements in the FIFO before reading
  bmp388.setTimeStandby(TIME_STANDBY_1280MS);       // Set the standby time to 1.3 seconds
  bmp388.startNormalConversion();                   // Start BMP388 continuous conversion in NORMAL_MODE
  Serial.println(F("Please wait for 13 seconds...")); // Wait message
}

void loop() 
{
  if (bmp388.getFIFOData(temperature, pressure, altitude, sensorTime))    // If the FIFO data is ready: get the 10 measurement readings
  {
    for (uint16_t i = 0; i < NO_OF_MEASUREMENTS; i++)     // Display the results 
    {
      Serial.print(i + 1);                            
      Serial.print(F(" : "));
      Serial.print(temperature[i]);                         
      Serial.print(F("*C   "));
      Serial.print(pressure[i]);    
      Serial.print(F("hPa   "));
      Serial.print(altitude[i]);
      Serial.println(F("m"));  
    }
    Serial.print(F("Sensor Time: "));
    Serial.println(sensorTime);
    Serial.println();
    Serial.println(F("Please wait for 13 seconds..."));   // Wait message
  }
}
