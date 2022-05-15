/*
  BMP280_DEV is an I2C/SPI compatible library for the Bosch BMP280 barometer.
	
	Copyright (C) Martin Lindupp 2019
	
	V1.0.0 -- Initial release 		
	V1.0.1 -- Added ESP32 HSPI support and change library to unique name
	V1.0.2 -- Modification to allow external creation of HSPI object on ESP32
	V1.0.3 -- Changed library name in the library.properties file
	V1.0.5 -- Fixed bug in BMP280_DEV::getTemperature() function, thanks to Jon M.
	V1.0.6 -- Merged multiple instances and initialisation pull requests by sensslen
	V1.0.8 -- Used default arguments for begin() member function and 
						added example using multiple BMP280 devices with SPI comms in NORMAL mode	
	V1.0.9 -- Moved writeMask to Device class and improved measurement detection code
	V1.0.10 -- Modification to allow user-defined pins for I2C operation on the ESP8266
	V1.0.12 -- Allow sea level pressure calibration using setSeaLevelPressure() function
	V1.0.14 -- Fix uninitialised structures, thanks to David Jade investigating and 
						 flagging up this issue
	V1.0.16 -- Modification to allow user-defined pins for I2C operation on the ESP32
	V1.0.17 -- Added getCurrentTemperature(), getCurrentPressure(), getCurrentTempPres() 
						 getCurrentAltitude() and getCurrentMeasurements() functions,
						 to allow the BMP280 to be read directly without checking the measuring bit
	V1.0.18 -- Initialise "device" constructor member variables in the same order they are declared
	
	The MIT License (MIT)
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#ifndef BMP280_DEV_h
#define BMP280_DEV_h

#include <Device.h>

////////////////////////////////////////////////////////////////////////////////
// BMP280_DEV Definitions
////////////////////////////////////////////////////////////////////////////////

#define BMP280_I2C_ADDR		 		0x77				// The BMP280 I2C address
#define BMP280_I2C_ALT_ADDR 	0x76				// The BMP280 I2C alternate address
#define DEVICE_ID 						0x58				// The BMP280 device ID
#define RESET_CODE						0xB6				// The BMP280 reset code

enum SPIPort { BMP280_SPI0, BMP280_SPI1 };

////////////////////////////////////////////////////////////////////////////////
// BMP280_DEV Registers
////////////////////////////////////////////////////////////////////////////////

enum {
	BMP280_TRIM_PARAMS		 = 0x88,          // Trim parameter registers' base sub-address
	BMP280_DEVICE_ID 			 = 0xD0,          // Device ID register sub-address
	BMP280_RESET 					 = 0xE0,          // Reset register sub-address
	BMP280_STATUS      		 = 0xF3,          // Status register sub-address
	BMP280_CTRL_MEAS   		 = 0xF4,          // Control and measurement register sub-address
	BMP280_CONFIG       	 = 0xF5,          // Configuration register sub-address
	BMP280_PRES_MSB    		 = 0xF7,          // Pressure Most Significant Byte (MSB) register sub-address
	BMP280_PRES_LSB    		 = 0xF8,          // Pressure Least Significant Byte (LSB) register sub-address
	BMP280_PRES_XLSB   		 = 0xF9,          // Pressure eXtended Least Significant Byte (XLSB) register sub-address
	BMP280_TEMP_MSB    		 = 0xFA,          // Pressure Most Significant Byte (MSB) register sub-address
	BMP280_TEMP_LSB    	 	 = 0xFB,          // Pressure Least Significant Byte (LSB) register sub-address
	BMP280_TEMP_XLSB    	 = 0xFC 					// Pressure eXtended Least Significant Byte (XLSB) register sub-address
};          

////////////////////////////////////////////////////////////////////////////////
// BMP280_DEV Modes
////////////////////////////////////////////////////////////////////////////////

enum Mode {
	SLEEP_MODE          	 = 0x00,          // Device mode bitfield in the control and measurement register 
	FORCED_MODE         	 = 0x01,
	NORMAL_MODE         	 = 0x03
};

////////////////////////////////////////////////////////////////////////////////
// BMP280_DEV Register bit field Definitions
////////////////////////////////////////////////////////////////////////////////

enum Oversampling {
	OVERSAMPLING_SKIP 		 = 0x00,     			// Oversampling bit fields in the control and measurement register
	OVERSAMPLING_X1   		 = 0x01,
	OVERSAMPLING_X2   		 = 0x02,
	OVERSAMPLING_X4  		   = 0x03,
	OVERSAMPLING_X8    		 = 0x04,
	OVERSAMPLING_X16   	 	 = 0x05
};

enum IIRFilter {
	IIR_FILTER_OFF  			 = 0x00,     			// Infinite Impulse Response (IIR) filter bit field in the configuration register
	IIR_FILTER_2    			 = 0x01,
	IIR_FILTER_4           = 0x02,
	IIR_FILTER_8           = 0x03,
	IIR_FILTER_16          = 0x04
};

enum TimeStandby {
	TIME_STANDBY_05MS      = 0x00,     		  // Time standby bit field in the configuration register
	TIME_STANDBY_62MS      = 0x01,
	TIME_STANDBY_125MS     = 0x02,
	TIME_STANDBY_250MS     = 0x03,
	TIME_STANDBY_500MS     = 0x04,
	TIME_STANDBY_1000MS    = 0x05,
	TIME_STANDBY_2000MS    = 0x06,
	TIME_STANDBY_4000MS    = 0x07
};

////////////////////////////////////////////////////////////////////////////////
// BMP280_DEV Class definition
////////////////////////////////////////////////////////////////////////////////

class BMP280_DEV : public Device {															// Derive the BMP280_DEV class from the Device class
	public:
		BMP280_DEV();																								// BMP280_DEV object for I2C operation
#ifdef ARDUINO_ARCH_ESP8266
		BMP280_DEV(uint8_t sda, uint8_t scl);												// BMP280_DEV object for ESP8266 I2C operation with user-defined pins
#endif
		BMP280_DEV(uint8_t cs);																			// BMP280_DEV object for SPI operation
#ifdef ARDUINO_ARCH_ESP32
		BMP280_DEV(uint8_t sda, uint8_t scl);												// BMP280_DEV object for ESP32 I2C operation with user-defined pins
		BMP280_DEV(uint8_t cs, uint8_t spiPort, SPIClass& spiClass);	// BMP280_DEV object for SPI1 with supplied SPIClass object
#endif
		uint8_t begin(Mode mode = SLEEP_MODE, 												// Initialise the barometer with arguments
									Oversampling presOversampling = OVERSAMPLING_X16, 
									Oversampling tempOversampling = OVERSAMPLING_X2, 
									IIRFilter iirFilter = IIR_FILTER_OFF, 
									TimeStandby timeStandby = TIME_STANDBY_05MS);
		uint8_t begin(Mode mode, uint8_t addr);											// Initialise the barometer specifying start mode and I2C addrss
		uint8_t begin(uint8_t addr);																// Initialise the barometer specifying I2C address with default initialisation
		void reset();																								// Soft reset the barometer		
		void startNormalConversion();																// Start continuous measurement in NORMAL_MODE
		void startForcedConversion();															  // Start a one shot measurement in FORCED_MODE
		void stopConversion();																			// Stop the conversion and return to SLEEP_MODE
		void setPresOversampling(Oversampling presOversampling);		// Set the pressure oversampling: OFF, X1, X2, X4, X8, X16
		void setTempOversampling(Oversampling tempOversampling);		// Set the temperature oversampling: OFF, X1, X2, X4, X8, X16
		void setIIRFilter(IIRFilter iirFilter);											// Set the IIR filter setting: OFF, 2, 4, 8, 16
		void setTimeStandby(TimeStandby timeStandby);	 							// Set the time standby measurement interval: 0.5, 62, 125, 250, 500ms, 1s, 2s, 4s
		void setSeaLevelPressure(float pressure = 1013.23f);				// Set the sea level pressure value
		void getCurrentTemperature(float &temperature);							// Get the current temperature measurement without checking the measuring bit
		uint8_t getTemperature(float &temperature);									// Get a temperature measurement
		void getCurrentPressure(float &pressure);										// Get the current pressure without checking the measuring bit
		uint8_t getPressure(float &pressure);												// Get a pressure measurement
		void getCurrentTempPres(float &temperature, float &pressure); // Get the current temperature and pressure without checking the measuring bit
		uint8_t getTempPres(float &temperature, float &pressure);		// Get a temperature and pressure measurement
		void getCurrentAltitude(float &altitude);										// Get the current altitude without checking the measuring bit
		uint8_t getAltitude(float &altitude);												// Get an altitude measurement
		void getCurrentMeasurements(float &temperature, float &pressure, float &altitude); // Get all measurements without checking the measuring bit
		uint8_t getMeasurements(float &temperature, float &pressure, float &altitude);	// Get temperature, pressure and altitude measurements
	protected:
	private:
		void setMode(Mode mode);																		// Set the barometer mode
		void setCtrlMeasRegister(Mode mode, Oversampling presOversampling, Oversampling tempOversamping);		// Set the BMP280 control and measurement register
		void setConfigRegister(IIRFilter iirFilter, TimeStandby timeStandby);		// Set the BMP280 configuration register
		uint8_t dataReady();																				// Checks if a measurement is ready
	
		struct {																										// The BMP280 compensation trim parameters (coefficients)
			uint16_t dig_T1;
			int16_t  dig_T2;
			int16_t  dig_T3;
			uint16_t dig_P1;
			int16_t  dig_P2;
			int16_t  dig_P3;
			int16_t  dig_P4;
			int16_t  dig_P5;
			int16_t  dig_P6;
			int16_t  dig_P7;
			int16_t  dig_P8;
			int16_t  dig_P9;
		} params;
			
		union {																											// Copy of the BMP280's configuration register
			struct {
				uint8_t spi3w_en : 1;
				uint8_t 				 : 1;
				uint8_t filter 	 : 3;
				uint8_t t_sb		 : 3;
			} bit;
			uint8_t reg;
		} config = { .reg = 0 };
		
		union {																											// Copy of the BMP280's control and measurement register
			struct {
				uint8_t mode   : 2;
				uint8_t osrs_p : 3;
				uint8_t osrs_t : 3;
			} bit;
			uint8_t reg;
		} ctrl_meas = { .reg = 0 };
			
		union {																											// Copy of the BMP280's status register
			struct {
				uint8_t im_update : 1;
				uint8_t						: 2;
				uint8_t measuring : 1;
			} bit;
			uint8_t reg;
		} status = { .reg = 0 };
		
		int32_t t_fine;																							// Bosch t_fine variable
		int32_t bmp280_compensate_T_int32(int32_t adc_T);						// Bosch temperature compensation function
		uint32_t bmp280_compensate_P_int64(int32_t adc_P);					// Bosch pressure compensation function
		bool previous_measuring;																		// Previous measuring state
		float sea_level_pressure = 1013.23f;												// Sea level pressure
};

#endif
