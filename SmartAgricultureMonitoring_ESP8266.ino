// Project Name : Smart Agriculture Monitoring System Using UV Light
// Platform : Blynk Mobile App
#define BLYNK_TEMPLATE_ID "TMPL39IOQY6aX"
#define BLYNK_TEMPLATE_NAME "SMART AGRICULTURE MONITORING SYSTEM"
#define BLYNK_AUTH_TOKEN "u7SY-5U1Tch5hLXknswMS23alzun6cva"

#define BLYNK_PRINT Serial
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>

#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
char auth[] = BLYNK_AUTH_TOKEN;

// Set password to "" for open networks.
char ssid[] = "GS";  // type your wifi name
char pass[] = "12341234";  // type your wifi password                      

#define DHTPIN  D4
#define rainSensor D1
#define moistureSensor D3
#define analogreading A0
#define PIR D2
#define motor D0

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

int moistureState =0;
int lastmoisState=0;
int rainState = 0;
int lastRainState = 0;
int MotorState=0;
int lastPIRState = 0;
int PIRState=0;
unsigned long old =0;
unsigned long current =0;
int interval=0;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
}

BLYNK_WRITE(V0)
{
MotorState = param.asInt();  
if (MotorState==1)
{
digitalWrite(motor,HIGH);
Serial.println("Motor ON");
}
else
{
digitalWrite(motor,LOW);
Serial.println("Motor OFF");
}
} 

void Reading()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  moistureState  = digitalRead(moistureSensor);
  PIRState  = digitalRead(PIR);
  delay(10);
  Serial.println("PIRSTATTE: "+String(PIRState));
  rainState = digitalRead(rainSensor);
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
 
  Blynk.virtualWrite(V2, h);  //V2 is for Humidity
  Blynk.virtualWrite(V1, t);  //V1 is for Temperature


int i = analogRead(analogreading);
i=4095-i;
i= map(i, 0, 4095, 0, 100);
// Serial.println ("AnalogReading: " + String (i) );
Blynk.virtualWrite(V3, i); 
 if (moistureState == 1 && lastmoisState == 0) {
  Serial.println("Blynk Notification, needs water, send notification");
  Blynk.logEvent("WATER","Water your plants");
  lastmoisState = 1;
  delay(1000);
  
} 

   else if (moistureState == 0 && lastmoisState == 1) {
  Serial.println("Water is sufficient");
  lastmoisState=0;
  }
  else if (rainState == 1 && lastRainState == 0) {
  Serial.println("Blynk notification: Its Raining!");
  Blynk.logEvent("RAIN","Raining in Village");
  lastRainState = 1;
  delay(1000);
 
  } 
 else if (rainState == 1 && lastRainState == 1) {
  
  Serial.println("has not been watered yet");delay(1000);
  Blynk.virtualWrite(V0,  LOW);
  }

   else if (PIRState == 1 && lastPIRState == 0) {
  Serial.println("Blynk Notification,PIR Object detected");
 Blynk.logEvent("PIR","Motion Detected");
  lastPIRState = 1;
  delay(1000);
} 

 if (rainState == 0) {
   Serial.println("Its Raining Continuously!");
    lastRainState = 0;
    
  }
 else if (moistureState == 1 && lastmoisState == 1) {
 Serial.println("NO RAIN");

  }

current=millis();
interval=(current-old)/1000;
   if (interval > 60 && PIRState==0)
   {
    interval=0;
    old=current;
     lastPIRState = 0;
     Serial.println("PIR Reactivating...");
     delay(3000);
   }

}
void setup()
{
  Serial.begin(115200);
  Serial.println();
   delay(2000);
  Serial.println("Pleae wait.... DHT Sensor activation");
  dht.begin();
  delay(3000);
  Serial.println("Please wait for Blynk Server connection");
  pinMode(moistureSensor, INPUT);
  pinMode(rainSensor, INPUT);
  pinMode(PIR, INPUT);
  pinMode(analogreading,INPUT);
  pinMode(motor,OUTPUT);
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0,  MotorState);
  timer.setInterval(1000L, Reading);
}

void loop()
{
  Blynk.run(); 
  timer.run(); 
}
