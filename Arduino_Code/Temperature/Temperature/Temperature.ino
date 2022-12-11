#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS A1
const int numReadingsTemp = 4;
float readingsTemp[numReadingsTemp];      // the readings from the analog input
int readIndexTemp = 0;              // the index of the current reading
float totalTemp = 0;                  // the running total
float averageTemp = 0;                // the average
float temp_adc_val_Temp;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
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
  print_value(temp_adc_val_Temp,readIndexTemp,averageTemp);
  delay(1);        // delay in between reads for stability
}
void loop(void)
{ 
  TempRead();
  delay(1000);
}
void print_value(float temp_1, int readIn_1,float Avg_1){
  Serial.print("Temperature_2 = ");
  Serial.println(temp_1);
  Serial.print("readIndex_2:");
  Serial.println(readIn_1);
  Serial.print("AVG_2:");
  Serial.println(Avg_1);
}
