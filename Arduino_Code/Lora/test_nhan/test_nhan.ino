#include <SoftwareSerial.h>
// EsP8266 Wifi package
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// SSID wifi and Password
const char *ssid = "Hanh";
const char *password = "0905128744";

// Declaration broker and information
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883 //port của hivemq
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_TOPIC_PUBLIC_SENSORS "Esp32/sensors"
// #define MQTT_TOPIC_PUBLIC_FAN "Esp32/fan"
#define MQTT_TOPIC_SUBSCRIBE "Esp32/control"


// Led Pin
#define LED1 5
String inString = "";    // string to hold incoming charaters
String MyMessage = ""; // Holds the complete message
  
SoftwareSerial loraSerial(15, 13); // TX, RX


//Declaration Master - Slave
byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;


// Tracks the time since last event fired from Slave Node 1
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 0.2 ; // updated every 200ms
int Secs = 0;

//Interup time for publish to broker
unsigned long previousMillis2 = 0;
const long interval2 = 300;

//Declaration message Receiver
String message = "";

//Declaration message Publish
String SendWebJsonSensors = "";
String SendWebJsonFan = "";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Setup wifi connection
void setup_wifi()
{
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
}
// Setup infor connect to broker
void connect_to_broker()
{
    while (!client.connected())
    {
        Serial.print("Đang kết nối MQTT...");
        String clientId = "nodemqtt32";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("Đã kết nối!!!");
            client.subscribe(MQTT_TOPIC_SUBSCRIBE);
        }
        else
        {
            Serial.print("Đã lỗi, rc=");
            Serial.print(client.state());
            Serial.println(" Đợi kết nối sau 5 giây");
            delay(5000);
        }
    }
}
// void setup callback subscribe
void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.println("------------Nhận tin nhắn từ topic----------");
    Serial.print("topic: ");
    Serial.println(topic);
    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }
    Serial.print("message: ");
    Serial.println(message);
}
// Void setup
void setup() {
  pinMode(LED1, OUTPUT);
  Serial.begin(9600);
  digitalWrite(LED1,LOW);
  loraSerial.begin(9600);  
  delay(1);
  setup_wifi();
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
    //Thiết lập địa chỉ broker và port của hivemq
    client.setServer(MQTT_SERVER, MQTT_PORT);
    /*Đăng ký hàm setcallback trong thư viện pubsubclient sẽ được gọi 
  khi thư viện nhận được giá trị mới thay đổi trong broker cho 
  dữ liệu mà topic được subscribe*/
    client.setCallback(callback);
    connect_to_broker();
}
void sendDataMQTT(String tempValue, String TDSValue,String turbidityValue, String pHValue)
{
    SendWebJsonSensors = "";
    SendWebJsonSensors = "{\"tempValue\":\"" + String(tempValue) + "\"," +
                         "\"TDSValue\":\"" + String(TDSValue) + "\"," + 
                          "\"turbidityValue\":\"" + String(turbidityValue) + "\","+
                           "\"pHValue\":\"" + String(pHValue) + "\"}";
    Serial.print("SendDataMQTT:");
    Serial.println(SendWebJsonSensors);
    
    //Gửi dữ liệu lên broker
    client.publish(MQTT_TOPIC_PUBLIC_SENSORS, SendWebJsonSensors.c_str());
}
void sendMessLora(){
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    Secs = Secs + 1;
    Serial.println(Secs);
    if ( Secs >= 6)
    {
      Secs = 0;
    }
    if ( (Secs >= 1 ) && (Secs <= 5))
    {
    String message1 = "20";
    sendMessageLora(message1, MasterNode, Node2);
    Serial.println("Doan Ma Gui Client1:"+message1+"+Masternode:0x"+String(MasterNode, HEX)+"+Reciver Node:0x"+String(Node2, HEX));
    ReciverMessLora();
    }
    // if ( (Secs >= 6 ) && (Secs <= 10))
    // {
    //   loraSerial.flush();// end Goi Tin   
    // }
    previoussecs = currentsecs;
  }
}
void ReciverMessLora(){
  while (loraSerial.available())
    {
      int inChar = loraSerial.read();
      inString += (char)inChar;
      MyMessage = inString;       
    }
    inString = "";     
//  Get value from void getValue();
    String temperature = getValue(MyMessage, ',', 0); // Temperature
    String TDS = getValue(MyMessage, ',', 1); // Humidity
    String turbidity = getValue(MyMessage, ',', 2); // Test
    String pH = getValue(MyMessage, ',', 3);// Test 

    float f2 = pH.toFloat();
    String pHValue = String(f2, 2); // 3.1
    delay(10);
    printData(temperature,TDS,turbidity,pHValue);
    delay(10);
    digitalWrite(LED1,HIGH);
    delay(500);
    digitalWrite(LED1,LOW);
}

void sendMessageLora(String outgoing, byte MasterNode, byte otherNode) {               // start packet
  loraSerial.write(otherNode);              // add destination address
  loraSerial.write(MasterNode);             // add sender address       
  loraSerial.print(outgoing);                 
}
// Getvalue
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
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
// PrintData to broker
void printData(String tempValue, String TDSValue, String turbidityValue, String pHValue){

  if(tempValue=="" &&TDSValue==""&&turbidityValue==""&&pHValue==""){
    Serial.println(("Lỗi đọc giá trị của Receiver Lora"));
  }else{
    Serial.println("Data:"+tempValue+"-"+TDSValue+"-"+turbidityValue+"-"+pHValue);
    unsigned long currentMillis2 = millis();
    if (currentMillis2 - previousMillis2 >= interval2)
    {
        if (tempValue=="" || TDSValue==""|| turbidityValue==""|| pHValue=="")
        {
            tempValue="nan";
            TDSValue="nan";
            turbidityValue="nan";
            pHValue="nan";
            sendDataMQTT(String(tempValue), String(TDSValue), String(turbidityValue), String(pHValue));
        }
        else
        {
            sendDataMQTT(String(tempValue), String(TDSValue), String(turbidityValue), String(pHValue));
        }
        previousMillis2 = currentMillis2;
    }
  }
}
//Void Main Loop
void loop() {

  delay(2000);
  sendMessLora();
  client.loop();
    
}
