
#include <Arduino.h>
#include <Wire.h> //don't know if we need this library (most likely not) if buggy remove for troubleshooting (mostly likely removing this)
#include <LiquidCrystal_I2C.h> //library to setup I2C adapted Liquid Crystal LCD screen
#include <OneWire.h> //library for data bus communication on temp sensor
#include <DallasTemperature.h> //library to pass this oneWire reference to DallasTemperature
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <Hash.h>
#include "OnPage.h" //webpage library


LiquidCrystal_I2C lcd(0x27, 16 ,2); //SDA on D2/GPIO4 (hence 4) and SLA on D1/GPIO5 (idk why it says 20)


int page; //declare varaible for breaking while loop to turn off processes on /On server
int lastMySwitchState;
int thisState = 0;
int dtime = 1;
int raw = 0;
int Vin = 5;
int temp_input;
int salt_input;
float Vout = 0;
float R1 = 1000;
float R2 = 0;
float buff = 0;
float avg = 0;
int samples = 50;

const char* ssid = "renaldo"; //Enter Wi-Fi SSID (in quotes)
const char* password =  "smokechan"; //Enter Wi-Fi Password (in quotes)

String on_page = OnPage;

uint32_t temperature = 0;

const int analogPin = A0; //A0 on ESP8266 (for salinity test)
const int heater_pin = 2; // D4 on ESP8266 (used for pool heater)
const int led_pin = 3; // RX on ESP8266 (used for led test)
const int temp_pin = 0; // D3 on ESP8266 (used for temp sensor)
const int buttonPin = 14;// D5 on ESP8266 (button for light bulb)
const int copperR = 12; //D6 on ESP8266 (copper electrode with resistor)
const int copperNoR = 13; //D7 on ESP8266 (copper electrode without resistor)
const int led_sensor = 15; //D8 on ESP8266

OneWire oneWire(temp_pin); //setup a oneWire instance to communicate with the DS18B20 device
DallasTemperature sensors(&oneWire); //pass this oneWire reference to DallasTemperature

ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void classify(){
  if (avg > 2000) {
      Serial.println("This is demineralised water.");
      lcd.setCursor(0,1);
      lcd.print("   Demi water   ");
    }
  else if (avg > 1000) {
      Serial.println("This is fresh/tap water.");
      lcd.setCursor(0,1);
      lcd.print("    Tap water   ");
      
    }
  else if (avg > 190) {
      Serial.println("This is brackish water.");
      lcd.setCursor(0,1);
      lcd.print(" Brackish water ");
      
    }
  else if (avg < 190) {
      Serial.println("This is seawater.");
      lcd.setCursor(0,1);
      lcd.print("   Sea water    ");
      
    }
}

void latch(){
  thisState = digitalRead(buttonPin);
  if(thisState != lastMySwitchState)
    { 
      //update the switch state
      lastMySwitchState = thisState;  
      //"HIGH condition code"
      //switch goes from LOW to HIGH
      if(thisState == HIGH)    
      {
      //LED on pin D4 is Push ON, Push OFF
      Serial.println("light bulb in on");
      digitalWrite(heater_pin,!digitalRead(heater_pin));
      classify(); 
      }
      
      if(not thisState){
      }
    }
}


void setup() {
    Serial.begin(115200);
    //Serial.begin(9600);

    //USE_SERIAL.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        //Serial.flush();
        delay(1000);
    }
    
  pinMode(led_sensor,INPUT);
  pinMode(buttonPin,INPUT);
  pinMode(led_pin, OUTPUT); //set led pin to output mode to activate led
  digitalWrite(led_pin, LOW); //initialize to LOW (or off or 0V etc) needed for testing in On function
  pinMode(heater_pin, OUTPUT); //set led pin to output mode to activate led
  digitalWrite(heater_pin, LOW); //initialize to LOW (or off or 0V etc) needed for testing in On function
  pinMode(copperNoR,OUTPUT); // define ports 8 and 7 for AC 
  pinMode(copperR,OUTPUT);
  
  lcd.init(); //initialize lcd
  lcd.backlight(); //turn on backlight
  lcd.print("Hi there !"); //print friendly message


    WiFiMulti.addAP(ssid, password);

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
        Serial.println("waitinggg");
    }

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.setCursor(0,1); //Initialize next row in LCD screen
    lcd.print(WiFi.localIP()); //Print local IP to LCD screen
    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }
    // handle index
    server.on("/", []() {
        // send index.html
        server.send(200, "text/html", on_page);
        lcd.clear();
    });

    server.begin();

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);

}

void loop() {
    webSocket.loop();
    server.handleClient();
    temp();
    salinity();
    light();
    
    /*String jsonString = "";                           // create a JSON string for sending data to the client
    StaticJsonDocument<200> doc;                      // create a JSON container
    JsonObject object = doc.to<JsonObject>();         // create a JSON Object
    object["rand1"] = random(100);                    // write data into the JSON object -> I used "rand1" and "rand2" here, but you can use anything else
    object["rand2"] = random(100);
    serializeJson(doc, jsonString);                   // convert JSON object to string
    Serial.println(jsonString);                       // print JSON string to console for debug purposes (you can comment this out)
    webSocket.broadcastTXT(jsonString);               // send JSON string to clients
    */
}

void temp(){
  sensors.requestTemperatures(); //Send command to get temperature from temp senor (DS18B20)
  float temp = sensors.getTempFByIndex(0); //acquire and save reading in fahrenheit (hence "TempF" portion) to float (for decimal places) to temp variable
  //float temp = rand() % 77 + 74;  //range 50 to 96
  String tempa = "t" + String(temp, 2);
  int temp_length = tempa.length() + 1;
  char acid[temp_length];
  tempa.toCharArray(acid, temp_length);
  webSocket.broadcastTXT(acid);
  //Serial.print("temp = ");
  //Serial.println(temp); //print temp reading to Serial Monitor
  lcd.setCursor(0,0);
  lcd.print("  Temp = ");
  lcd.print(temp); //print temp reading to LCD screen
  //latch();
  if(temperature > temp){
    digitalWrite(heater_pin, HIGH);
    
  }
  else{
    digitalWrite(heater_pin, LOW);
  }
  delayMicroseconds(dtime);
}

void salinity(){
  float tot = 0;
  for (int i =0; i<samples; i++) {
    digitalWrite(copperR,HIGH);
    digitalWrite(copperNoR,LOW);
    delay(dtime/4);
    digitalWrite(copperR,LOW);
    digitalWrite(copperNoR,HIGH);
    delayMicroseconds(dtime/4);
    raw = analogRead(analogPin);
    if(raw){
      buff = raw * Vin;
      Vout = (buff)/1024.0;
      buff = (Vin/Vout) - 1;
      R2= R1 * buff;
      tot = tot + R2;
    }
  }
  //Serial.print("Average: ");
  //Serial.println(avg);
  avg = tot/samples;
  String salty = "s" + String(avg, 1);
  int salty_length = salty.length() + 1;
  char mdma[salty_length];
  salty.toCharArray(mdma, salty_length);
  webSocket.broadcastTXT(mdma);
  //Serial.print("The average resistance is: ");
  //Serial.print(avg);
  //Serial.println(" Ohm");
  lcd.setCursor(0,1); //Initialize next row in LCD screen
  lcd.print("Ave Ohm = ");
  lcd.print(avg); //print temp reading to LCD screen
}

void light(){
  if(digitalRead(led_pin)){
       webSocket.broadcastTXT("On");
  }
  else{
        webSocket.broadcastTXT("Off");
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            
            if(payload[0] == 't') {
                // we get temp data

                // decode temp data
                temperature = (uint32_t) strtol((const char *) &payload[1], NULL, 10);
                /*for(int i=0; i<length; i++){
                  Serial.printf((const char*)payload[i]);
                  temp_input+=(char)payload[i];
                }*/
                Serial.printf("Set Temperature = %d\n", temperature);
            }
            else if(payload[0] == '1'){
              Serial.printf("Toggle On");
              digitalWrite(led_pin, HIGH);
              webSocket.broadcastTXT("On");
            }
            else if(payload[0] == '0'){
              Serial.printf("Toggle Off");
              digitalWrite(led_pin, LOW);
              webSocket.broadcastTXT("Off");
            }

            break;
    }

}
