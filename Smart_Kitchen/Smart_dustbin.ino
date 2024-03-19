

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
#define TRIGGER D7
#define ECHO D6
#include <Servo.h> 
long duration1;
int distance1;
 
Servo myservo; 
const int TRIGGE=D0;  //D4
const int ECH = D1;  //D3
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"



/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Honor8"
#define WLAN_PASS       "Huaweihonor8"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "yasar_ahamed"
#define AIO_KEY         "c0452f59a4e34688b95b0e36414ebaaf"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish photo = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/smart_bin");
//Adafruit_MQTT_Publish photoce = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/smart_container");
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/LPG_montoring");


// Setup a feed called 'onoff' for subscribing to changes.
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  
  Serial.begin(115200);
  //sevo with dustbin open
  myservo.attach(D3);
pinMode(TRIGGE, OUTPUT); // Sets the trigPin as an Output
pinMode(ECH, INPUT);

  //Setup for ultrasonic
// pinMode(sensor,INPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  
//  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
 // mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.


  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

 

  // Now we can publish stuff!
  //servo open
  long duration1, distance1;
  digitalWrite(TRIGGE, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGE, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGE, LOW);
  duration1 = pulseIn(ECH, HIGH);
  distance1 = (duration1/2) / 29.1;
  
  Serial.print(distance1);
  Serial.println("Centimeter:");
  int pos;

  if(distance1>50)
  {
                            // in steps of 1 degree 
    myservo.write(180);              // tell servo to go to position in variable 'pos' 
    //delay(15);                       // waits 15ms for the servo to reach the position 
  }   
if(distance1<50)
  {
                               // in steps of 1 degree 
    myservo.write(0);              // tell servo to go to position in variable 'pos' 
    //delay(15);                       // waits 15ms for the servo to reach the position 
}        

delay(250);
  MQTT_connect();

  //For smart bin detection
int duration, distance;
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print(distance);
  Serial.println("Centimeter:");
 // delay(100);

    
//   For smart Bin sending data
  Serial.print(F("\nSending photocell val "));
  Serial.print(distance);
  Serial.print(" cm");
  Serial.print("...");
    if (! photo.publish(distance)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  delay(500);
  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}

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
       delay(1000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
