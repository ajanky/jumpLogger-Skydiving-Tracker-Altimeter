// Jumplogger - GMG12864-06D - SD Card GPX - new fonts - 19.04.22

#include <jumpLogger1.h>
#include <web.h>
// JL1-TRKR-03 // dbl tap, added marking in GPX/CSV - 02.05.22
// jumpLogger1-05 //  added webserver - 03.05.22
// jumpLogger1-06 //  iphone Hotspot, arrow 20px - 06.05.22
// jumpLogger1-07 //  disp 5m steps, Set with hold2s , time display - 09.05.22
// jumpLogger1-08 //  BMP in normal Mode/80ms; MultiWifi added, zeroAlt fixed - 10.05.22
// jumpLogger1-09 //  added glide ratio - 12.05.22
#define Version$ "jumpLogger1-10" //  new web interface w/ ShowGPX - 14.05.22



//****************************************************************************
//   Setup Funktion
//****************************************************************************

void setup()
{

  Serial.begin(115200); // Serial ist die Ausgabe im Serial Monitor

  pinMode(Tast1, INPUT_PULLUP);
  pinMode(Tast2, INPUT_PULLUP);
  pinMode(Tast3, INPUT_PULLUP);
  pinMode(BL, OUTPUT);
  pinMode(MSG, OUTPUT);
  pinMode(LA, OUTPUT);
  Zpressure = 1013.25; // standard sea level

  delay(250);
  u8g2.begin();
  u8g2.setFlipMode(flip);
  u8g2.setContrast(125); //

  delay(250);
  u8g2.setBusClock(4000000);                 // high speed i2c
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17); // init GPS serial
  changeBaudrate();
  delay(100);
  SerialGPS.flush();
  SerialGPS.begin(115200, SERIAL_8N1, 16, 17); // new 115k boost mode

  BatLight();

  u8g2.clearBuffer();                    // clear the internal memory
  u8g2.drawXBMP(12, 5, 100, 25, crwdgs); // show logo
  u8g2.setFont(u8g2_font_helvB08_tf);    // choose font
  u8g2.drawStr(12, 40, "Marion's JumpLogger");
  u8g2.setFont(u8g2_font_helvR08_tf); // choose font
  u8g2.drawStr(12, 50, "emm@crwdgs.com");
  u8g2.drawStr(25, 60, Version$);
  u8g2.sendBuffer(); // transfer internal memory to the display
  Serial.println("marion's JumpLogger");
  delay(2000);

  /* 1 tick take 1/(80MHZ/80000) = 1ms so we set divider 80 and count up */

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true);
  timerAlarmEnable(timer);
  Serial.println("start timer");

  // initialize SD card
  Serial.print(F("Initializing SD card ..."));

  if (!SD.begin())
  {
    Serial.println(F("Card initialization failed!"));
    u8g2.setFont(u8g2_font_chikita_tr);
    u8g2.setCursor(10, 10);
    u8g2.print("CARD FAILED");
    u8g2.sendBuffer();
    delay(2000);
  }
  else
  {
    Serial.println(F("Card OK"));
  }

  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  ///////////////////

  bmp388.begin(); // Default initialisation, place the BMP388 into SLEEP_MODE
  bmp388.setTimeStandby(TIME_STANDBY_80MS );     // Set the standby time to 80ms
  bmp388.setIIRFilter(IIR_FILTER_4 );
  bmp388.startNormalConversion();
  delay(100);

  ADXL.init();
  if (!ADXL.init())
  {
    Serial.println("ADXL345 not connected!");
  }
  ADXL.setDataRate(ADXL345_DATA_RATE_50);
  ADXL.setRange(ADXL345_RANGE_4G);

  ADXL.setGeneralTapParameters(ADXL345_X00, 4.0, 30, 100.0);
  ADXL.setAdditionalDoubleTapParameters(false, 250);
  ADXL.setInterrupt(ADXL345_DOUBLE_TAP, INT_PIN_2);

  u8g2.clearBuffer(); // clear screen

/////// auto set zero
  bmp388.getPressure(pressure);
  Zpressure = pressure;
  bmp388.setSeaLevelPressure(Zpressure); // set alt to zero

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
 

}

//********************************************************************************
//  Main Loop
//****************************************************************************
void loop()
{

  if (getSample) // 10times per sec
  {

    ///// event marking by double tap
    byte intSource = ADXL.readAndClearInterrupts();
    if (ADXL.checkInterrupt(intSource, ADXL345_DOUBLE_TAP))
    {
      if (Logging) // mark event if logging by dbl tap
      {
        name$ = "event";
        u8g2.sendF("c", 0x0a7); // display inverse
      }

      if (!Logging && fix && !MENU)
      {
        Logging = ON;
        startLogging();
      }
    }
    ///////

    if (Logging && menuCount >=20) // STOP logging if hold Menu >2s
    {
      Logging = OFF;
      u8g2.setFont(u8g2_font_chikita_tr);
      u8g2.setCursor(10, 15);
      u8g2.print("LOGGING STOPPED");
      u8g2.sendBuffer();
      delay(1000);
    }

    if (bmp388.getPressure(pressure))
    { 
      ICAO = caltitude * 0.0065;                                                                           // last computed caltitude sets ICAO standard temperature for altitude calculation
      caltitude = ((powf(Zpressure / pressure, 0.190223f) - 1.0f) * (273.15f + 15 - ICAO)) / 0.0065f;
      // velD = caltitude - Lastcaltitude;  // calculated on per second
      // Lastcaltitude = caltitude; // calculate sinkrate

      g = ADXL.getGValues();
      fltGx = GXfilter.filter(g.x);
      fltGy = GYfilter.filter(g.y);
      fltGz = GZfilter.filter(g.z);

      if (gps.satellites.value() > 4 && gps.location.age() < 2000) // only with valid fix and not older than 2s
      {
        fix = true;                                                  // valid GPS fix
        filteredLat = LatFilter.filter(gps.location.lat() * 100000); // KalmanLat.updateEstimate(gps.location.lat() );
        filteredLon = LonFilter.filter(gps.location.lng() * 100000); // KalmanLon.updateEstimate(gps.location.lng() );
        dist = TinyGPSPlus::distanceBetween(filteredLat / 100000.0, filteredLon / 100000.0, tgtLat, tgtLon);
        trueBearing = TinyGPSPlus::courseTo(gps.location.lat(), gps.location.lng(), tgtLat, tgtLon);
        altitude = gps.altitude.meters();  // (altitude + gps.altitude.meters())/2; // filtered GPS altitude
        trueCourse = gps.course.deg();
        relBearing = constrainAngle(trueBearing - trueCourse);
        speed = gps.speed.mps();
        CourseRad = trueCourse * 0.01745329252;
        velN = round(speed * cos(CourseRad));
        velE = round(speed * sin(CourseRad));
      }
    }
    getSample = done;
  }

  // Set Mode
  while (MENU && (caltitude < 20) && !Logging) // select MODE only if near ground and not logging
  {
    if (SET0)
    {
      flip = !flip;
      setup();
    }
    if (SETDZ)
      MODE++;

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
      u8g2.setCursor(30, 15);
      u8g2.print("DATA MODE");
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
    u8g2.sendBuffer();
    delay(100);
  }

  

  if (setCount >= 20) // set ALT to zero and wait a bit until button released (hold > 2s)
  {
    bmp388.getPressure(pressure);
    Zpressure = pressure;
    bmp388.setSeaLevelPressure(Zpressure); // set alt to zero
    u8g2.setFont(u8g2_font_chikita_tr);
    u8g2.setCursor(15, 15);
    u8g2.print("ALT ZERO");
    u8g2.sendBuffer();
    AltSet = true;
    u8g2.clearBuffer();  //remove artefacts
    delay(1000);
  }

  if (dzCount >= 20) // set DZ target and wait a bit until button released (only if near ground!)
  {
    tgtLat = filteredLat / 100000.0;
    tgtLon = filteredLon / 100000.0;
    u8g2.setFont(u8g2_font_chikita_tr);
    u8g2.setCursor(20, 15);
    u8g2.print("DZ SET");
    u8g2.sendBuffer();
    Set = true;
    delay(1000);
  }

  while (SerialGPS.available() > 0) // feed the gps fixes into decoder
  {
    gps.encode(SerialGPS.read());
  }

  if (!Set)
  {
    tgtLat = filteredLat / 100000.0;
    tgtLon = filteredLon / 100000.0;
  }

  // twice a second
  if (MS500) // screen update
  {
    digitalWrite(LA, HIGH); //  LA Timing
    if (fix && MS1000)
    digitalWrite(MSG, ON);  // toggle MSG

    u8g2.clearBuffer();            // clear screen
    distFkt = 0.5 + (dist / 2000); // -> maximum length at 3km
    if (distFkt >= 2)
    {
      distFkt = 2.0;
    }

    if (MODE == GPX || MODE == CSV)
    {
      WiFi.mode(WIFI_OFF);
      //timerAlarmEnable(timer);
      runonce = ON;

      drawCompass();
      // alt = (int)((caltitude + 5) / 10) * 10;  // round to 10m
      alt = (int)((caltitude + 2.5) / 5) * 5;  // round to 5m



      u8g2.setFont(u8g2_font_helvB24_tn);      // u8g2_font_logisoso24_tn); // LUB= w18, start 57  u8g2_font_luBS19_tr);
      
      if (alt < 0)
        xoff = digitWidth;
      if (alt >= 0)
        xoff = digitWidth * 3;
      if (alt > 9)
        xoff = digitWidth * 2;
      if (alt > 99)
        xoff = digitWidth;
      if (alt > 999)
        xoff = 0;
      if (alt > 9999)
        xoff = -1 * digitWidth;


      u8g2.setCursor(57 + xoff, 24);
      u8g2.print(alt); // rounded to 5

      u8g2.setFont(u8g2_font_logisoso16_tf); //
      u8g2.setCursor(70, 43);
      u8g2.print(speed);
      u8g2.setCursor(102, 43);
      u8g2.print(GlideG,1);
      u8g2.setFont(u8g2_font_chikita_tr);
      //u8g2.setCursor(103, 34);
      //u8g2.print("m/s");

      u8g2.setFont(u8g2_font_helvB08_tf);
      u8g2.setCursor(104, 55);  // 45);
      if (MODE == GPX)
        u8g2.print("GPX");
      else
        u8g2.print("CSV");

      u8g2.setFont(u8g2_font_logisoso16_tf);
      u8g2.setCursor(70, 62);
      u8g2.print(dist / 1000, 1); // m to km

      u8g2.setFont(u8g2_font_chikita_tr);
      u8g2.setCursor(104, 63);   // 53);
      u8g2.print(u8x8_u8toa(gps.time.hour()+TZ, 2)); u8g2.print(":"); u8g2.print(u8x8_u8toa(gps.time.minute(), 2));
      //u8g2.setCursor(99, 62);
      //u8g2.print("kmDST");
      u8g2.setCursor(0, 5);
      u8g2.print(battery);
      u8g2.print("%"); // Battery Status
      u8g2.setCursor(0, 63);
      u8g2.setFont(u8g2_font_t0_12_tn);
      u8g2.print(gps.satellites.value());

      u8g2.sendBuffer(); // transfer internal memory to the display
    }

    if (MODE == DATA)
    {
      WiFi.mode(WIFI_OFF);
      //timerAlarmEnable(timer);
      runonce = ON;

      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.drawStr(9, 8, "SAT ALT");
      u8g2.drawStr(0, 16, "LAT/LON");
      u8g2.setCursor(0, 8);
      u8g2.print(gps.satellites.value()); // Sat
      u8g2.drawStr(0, 63, "SPD");
      u8g2.setCursor(32, 63);
      u8g2.print(speed);

      u8g2.setFont(u8g2_font_VCR_OSD_mn);
      u8g2.setCursor(64, 16);
      u8g2.print(caltitude, 0);

      u8g2.setFont(u8g2_font_crox2hb_tr); // u8g2_font_t0_16b_mr);
      u8g2.setCursor(0, 40);
      u8g2.print(filteredLat / 100000.f, 4);
      u8g2.setCursor(64, 40);
      u8g2.print(filteredLon / 100000.f, 4);

      u8g2.setCursor(0, 53);
      u8g2.print(fltGx, 2);
      u8g2.setCursor(45, 53);
      u8g2.print(fltGy, 2);
      u8g2.setCursor(90, 53);
      u8g2.print(fltGz, 2);

      u8g2.sendBuffer(); // transfer internal memory to the display
    }

    if (MODE == WEB)
    {
      if (runonce)
      {
        //timerAlarmDisable(timer);
        initwifi();
        setupurl();
        delay(2000);
        u8g2.setFont(u8g2_font_helvB08_tf);
        u8g2.setCursor(15, 15);
        u8g2.print("WIFI SERVER MODE");
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.setCursor(15, 33);
        u8g2.print("connected: ");
        u8g2.print(WiFi.SSID());
        u8g2.setCursor(15, 43);
        u8g2.print(WiFi.localIP().toString());
        u8g2.setCursor(15, 53);
        u8g2.print("jumploggerM.local");
        u8g2.sendBuffer(); // transfer to display
        runonce = OFF;
      }
    }
    countMin++;
    u8g2.sendF("c", 0x0a6); // display normal
    MS500 = false;
    digitalWrite(LA, LOW); // LA Timing
    digitalWrite(MSG, OFF);  // toggle MSG
  }



  if (MS1000)  // every second
  { 
    velD = caltitude - Lastcaltitude;  // calculated per second
    Lastcaltitude = caltitude; // calculate sinkrate
    if (velD != 0.0) GlideG = GlideFilter.filter (speed / velD); // glide = m/s / sink/s
    else GlideG = 0; 

    if (Logging)  // write trackpoint every second
    {
      if (MODE == GPX)
        addGPXTrkpt();
      if (MODE == DATA || MODE == CSV)
        addCSVTrkpt();

      u8g2.setFont(u8g2_font_helvB08_tf);
      u8g2.setDrawColor(z);
      u8g2.drawBox(99, 36, 30, 10);
      z = !z;
      u8g2.setDrawColor(z);
      u8g2.setCursor(104, 55);
      if (MODE == GPX)
        u8g2.print("GPX");
      else
        u8g2.print("CSV");

        u8g2.sendBuffer();
    }

    if (trk_is_init && !Logging) // logging finished
    {
      if (MODE == GPX)
      {
        completeGPX();
      }
      trk_is_init = false; // current file is completed and not initialized anymore
    }
    MS1000 = false;
  }

  if (countMin >= 60)
  {
    BatLight(); // every minute
    countMin = 0;
  }
}
