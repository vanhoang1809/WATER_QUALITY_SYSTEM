#include <SoftwareSerial.h>
 
#define LED1  13 
String inString = "";    // string to hold incoming charaters
String MyMessage = ""; // Holds the complete message
  
SoftwareSerial loraSerial(2, 3); // TX, RX
int temperature;
int humidity; 

byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 0.2 ; // updated every 1 second
int Secs = 0;

void setup() {
  pinMode(LED1, OUTPUT);
  Serial.begin(9600);
  loraSerial.begin(9600);  
}
void sendMess(){
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    Secs = Secs + 1;
    Serial.println(Secs);
    if ( Secs >= 11)
    {
      Secs = 0;
    }
    if ( (Secs >= 1 ) && (Secs <= 5))
    {
    String message1 = "20";
    sendMessage(message1, MasterNode, Node2);
    Serial.println("Doan Ma Gui Client1:"+message1+"+Masternode:0x"+String(MasterNode, HEX)+"+Reciver Node:0x"+String(Node2, HEX));
    }
    if ( (Secs >= 6 ) && (Secs <= 10))
    {
      loraSerial.end();
    }
    previoussecs = currentsecs;
  }
}
void loop() {
    sendMess();
     while (loraSerial.available())
    {
      int inChar = loraSerial.read();
      inString += (char)inChar;
      MyMessage = inString;       
    }
    inString = "";     

    String t = getValue(MyMessage, ',', 0); // Temperature
    String h = getValue(MyMessage, ',', 1); // Humidity
    String ds = getValue(MyMessage, ',', 2);
    Serial.println("Temperature:"+t);
    Serial.println("Humidity:"+h);
    Serial.println("Humidity:"+ds);
    delay(300);
}
void sendMessage(String outgoing, byte MasterNode, byte otherNode) {               // start packet
  loraSerial.write(otherNode);              // add destination address
  loraSerial.write(MasterNode);             // add sender address       
  loraSerial.print(outgoing);                 // add payload                     
}
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";}
