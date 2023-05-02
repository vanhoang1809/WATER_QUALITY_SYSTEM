#include <SoftwareSerial.h>
   
#define LED1 13  

#include "DHT.h"            
const int DHTPIN = 9;      
const int DHTTYPE = DHT11;  

DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial loraSerial(2, 3); // TX, RX
 
String turnOn = "";
String turnOff = "";

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte MasterNode = 0xFF;
byte Node2 = 0xCC;

int humidity;
int temperature;

String MyMessage = "";
String MessSender = "";

String inString = "";
void setup() {
  Serial.begin(9600);
  loraSerial.begin(9600);
  dht.begin();
   
}
 
void loop(){
    humidity = random(1,10);
    temperature = random(10, 20);
    // read packet header bytes:
    byte sender = loraSerial.read();
    int recipient = loraSerial.read();          // recipient address
    while (loraSerial.available())
    {
      int inChar = loraSerial.read();
      inString += (char)inChar;
      MyMessage = inString;       
    }
    inString = ""; 
    Serial.println("Received from: 0x" + String(sender, HEX));
    Serial.println("Sent to: 0x" + String(recipient, HEX));    
    Serial.println("Mess sender:" +MyMessage);
    int Val = MyMessage.toInt();
    if (Val == 20)
    {
      turnOn = turnOn + temperature + "," + humidity+ "," + humidity;
      delay(100);
      loraSerial.print(turnOn);
      delay(500);
      Serial.println(turnOn);
      turnOn = "";
    }
    
}
