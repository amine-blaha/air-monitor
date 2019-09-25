#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "MQ135.h"
/*
   Lien pour les librairie:
   
   MQ135.H
   https://github.com/GeorgK/MQ135

   TinyGPSPlus
   https://github.com/mikalhart/TinyGPSPlus/blob/master/src/TinyGPS%2B%2B.h
   
*/



static const int RXPin = 5, TXPin = 4;
static const uint32_t GPSBaud = 9600;

// Definir le numero du PIN ou le capteur MQ135 est branché
const int mq135Pin = 0;

// Initialisation de l'objet MQ135 sur le Pin spécifié
MQ135 gasSensor = MQ135(mq135Pin);


// Declaration du module GPS
TinyGPSPlus gps;

// La connextion au serial ou le GPS est branché
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  
  Serial.begin(115200);
  Serial.println("ppm,longitude,latitude");
  ss.begin(GPSBaud);
  
  // Accordage de la résistance pour le capteur MQ135 pour détecter le CO
  float rzero = gasSensor.getRZero();
}

void loop()
{
  // Dans cette partie on va pas se servir du serial moniteur de Arduino,
  // mais celui d'un autre logiciel externe nommé Tera Term dans le but de pouvoir
  // enregistrer la valeurs affichées par le serial moniteur dans un fichier CSV
  // pour un futur traitement.

  // On affiche en ordre:
  //  la valeur du CO
  //  la latitude
  //  ma mongitude

  // Tout en affichant une virgule ',' entre ces valeur pour garantir le format
  // d'un fichier CSV

  
  // Avoir la concentration du CO détéctée par le capteur en PPM et l'afficher
  float ppm = gasSensor.getPPM();
  Serial.print(analogRead(mq135Pin));

  Serial.print(",");

  // Avoir la latitude obtenue par le GPS et l'afficher
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  
  Serial.print(",");

  // Avoir la latitude obtenue par le GPS et l'afficher
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);

  Serial.println("");

  // Affichage du temps
  //printDateTime(gps.date, gps.time);

  
  smartDelay(1000);

  // Ici grace à la fonction charsProcesses prédéfinit on peut savoir si le GPS 
  // est branché ou non
  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

// la fonction smartDelay est une fonction 
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
    
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    //for (int i=flen; i<len; ++i)
      //Serial.print(',');
  }
  //Serial.print(',');
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
  smartDelay(0);
}
