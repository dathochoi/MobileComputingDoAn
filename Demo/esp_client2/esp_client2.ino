#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>

// Update these with values suitable for your network.
//#define ssid "Coffee VuonXoai"
//#define password "saicoythuc"
//#define mqtt_server "192.168.1.77"

//#define ssid "wifi"
//#define password "forever147"
//#define mqtt_server "192.168.1.114"

#define ssid "Dan"
#define password "1234567890"
#define mqtt_server "192.168.43.230"

//#define ssid "Bim~"
//#define password "1235813213455"
//#define mqtt_server "192.168.1.128"

#define mqtt_topic_sub "#"
#define mqtt_user ""
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883;

const byte ledPin1 = 4;

// debounce button----------------------------------------
const int buttonPin1 = 12;    // the number of the pushbutton pin
const int water = 13;
int waterstate = 0;

//const int pump =14; // output ra may bom 
const int pumpstate = 5;// output ra may bom

const int sensorTemp = A0; 



int ledState1 = HIGH;         // the current state of the output pin
int buttonState1;             // the current reading from the input pin
int lastButtonState1 = LOW;   // the previous reading from the input pin

long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

long delayTemp= 2000;
long lastTemp =0;


int isAuto= 0;
//-----------------------------------------
WiFiClient espClient12;
PubSubClient client(espClient12);
char msg[50];

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,mqtt_port);
  client.setCallback(callback);

  pinMode (water,INPUT);
  
  pinMode(ledPin1, OUTPUT);
  
  pinMode(buttonPin1, INPUT);

  pinMode(sensorTemp, INPUT);

  digitalWrite(water, LOW);

 // pinMode (pump, INPUT);

  pinMode (pumpstate, OUTPUT);

  
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String str_topic = "";
  for(int i =0;i<19;i++){
    str_topic += topic[i];
  }
  //Serial.print(str_topic);
  if(str_topic=="/Esp1/Light/Command"){
    if ((char)payload[0] == '0') {
        digitalWrite(ledPin1, LOW);   // Turn the LED on (Note that LOW is the voltage level
    }
    if ((char)payload[0] == '1'){
        digitalWrite(ledPin1, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  }
  
 // if (str_topic=="/Esp1/Auto1/Command"){
//    if ((char)payload[0] == '0') {
//        isAuto= 0;
//         Serial.println(isAuto);
//    }
//    if ((char)payload[0] == '1'){
///        isAuto= 1;
//        Serial.println(isAuto);
//    }
//  }
  
 // if (str_topic=="/Esp1/Pump1/Command"){
//    if(isAuto== 0)
//    {
//       if ((char)payload[0] == '0') {
 //       digitalWrite(pumpstate, LOW);
//        Serial.println("auto is false, not run");
//      }
//      if ((char)payload[0] == '1'){
//         digitalWrite(pumpstate, HIGH);
//         Serial.println("auto is false, run");
//      }
//    }
//  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client1",mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("esp", "ESP1_reconnected");
      // ... and resubscribe
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void loop()
{
  
  int reading1 = digitalRead(buttonPin1);
  if (reading1 != lastButtonState1) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading1 != buttonState1) {
      buttonState1 = reading1;
      if (buttonState1 == HIGH) {
        if(digitalRead(ledPin1) == 0){
          digitalWrite(ledPin1, 1);
          client.publish("/Esp1/Light/Status", "ON");
        }
        else if(digitalRead(ledPin1) == 1){
          digitalWrite(ledPin1, 0);
           client.publish("/Esp1/Light/Status", "OFF");
        }
      }
    }

  }
  lastButtonState1 = reading1;

  int reading2= digitalRead(water);
  Serial.println(reading2);
  if(reading2!=waterstate){
    Serial.println(reading2);
    client.publish("/Esp1/Water", String(reading2).c_str(),true);
    waterstate = reading2;
    
    if(reading2 ==LOW){
       Serial.println("low");
        digitalWrite(pumpstate, HIGH);
    }
    else if(reading2 ==HIGH){
        Serial.println("high");
        digitalWrite(pumpstate, LOW);
    }
   
  }


// code them vao khi trang thai water = low thì may bom bom
//khi nuoc cao thì may bom ngung bom
   
  
  

 if( millis()-lastTemp> delayTemp)
 {
  int reading = analogRead(sensorTemp); 

  float temp =reading/9.31;
  client.publish("/Esp1/Temperature",  String(temp).c_str(),true);
  lastTemp = millis();
 }
 
  
 if (!client.connected()) {
  reconnect();
 }
 client.loop();
 
}
