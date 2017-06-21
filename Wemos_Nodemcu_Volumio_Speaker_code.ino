/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

/********************************************************************
   Modified by Sid for Sid's E Classroom
   https://www.youtube.com/c/SidsEClassroom
 ********************************************************************/

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//Change the pins according to the choice of your board

#define Play D1 //Define the pins connected to the Play Button
#define Volup D2 //Define the pins connected to the Volume Up Button
#define Voldown D3 //Define the pins connected to the Volume Down Button
#define Tracknext D4 //Define the pins connected to the Next Track Button
#define Trackprevious D5 //Define the pins connected to the Previous Track Button
#define Switchoff D6 //Define the pins connected to Switchoff the Speaker
#define Boot D7 //Define the pins connected to Boot the Speaker

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "*******"  //Change accordingly
#define WLAN_PASS       "********" //Change accordingly

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "********"
#define AIO_KEY         "**************************"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
// Setup a feed called 'Speaker' for subscribing to changes.
Adafruit_MQTT_Subscribe Speaker = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Speaker");


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(Play, OUTPUT);
  pinMode(Volup, OUTPUT);
  pinMode(Voldown, OUTPUT);
  pinMode(Tracknext, OUTPUT);
  pinMode(Trackprevious, OUTPUT);
  pinMode(Switchoff, OUTPUT);
  pinMode(Boot, OUTPUT);

  
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
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
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Speaker);

}

uint32_t x = 0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {


    if (subscription == &Speaker) {
      Serial.print(F("Got from Speaker Feed: "));

      Serial.println((char *)Speaker.lastread);

      if (strcmp((char *)Speaker.lastread, "play") == 0) {
        digitalWrite(Play, HIGH);
        delay(10);
        digitalWrite(Play, LOW);
      }
      if (strcmp((char *)Speaker.lastread, "stop") == 0) {
        digitalWrite(Play, HIGH);
        delay(10);
        digitalWrite(Play, LOW);
      }
      if (strcmp((char *)Speaker.lastread, "volume up") == 0) {
        digitalWrite(Volup, HIGH);
        delay(10);
        digitalWrite(Volup, LOW);
      }
      if (strcmp((char *)Speaker.lastread, "volume down") == 0) {
        digitalWrite(Voldown, HIGH);
        delay(10);
        digitalWrite(Voldown, LOW);
      }
      if (strcmp((char *)Speaker.lastread, "next") == 0) {
        digitalWrite(Tracknext, HIGH);
        delay(10);
        digitalWrite(Tracknext, LOW);
      }
      if (strcmp((char *)Speaker.lastread, "previous") == 0) {
        digitalWrite(Trackprevious, HIGH);
        delay(10);
        digitalWrite(Trackprevious, LOW);
      }
      if (strcmp((char *)Speaker.lastread, "turn off") == 0) {
        digitalWrite(Switchoff, HIGH);
        delay(10);
        digitalWrite(Switchoff, LOW);
      }
      if (strcmp((char *)Speaker.lastread, "turn on") == 0) {
        digitalWrite(Boot, LOW);
        delay(10);
        digitalWrite(Boot, HIGH);
      }
    }

  }
}
// ping the server to keep the mqtt connection alive
// NOT required if you are publishing once every KEEPALIVE seconds
/*
  if(! mqtt.ping()) {
  mqtt.disconnect();
  }
*/

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
