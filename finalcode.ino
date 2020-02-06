#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "Wire.h"        
#include <BMP180.h>
#include "Adafruit_BMP085.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

#define DHTPIN            D7
#define WLAN_SSID       "srujan"
#define WLAN_PASS       "qwertyuiop"
#define FIREBASE_HOST "dhtforfbandadafruit.firebaseio.com"
#define FIREBASE_AUTH "YT1x0uy0vgGnXo91HJZUXt12LIVmhDzB3bmB8Rxk"

#define analogpin         D0

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "Smartypro"
#define AIO_KEY         "43310029fa08414fa7f8c8a30937b024"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temprature");
Adafruit_MQTT_Publish Pressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pressure");
Adafruit_MQTT_Publish Rainfallintensity  = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/rain-fall-intensity");

#define DHTTYPE           DHT11      

DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void MQTT_connect();

BMP180 myBMP(BMP180_ULTRALOWPOWER);

void setup() {
  Serial.begin(115200);

  delay(10);
  
  pinMode(A0, INPUT);
  
  Serial.println(F("Adafruit MQTT demo"));

  
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
   while (myBMP.begin(D5, D3) != true) //sda, scl
  {
    Serial.println(F("Bosch BMP180/BMP085 is not connected or fail to read calibration coefficients"));
    delay(5000);
  }
  
  Serial.println(F("Bosch BMP180/BMP085 sensor is OK"));

  
  dht.begin();
  
  uint32_t x = 0;
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
}
int n = 0; 
//int sensorValue;
void loop() {
  
   MQTT_connect();

  delay(500);

 // Serial.print("Rainfall "); 
 // Serial.print(digitalRead(analogpin));
  if(digitalRead(analogpin)==1){
      Serial.println("It's not raining");
  }
  else{
     Serial.println("It's  raining");
  }
//  Serial.print("...");
int sensorValue = digitalRead(analogpin);
Rainfallintensity.publish(sensorValue);
//  if ( Rainfallintensity.publish(sensorValue)) {
  //  Serial.println(F("OK!"));
 // } else {
  //  Serial.println(F("FAILED"));
  //}
 
  float h = dht.readHumidity();

  float t = dht.readTemperature();
 
 // Check if any reads failed and exit early (to try again).
 //  if (isnan(h) || isnan(t)) {
 //   Serial.println("Failed to read from DHT sensor!");
 //   return;
 // }
  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.println("°C");
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.println("%");
  Humidity.publish(h);
  Temperature.publish(t);
 // if (! Humidity.publish(h)) {
 //   Serial.println(F("Failed"));
 // } else {
 //   Serial.println(F("OK!"));
 // }
 // if (! Temperature.publish(t)) {
 //   Serial.println(F("Failed"));
 // } else {
 //   Serial.println(F("OK!"));
 // }


 Serial.print(F("Pressure = "));
 float val=(myBMP.getPressure()/1000.00);
 Serial.print(val);   
 Serial.println(F("hPa"));
 Pressure.publish(val);
// if (! Pressure.publish(val)) {
//   Serial.println(F("Failed"));
//  } else {
//    Serial.println(F("OK!"));
//  }
Serial.println("-----------------------------");
Firebase.setFloat("TEMPERATURE",t); 
Firebase.setFloat("HUMIDITY",h);
Firebase.setFloat("PRESSURE",val);
Firebase.setFloat("RAINFALL",sensorValue); 


  
  if (Firebase.failed()) { 
      Serial.print("setting /number failed:"); 
      Serial.println(Firebase.error());   
      return; }
  delay(1000);  
}
 void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { 
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  }
