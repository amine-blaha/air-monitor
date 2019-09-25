 #include "MQ135.h"
#include <string.h>
#include <stdlib.h>
#include <Ethernet.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
const int mq135Pin = 0;     // Pin sur lequel est branché de MQ135

static const int RXPin = 5, TXPin = 4;
static const uint32_t GPSBaud = 9600;

#define MAX 100 

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

char ppm_value[MAX];
MQ135 gasSensor = MQ135(mq135Pin);  // Initialise l'objet MQ135 sur le Pin spécifié

void setup()
{
    Serial.begin(115200);     // Initialise le port série à 9600 bps 
    float rzero = gasSensor.getRZero();
    ss.begin(GPSBaud);
    Serial.println(TinyGPSPlus::libraryVersion());
    Serial.println();
    Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
    Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
    Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
    Serial.print("R0: ");
    Serial.println(rzero);  // Valeur à reporter ligne 27 du fichier mq135.h après 48h de préchauffage
   
}

void loop()
{
    float ppm = gasSensor.getPPM();
    Serial.print("A0: ");
    Serial.print(analogRead(mq135Pin));
    Serial.print(" ppm CO2: ");
    Serial.println(ppm);

    delay(3000);        // Actualise la mesure chaque seconde
    Serial.print("\n");
}
