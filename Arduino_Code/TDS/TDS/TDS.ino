#include <OneWire.h>
#include <DallasTemperature.h>

//TEMPERATURE
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

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void)
{
  Serial.begin(9600);
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
        avgTemp=34;
        }
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(avgTemp-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
        print_value(avgTemp,tdsValue,averageVoltage,readIndex);
//   }
}
void loop()
{ 
  TempRead();
  delay(100);
}

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
void print_value(float temp_1, int readIn_1,float Avg_1,int readIn){
  Serial.print("ReadIndex = ");
  Serial.println(readIn);
  Serial.print("AVGTemperature = ");
  Serial.println(temp_1);
  Serial.print("tds:");
  Serial.println(readIn_1);
  Serial.print("Voltage:");
  Serial.println(Avg_1);
  Serial.println();
}
