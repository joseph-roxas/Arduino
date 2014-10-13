#include <Time.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SD.h>

/*
   This sample code demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

int CS_pin = 10;
int pow_pin = 8;

time_t prevDisplay = 0; // Count for when time last displayed
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(115200);
  
  Serial.println("Initializing Card");
  //CS Pin is an output
  pinMode(CS_pin, OUTPUT);
  
  //Card will Draw Power from Pin 8, so set it high
  pinMode(pow_pin, OUTPUT);  
  digitalWrite(pow_pin, HIGH);
  
  if (!SD.begin(CS_pin))
  {
      Serial.println("Card Failure");
      return;
  }
  Serial.println("Card Ready");
  
  
  ss.begin(GPSBaud);

  Serial.println(F("FullExample.ino"));
  Serial.println(F("An extensive example of many interesting TinyGPS++ features"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
  Serial.println(F("Sats HDOP Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
  Serial.println(F("          (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
  Serial.println(F("---------------------------------------------------------------------------------------------------------------------------------------"));
}

void loop()
{
  GPS_Timezone_Adjust();  // Call Time Adjust Function
  
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  //printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  //printInt(gps.hdop.value(), gps.hdop.isValid(), 5);
  //printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  //printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  //printInt(gps.location.age(), gps.location.isValid(), 5);
  //printDateTime(gps.date, gps.time);
  //printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  //printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  //printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  //printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.value()) : "*** ", 6);

  //unsigned long distanceKmToLondon =
  //  (unsigned long)TinyGPSPlus::distanceBetween(
  //    gps.location.lat(),
  //    gps.location.lng(),
  //    LONDON_LAT, 
  //    LONDON_LON) / 1000;
  //printInt(distanceKmToLondon, gps.location.isValid(), 9);

  //double courseToLondon =
  //  TinyGPSPlus::courseTo(
  //    gps.location.lat(),
  //    gps.location.lng(),
  //    LONDON_LAT, 
  //    LONDON_LON);
  
  //printFloat(courseToLondon, gps.location.isValid(), 7, 2);

  //const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

  //printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

  //printInt(gps.charsProcessed(), true, 6);
  //printInt(gps.sentencesWithFix(), true, 10);
  //printInt(gps.failedChecksum(), true, 9);
  //Serial.println();
  
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
  
  
  //Open a file to write to
  //Only one file can be open at a time
  
  //Serial.print("LAT=");  Serial.println(gps.location.lat(), 7);
  //Serial.print("LONG="); Serial.println(gps.location.lng(), 7);
  
  File dataFile = SD.open("GPS_log.csv", FILE_WRITE);
  if (dataFile)
  {
    //dataFile.println(dataString);
    dataFile.print(gps.location.lat(), 7);
    dataFile.print(",");
    dataFile.print(gps.location.lng(),7);
    dataFile.print(",");
        
    if (month() < 10) dataFile.print(F("0"));      
        dataFile.print(month());
        dataFile.print(F("/"));
    if (day() < 10) dataFile.print(F("0"));
        dataFile.print(day());
        dataFile.print(F("/"));
        dataFile.print(year());
    
    dataFile.print(",");
         
    if (hour() < 10) dataFile.print(F("0"));
        dataFile.print(hour());
        dataFile.print(F(":"));
    if (minute() < 10) dataFile.print(F("0"));
        dataFile.print(minute());
        dataFile.print(F(":"));
    if (second() < 10) dataFile.print(F("0"));
        dataFile.print(second()); 
    
    dataFile.print(",");
    dataFile.print(gps.speed.kmph(),3);
    dataFile.print(",");
    dataFile.print(gps.course.deg(),3);
    dataFile.println();
    dataFile.close();
    
  }
  else
  {
    Serial.println("Couldn't open log file");
  }  
  
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}


void GPS_Timezone_Adjust(){
  
  while (ss.available()) {
    if (gps.encode(ss.read())) { 
      
      int Year = gps.date.year();
      byte Month = gps.date.month();
      byte Day = gps.date.day();
      byte Hour = gps.time.hour();
      byte Minute = gps.time.minute();
      byte Second = gps.time.second();

        // Set Time from GPS data string
        setTime(Hour, Minute, Second, Day, Month, Year);
        // Calc current Time Zone time by offset value
        adjustTime(8 * SECS_PER_HOUR);           
      
    }
  }
  
  if (timeStatus()!=timeNotSet){
    if (now() != prevDisplay ){
      prevDisplay = now();
      //SerialClockDisplay();
    }
  }
}

void SerialClockDisplay(){
  // Serial Monitor display of new calculated time - 
  // once adjusted GPS time stored in now() Time Library 
  // calculations or displays can be made.
  
  if (hour() < 10) Serial.print(F("0"));
        Serial.print(hour());
        Serial.print(F(":"));
  if (minute() < 10) Serial.print(F("0"));
        Serial.print(minute());
        Serial.print(F(":"));
  if (second() < 10) Serial.print(F("0"));
        Serial.print(second()); 
        
        Serial.print(" ");
  
  if (day() < 10) Serial.print(F("0"));      
        Serial.print(day());
        Serial.print(F("/"));
  if (month() < 10) Serial.print(F("0"));
        Serial.print(month());
        Serial.print(F("/"));
        Serial.println(year());
}
