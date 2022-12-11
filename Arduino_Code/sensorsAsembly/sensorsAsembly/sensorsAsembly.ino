// init Libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

// TURBIDITY SENSOR
int turbidity_sensorPin = A0;
float turbidity_volt;
float ntu;

//TEMPERATURE SENSOR
#define ONE_WIRE_BUS A1
const int numReadingsTemp = 4;
float readingsTemp[numReadingsTemp];      // the readings from the analog input
int readIndexTemp = 0;              // the index of the current reading
float totalTemp = 0;                  // the running total
float averageTemp = 0;                // the average
float temp_adc_val_Temp;

//TDS SENSORS
#define TdsSensorPin A5
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
int analogReadTDS=0;
float averageVoltage = 0,tdsValue = 0;

// PH SENSOR
#define pHsensorPin A3          //pH meter Analog output to Arduino Analog Input 0
unsigned long int avgValueForPH;  //Store the average value of the ph sensor feedback
int pHbuffer[10];
float phValue;

// SENDER LORA
String turnOn = "";

String MyMessage = "";
String inString = "";

// LED infor
#define LED1 13  

// init Libraries for Temperature sesor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// init Libraries for Sender Lora
SoftwareSerial loraSerial(2, 3); // TX, RX

void setup(void)
{
  Serial.begin(9600);
  loraSerial.begin(9600);
  sensors.begin();
  pinMode(TdsSensorPin,INPUT);
  for (int thisReading = 0; thisReading < numReadingsTemp; thisReading++) {
    readingsTemp[thisReading] = 0;
  }
}
void TempRead(){
  sensors.requestTemperatures(); 
  temp_adc_val_Temp=sensors.getTempCByIndex(0);
  totalTemp = totalTemp - readingsTemp[readIndexTemp]; 
  // read from the sensor:
  readingsTemp[readIndexTemp] = temp_adc_val_Temp;
  // add the reading to the total:
  totalTemp = totalTemp + readingsTemp[readIndexTemp];
  // advance to the next position in the array:
  readIndexTemp = readIndexTemp + 1;
  if (readIndexTemp >= numReadingsTemp) {
     readIndexTemp = 0;
  }
  averageTemp = totalTemp / numReadingsTemp;
  TDSRead(averageTemp,readIndexTemp);
  delay(1);
}
void TDSRead(float avgTemp,int readIndex){
  static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogReadTDS=analogRead(TdsSensorPin);
     analogBuffer[analogBufferIndex] =  analogReadTDS;   //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
      if(avgTemp<=0){
        Serial.println("Temperature Sensor Error!Set Temp=25C");
        avgTemp=25;
        }
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(avgTemp-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
        TurbidityRead(avgTemp,tdsValue,averageVoltage,readIndex);
  delay(1);
//   }
}
void TurbidityRead(float avgTemp, float tdsValue,float avg_volt_TDS,int readIndex){
  turbidity_volt = 0;
  for(int i=0; i<800; i++)
  {
    turbidity_volt += ((float)analogRead(turbidity_sensorPin)/875)*5;
  }
  turbidity_volt = turbidity_volt/800;
  turbidity_volt = round_to_dp(turbidity_volt,2);
  if(turbidity_volt < 2.5){
    ntu = 3000;
  }else{
    ntu = -1120.4*square(turbidity_volt)+5770.3*turbidity_volt-4352.2; 
  }
  phRead(avgTemp,tdsValue,avg_volt_TDS,readIndex,ntu,turbidity_volt);
  // Serial.print(turbidity_volt);
  // Serial.print(" V");
  // Serial.println();
  
  // Serial.print(ntu);
  // Serial.print(" NTU");
  // Serial.println();
  // delay(500);
  delay(1);
}
void phRead(float avgTemp, float tdsValue,float avg_volt_TDS,int readIndex, float ntuValue,float turbidity_volt){
  for(int i=0;i<10;i++)        //Get 10 sample value from the sensor for smooth the value
  { 
    pHbuffer[i]=analogRead(pHsensorPin);
  //  Serial.println(pHbuffer[i]);
    delay(10);
  }
  avgValueForPH=0;
  for(int i=2;i<8;i++)                    
    avgValueForPH+=pHbuffer[i];
  float Voltage=(float)avgValueForPH*5/1024/6;
  if(Voltage >= 1.8 && Voltage <=1.9){
    Voltage+=0.15;
  }
  // Serial.print("    Voltage:");  
  // Serial.print(Voltage,2);     //convert the analog into millivolt
  phValue=3.5*Voltage;         //convert the millivolt into pH value

  SendMessLora_to_Master(avgTemp,tdsValue,avg_volt_TDS,readIndex,ntuValue,turbidity_volt,phValue,Voltage);
  delay(1);
}
void SendMessLora_to_Master(float avgTemp, float tdsValue,float avg_volt_TDS,int readIndex,float ntuValue,float avg_volt_Turbidity,float phValue, float avg_volt_pH){
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

    int Val = MyMessage.toInt();
    if (Val == 20)
    {
      turnOn = turnOn + avgTemp + "," + tdsValue+ "," + ntuValue+"," + phValue;
      delay(10);
      loraSerial.print(turnOn);
      delay(10);
      Serial.println("Received from: 0x" + String(sender, HEX));
      Serial.println("Sent to: 0x" + String(recipient, HEX));    
      Serial.println("Mess sender:" +MyMessage);
      Serial.println(turnOn);
      Serial.println("");
    }
    turnOn = "";
}

// void print_value(float avgTemp, float tdsValue,float avg_volt_TDS,int readIndex,float ntuValue,float avg_volt_Turbidity,float phValue, float avg_volt_pH){
//   Serial.print("ReadIndex = ");
//   Serial.println(readIndex);
//   Serial.print("AVG_Temperature = ");
//   Serial.print(avgTemp);
//   Serial.print(" *C");
//   Serial.println();
//   Serial.print("TDS Sensor Value = ");
//   Serial.print(tdsValue);
//   Serial.print("   - Voltage_TDS = ");
//   Serial.println(avg_volt_TDS);
//   Serial.print("Turbidity Sensor Value = ");
//   Serial.print(ntuValue);
//   Serial.print("   - Voltage_Turbidity = ");
//   Serial.println(avg_volt_Turbidity);
//   Serial.print("pH Sensor Value = ");
//   Serial.print(phValue);
//   Serial.print("   - Voltage_pH = ");
//   Serial.println(avg_volt_pH);
//   Serial.println();
// }
// getMedianNum for TDS sensor
int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}
// Round for Turbidity sensor
float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}

void loop()
{ 
  TempRead();
  delay(3000);
}
