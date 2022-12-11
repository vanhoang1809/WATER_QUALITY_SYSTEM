

const int numReadings = 4;
float readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float average = 0;                // the average
int temp_adc_val;
int inputPin = A1;
float temp_val;

const int numReadings_1 = 4;
float readings_1[numReadings_1];      // the readings from the analog input
int readIndex_1 = 0;              // the index of the current reading
float total_1 = 0;                  // the running total
float average_1 = 0;                // the average
int temp_adc_val_1;
int inputPin_1 = A2;
float temp_val_1;

void setup() {
  // initialize serial communication with computer:
  Serial.begin(9600);
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

}
void Avg_LM35_1(){
                                                                                      //  TEMP VALUE 1
  temp_adc_val = analogRead(inputPin);  /* Read Temperature */
  temp_val = (temp_adc_val * 4.88); /* Convert adc value to equivalent voltage */
  temp_val = (temp_val/10); /* LM35 gives output of 10mv/°C */
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = temp_val;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
     readIndex = 0;
  }
  average = total / numReadings;
  print_value_1(temp_val,readIndex,average);
  delay(1);        // delay in between reads for stability
}
void Avg_LM35_2(){
                                                                               // Temp Value 2
  temp_adc_val_1 = analogRead(inputPin_1);  
  temp_val_1 = (temp_adc_val_1 * 4.88); 
  temp_val_1 = (temp_val_1/10); 
  total_1 = total_1 - readings_1[readIndex_1];
  // read from the sensor:
  readings_1[readIndex_1] = temp_val_1;
  // add the reading to the total:
  total_1 = total_1 + readings_1[readIndex_1];
  // advance to the next position in the array:
  readIndex_1 = readIndex_1 + 1;
  if (readIndex_1 >= numReadings_1) {
    readIndex_1 = 0;
  }
  average_1 = total_1 / numReadings_1;
  
  print_value_1(temp_val_1,readIndex_1,average_1);
  delay(1);        
}
void print_value(float temp_1, int readIn_1,float Avg_1){
  Serial.print("Temperature_2 = ");
  Serial.println(temp_1);
  Serial.print("readIndex_2:");
  Serial.println(readIn_1);
  Serial.print("AVG_2:");
  Serial.println(Avg_1);
}
void print_value_1(float temp_2, int readIn_2,float Avg_2){
  Serial.print("Temperature_2 = ");
  Serial.println(temp_2);
  Serial.print("readIndex_2:");
  Serial.println(readIn_2);
  Serial.print("AVG_2:");
  Serial.println(Avg_2);
}
void loop() {
  Avg_LM35_1();
  Avg_LM35_2();
  delay(2000);
}
