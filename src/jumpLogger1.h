#pragma once
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <BMP388_DEV.h>
#include <ADXL345_WE.h>
#include <math.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Ewma.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include <WiFi.h>
//#include <WiFiClient.h>
#include <WifiMulti.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>


#define Tast1 32
#define Tast2 33
#define Tast3 27
#define BL 12 // backlight for LCD
#define MSG 4
#define GPX 0
#define DATA 1
#define CSV 2
#define WEB 3
#define LA 2
#define done false
#define ON true
#define OFF false
#define TZ 2  // time zone

//wifi Setting
//const char *ssid="jam2";
//const char *password="T57707300$";

//iPhone HotSpot
//const char *ssid="iPhone 11 Andreas";
//const char *password="bfpycrgbr39mv";

bool SD_present=true;
AsyncWebServer server(80);
WiFiMulti wifiMulti;


int trueBearing, trueCourse, relBearing; // trueBearing for compass heading.
int degree, xoff, alt, light, battery;
float distFkt = 0.5, GlideG, GlideA;
float temperature, pressure, altitude, caltitude, Lastcaltitude, Zpressure, ICAO, velN, velE, velD, tempAlt;
int counter, countMin, hyst, MODE = 0;
bool SET0, SETDZ, MENU, Set, Logging, AltSet, trk_is_init, z, fix, runonce, flip = false;
double tgtLat, tgtLon, dist;
int filteredLat, filteredLon, speed;
byte digitWidth = 18; 
char tme[22];         // 21
volatile bool getSample, MS500, MS1000;
volatile int cnt, setCount, dzCount, menuCount;
xyzFloat g;
float fltGx, fltGy, fltGz, CourseRad;
String name$ = " ";
String val;

File csv, gpx;
int fileID = 0;
char CSVFILE[13], GPXFILE[13];
// char val[6];
BMP388_DEV bmp388;

Ewma LatFilter(0.2);
Ewma LonFilter(0.2);
Ewma GXfilter(0.3);
Ewma GYfilter(0.3);
Ewma GZfilter(0.3);
Ewma GlideFilter(0.3);

// U8G2_ST7565_EA_DOGM128_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ 25, /* reset=*/ 26);  // DOGM6-128x64
U8G2_ST7565_NHD_C12864_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/14, /* data=*/13, /* cs=*/15, /* dc=*/25, /* reset=*/26); // Wide LCD
// U8G2_ST7565_Z_JLX12864_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ 25, /* reset=*/ 26);  // JST 12864 G - SPI

TinyGPSPlus gps;

/* create a hardware timer */
hw_timer_t *timer = NULL;

ADXL345_WE ADXL = ADXL345_WE(0x1D);


//  Enable C++ style printing templates
template <class T>
inline Print &operator<<(Print &str, T arg)
{
  str.print(arg);
  return str;
}

HardwareSerial SerialGPS(1);

void IRAM_ATTR onTimer()  //  TIMER IRQ service routine - 100ms
{ 
  getSample = true; //  100ms 
  if (cnt == 5 || cnt == 10) { MS500 = true; } // MS500
  if (cnt == 10) { MS1000 = true; cnt=0;} // MS1000
  ++cnt;
  SET0 = !digitalRead(Tast1);
    if (SET0) setCount++; else setCount = 0;
  SETDZ = !digitalRead(Tast2);
    if (SETDZ) dzCount++; else dzCount = 0;
  MENU = !digitalRead(Tast3);
    if (MENU) menuCount++; else menuCount = 0;// read buttons
}

void sendPacket(byte *packet, byte len) // config packet for GPS device
{
  for (byte i = 0; i < len; i++)
  {
    SerialGPS.write(packet[i]);
  }
}

// Send a packet to the receiver to change baudrate to 115200.
void changeBaudrate()
{
  // CFG-PRT
  byte packet[] = {
      0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00,
      0x00, 0x00, 0xC2, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
      0xC0, // CK_A
      0x7E, // CK_B
  };
  sendPacket(packet, sizeof(packet));
}


void drawCompass()
{
  static int arrowLength = 20;  // was 25
  static int cx = 33;
  static int cy = 31;
  int arrow1X, arrow1Y, arrow2X, arrow2Y; // armX, armY,

  // convert degree to radian
  float arm1Rad = (relBearing - 15) / 57.2957795; // calculate for little arrow arms +/- a few degrees.
  float arm2Rad = (relBearing + 15) / 57.2957795;

  u8g2.setDrawColor(1);
  u8g2.drawLine(cx, cy, cx - 8, cy + 28);
  u8g2.drawLine(cx, cy, cx + 8, cy + 28);

  if (speed >= 1) // no Bearing if slower than 1m/s
  {

    if (dist >= 100) // closer is target
    {

      arrow1X = (arrowLength * cos(arm1Rad) * distFkt); // x and y offsets to draw the arrow bits
      arrow1Y = (-arrowLength * sin(arm1Rad) * distFkt);
      arrow2X = (arrowLength * cos(arm2Rad) * distFkt); // x and y offsets to draw the rest of the arrow bits
      arrow2Y = (-arrowLength * sin(arm2Rad) * distFkt);

      // u8g2.drawCircle(cx, cy+1, 32, U8G2_DRAW_ALL);

      u8g2.drawTriangle(cx, cy, cx + arrow1Y, cy + arrow1X, cx + arrow2Y, cy + arrow2X);
    }

    else // at target !
    {
      u8g2.drawDisc(cx, cy, 5);
      u8g2.drawCircle(cx, cy, 10);
      u8g2.drawCircle(cx, cy, 15);
    }
  }
  else
  {
    u8g2.setFont(u8g2_font_chikita_tr);
    u8g2.setCursor(20, 30);
    u8g2.print("NO DIR");
  }
}

unsigned int rundenAuf(unsigned int zahl, byte auf)
{
  return ((zahl + auf / 2) / auf) * auf;
}

// Normalize to [0,360):

int constrainAngle(int x)
{
  x = fmod(x, 360);
  if (x < 0)
    x += 360;
  return x;
}

// Normalize to [-180,180):

double constrainAngle(double x)
{
  x = fmod(x + 180, 360);
  if (x < 0)
    x += 360;
  return x - 180;
}


// Initialize the CSV file structure
void initCSV()
{
  Serial.println(F("Initializing CSV file ..."));
  csv = SD.open(CSVFILE, FILE_WRITE);

  if (csv)
  {
    // header
    csv.println("time,lat,lon,hMSL,velN,velE,velD,hAcc,vAcc,sAcc,gpsFix,numSV");
    csv.println(",(deg),(deg),(m),(m/s),(m/s),(m/s),(m),(m),(m/s),,,");

    csv.close();
  }
  else
  {
    Serial.println(F("Error opening csv-file"));
  }
}




// Initialize the gpx file structure
void initGPX()
{
  Serial.println(F("Initializing GPX file ..."));
  gpx = SD.open(GPXFILE, FILE_WRITE);

  if (gpx)
  {
    // header
    gpx.print(F("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n"));

    // start of gpx file body
    gpx.print(F("<gpx version=\"1.1\" creator=\"janky\">\n"));

    // metadata
    gpx.print(F("  <metadata>\n"));
    gpx << "    <name>" << GPXFILE << "</name>\n";
    gpx.print(F("    <desc>Tracked with JumpLogger</desc>\n"));
    //gpx.print(F("    <author>\n"));
    //gpx.print(F("      <name>janky</name>\n"));
    //gpx.print(F("      <email>janky@crwdgs.com</email>\n"));
    //gpx.print(F("    </author>\n"));
    gpx.print(F("  </metadata>\n"));

    // start track and track segment (only single segmented tracks possible)
    gpx.print(F("  <trk>\n"));
    gpx << "    <name>" << GPXFILE << "</name>\n";
    gpx.print(F("    <desc>Tracked with JumpLogger</desc>\n"));
    gpx.print(F("    <trkseg>\n"));

    gpx.close();
  }
  else
  {
    Serial.println(F("Error opening gpx-file"));
  }
}



//  Create a trackpoint in csv syntax
void addCSVTrkpt()
{
  Serial.println(F("Adding Trackpoint ..."));
  csv = SD.open(CSVFILE, FILE_APPEND);

  if (csv)
  {
    char time[21];
    sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02dZ", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
    Serial.print(time);
    csv.print(time);
    csv.print(F(","));
    csv.print(filteredLat / 100000.0, 5);
    csv.print(F(","));
    csv.print(filteredLon / 100000.0, 5);
    csv.print(",");
    csv.print(caltitude);
    csv.print(",");
    csv.print(velN);
    csv.print(",");
    csv.print(velE);
    csv.print(",");
    csv.print(velD);
    csv.print(",");
    csv.print("1.0"); // hAcc);
    csv.print(",");
    csv.print("1.0"); // vAcc);
    csv.print(",");
    csv.print("1.0"); // sAcc);
    csv.print(",");
    csv.print("3");
    csv.print(",");
    csv.print(gps.satellites.value());
    if (name$ != " ") {csv.print(",");csv.print(name$);}
    csv.print("\n");
    name$ = " ";
    csv.close();
  }
  else
  {
    Serial.println(F("Error opening csv-file"));
  }
}

//  Create a trackpoint in gpx syntax
void addGPXTrkpt()
{
  //Serial.println(F("Adding Trackpoint ..."));
  gpx = SD.open(GPXFILE, FILE_APPEND);

  if (gpx)
  {
    gpx.print(F("  <trkpt lat=\""));
    gpx.print(filteredLat/100000.0, 5); 
    gpx.print(F("\" lon=\""));
    gpx.print(filteredLon/100000.0, 5); 
    gpx.print("\"> \n");
    gpx << "    <ele>" << caltitude << "</ele>\n";
    char time[21];
    sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02dZ", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
    gpx << "    <time>" << time << "</time>\n";
    if (name$ != " ") gpx << "    <name>" << name$ << "</name>\n";
    gpx << "     <extensions>\n";
    gpx << "      <gpxtpx:TrackPointExtension>\n";
    gpx << "       <gpxtpx:gpsalt>"  << altitude  <<  "</gpxtpx:gpsalt>\n" ;
    //gpx << "       <gpxtpx:allalts>"  << alts  <<  "</gpxtpx:allalts>\n" ;
    gpx << "       <gpxtpx:speed>"  << speed  <<  "</gpxtpx:speed>\n" ;
    gpx << "      </gpxtpx:TrackPointExtension>\n";
    gpx << "     </extensions>\n" ;
    gpx << "    </trkpt>\n";
    gpx.close();

    name$ = " ";
  }
  else
  {
    Serial.println(F("Error opening gpx-file"));
  }
}


//  Conclude the gpx file structure
void completeGPX()
{
  Serial.println(F("Completing GPX file ..."));
  gpx = SD.open(GPXFILE, FILE_APPEND);

  if (gpx)
  {
    // complete recorded track, track segment and gpx file body
    gpx.print(F("    </trkseg>\n"));
    gpx.print(F("  </trk>\n"));
    gpx.print(F("</gpx>\n"));
    gpx.close();
  }
  else
  {
    Serial.println(F("Error opening gpx-file"));
  }
}



void BatLight()    // check battery and light
{
  battery = int((((analogRead(35) * 1.73) - 3200) / 100)+0.5) * 10; //  Voltage range 3.2V...4.2V = 0% ... 100%  - faktor was 1.7

  light = analogRead(34);
  if (light > (600 - hyst))
  {
    digitalWrite(BL, HIGH);
    hyst = 200;
  } // light up display, set hysteresis
  else
  {
    digitalWrite(BL, LOW);
    hyst = 0;
  } // no backlight
}



void MenuSelect() 
{
  while (MENU && (caltitude < 10) && !Logging) // select MODE only if near ground and not logging
  {
    if (SET0) MODE++;
    if (SETDZ) MODE--;

    if (MODE >= 4)
      MODE = 0; // toggle MENU 0..3

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB08_tf);

    if (MODE == GPX)
    {
      u8g2.setCursor(0, 15);
      u8g2.print("ALTI/LOGGER GPX MODE");
    }
    if (MODE == DATA)
    {
      u8g2.setCursor(20, 15);
      u8g2.print("MEASURE MODE");
    }
    if (MODE == CSV)
    {
      u8g2.setCursor(0, 15);
      u8g2.print("ALTI/LOGGER CSV MODE");
    }
    if (MODE == WEB)
    {
      u8g2.setCursor(15, 15);
      u8g2.print("WIFI SERVER MODE");
      runonce = ON;
    }
  }
  u8g2.sendBuffer();
  delay(1000);
}


void startLogging()
{
  if (Logging)
  {
    while (1)
    {

      if (MODE == GPX)
      {
        sprintf(GPXFILE, "/%02d%02d%02d-%02d.gpx", gps.date.year(), gps.date.month(), gps.date.day(), fileID);
        if (!SD.exists(GPXFILE))
        {
          break;
        }
        else
        {
          fileID++;
        }
      }

      if (MODE == DATA || MODE == CSV)
      {
        sprintf(CSVFILE, "/%02d%02d%02d-%02d.csv", gps.date.year(), gps.date.month(), gps.date.day(), fileID);
        if (!SD.exists(CSVFILE))
        {
          break;
        }
        else
        {
          fileID++;
        }
      }
    }

    if (MODE == GPX)
    {
      Serial << F("Current gpx file: ") << GPXFILE << "\n";
      initGPX();          // initialize gpx file header
      trk_is_init = true; // gpx file is initialized now
    }

    if (MODE == DATA || MODE == CSV)
    {
      Serial << F("Current CSV file: ") << CSVFILE << "\n";
      initCSV();          // initialize gpx file header
      trk_is_init = true; // gpx file is initialized now
    }
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_chikita_tr);
    u8g2.setCursor(10, 15);
    u8g2.print("LOGGING STARTED");
    u8g2.sendBuffer();
  }
}



////////////////
// width 100
// height 25
const unsigned char crwdgs[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xF8, 0x9F, 0xFF, 0xC3, 0x83, 0x0F, 0x1E, 0xFF, 0x0F, 0x00, 0x00, 
  0x00, 0x00, 0xFC, 0x9F, 0xFF, 0xC7, 0xC3, 0x1F, 0x9E, 0xFF, 0x1F, 0x00, 
  0x00, 0x00, 0x00, 0x7C, 0x80, 0xC7, 0xCF, 0xC3, 0x1F, 0x9F, 0x0F, 0x1F, 
  0x00, 0x00, 0x00, 0x00, 0x3E, 0x80, 0x87, 0xCF, 0xC7, 0x1F, 0x9F, 0x0F, 
  0x3E, 0xC0, 0x1F, 0xF0, 0x01, 0x1E, 0x80, 0x87, 0xCF, 0xC7, 0x1F, 0x9F, 
  0x0F, 0x3C, 0xF8, 0x3F, 0xFC, 0x07, 0x1E, 0x80, 0x07, 0xCF, 0xC7, 0x1F, 
  0x8F, 0x0F, 0x3C, 0xFC, 0x3F, 0xFC, 0x07, 0x1E, 0x80, 0x87, 0xCF, 0xE7, 
  0x1F, 0x8F, 0x0F, 0x3C, 0x3C, 0x3C, 0x3E, 0x00, 0x1E, 0x80, 0x87, 0x8F, 
  0xE7, 0x3F, 0x8F, 0x0F, 0x3C, 0x3E, 0x3C, 0x3E, 0x00, 0x1E, 0x80, 0xCF, 
  0x87, 0xE7, 0x3F, 0x8F, 0x0F, 0x3C, 0x3E, 0x3C, 0x7C, 0x00, 0x1E, 0x80, 
  0xFF, 0x81, 0xE7, 0xBD, 0x8F, 0x0F, 0x3C, 0x3E, 0x3C, 0xFC, 0x00, 0x1E, 
  0x80, 0xFF, 0x81, 0xEF, 0xBD, 0x8F, 0x0F, 0x3C, 0x1E, 0x3C, 0xF8, 0x01, 
  0x1E, 0x80, 0xEF, 0x83, 0xFF, 0xBD, 0x87, 0x0F, 0x3C, 0x1E, 0x3C, 0xE0, 
  0x03, 0x1E, 0x80, 0xCF, 0x83, 0xFF, 0xFC, 0x87, 0x0F, 0x3C, 0x3E, 0x3C, 
  0xC0, 0x07, 0x1E, 0x80, 0xCF, 0x07, 0xFF, 0xF8, 0x87, 0x0F, 0x3E, 0x3E, 
  0x3C, 0x80, 0x07, 0x3E, 0x80, 0x8F, 0x0F, 0xFF, 0xF8, 0x87, 0x0F, 0x3E, 
  0x3E, 0x3C, 0x80, 0x07, 0x3C, 0x80, 0x0F, 0x1F, 0xFF, 0xF8, 0x87, 0x0F, 
  0x1F, 0xFC, 0x3E, 0x80, 0x07, 0xFC, 0x9F, 0x0F, 0x1F, 0xFF, 0xF8, 0x87, 
  0xFF, 0x1F, 0xF8, 0x3F, 0xFE, 0x07, 0xF8, 0x9F, 0x0F, 0x1E, 0x7F, 0xF8, 
  0x03, 0xFF, 0x07, 0x80, 0x3F, 0xFE, 0x03, 0xE0, 0x0F, 0x00, 0x00, 0x3C, 
  0xE0, 0x01, 0x00, 0x00, 0x00, 0x3C, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x3F, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x0F, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, };

