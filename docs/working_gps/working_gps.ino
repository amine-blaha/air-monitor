#include <NeoSWSerial.h>
#include <NMEAGPS.h>

NMEAGPS gps;  //Declaramos el objeto GPS
gps_fix fix;  // all GPS fields, including date/time, location, etc.
uint8_t fixCount;
NeoSWSerial serialgps(4,5);

void setup()
{
 Serial.begin(9600);  //Iniciamos el puerto serie
 serialgps.begin(9600); //Iniciamos el puerto serie del gps

 //Imprimimos en el monitor serial:
 Serial.println( F("Test") );

}

void loop()
{
 // Check for GPS characters and parse them
 if (gps.available( serialgps )) 
 {
  // Once per second, a complete GPS fix structure is ready.  Get it.
  fix = gps.read();

  // Count elapsed seconds
  fixCount++;

  if (fixCount >= 10)
  {
    fixCount = 0; // reset counter

    // adjust from UTC to local time
    if (fix.valid.time)
      adjustTime( fix.dateTime );

    printGPSdata();
  }
 }
}


void printGPSdata()
{
  Serial.print( F("Latitud/Longitud: ") ); 
  if (fix.valid.location) {
    Serial.print( fix.latitude(), 5 ); 
    Serial.print( F(", ") ); 
    Serial.print( fix.longitude(), 5 );
  }
  Serial.println();

  Serial.print( F("Date: ") );
  if (fix.valid.date) {
    Serial.print( fix.dateTime.date  ); Serial.print('/'); 
    Serial.print( fix.dateTime.month ); Serial.print('/');
    Serial.print( fix.dateTime.year  );
  }

  Serial.print( F(" Time: ") );
  if (fix.valid.time) {
    Serial.print( fix.dateTime.hours ); Serial.print(':'); 
    if (fix.dateTime.minutes < 10)
      Serial.print( '0' );
    Serial.print(fix.dateTime.minutes); Serial.print(':');
    if (fix.dateTime.seconds < 10)
      Serial.print( '0' );
    Serial.print( fix.dateTime.seconds ); 
    Serial.print('.');
    if (fix.dateTime_cs < 10)
      Serial.print( '0' );
    Serial.print( fix.dateTime_cs );
  }
  Serial.println();

  Serial.print( F("Altitud (metros): ") );
  if (fix.valid.altitude)
    Serial.print( fix.altitude() );
  Serial.println();

  Serial.print( F("Rumbo (grados): ") );
  if (fix.valid.heading)
    Serial.print( fix.heading() ); 
  Serial.println();

  Serial.print( F("Velocidad(kmph): ") );
  if (fix.valid.speed)
    Serial.print( fix.speed_kph() );
  Serial.println();

  Serial.print( F("Satelites: ") );
  if (fix.valid.satellites)
    Serial.println( fix.satellites );
  Serial.println();
  Serial.println();
}


//--------------------------

void adjustTime( NeoGPS::time_t & dt )
{
  NeoGPS::clock_t seconds = dt; // convert date/time structure to seconds

  // Offset to the local time
  const int32_t          zone_hours  = +8L;
  const NeoGPS::clock_t  zone_offset = zone_hours * NeoGPS::SECONDS_PER_HOUR;
  seconds += zone_offset;

  dt = seconds; // convert seconds back to a date/time structure

} // adjustTime
