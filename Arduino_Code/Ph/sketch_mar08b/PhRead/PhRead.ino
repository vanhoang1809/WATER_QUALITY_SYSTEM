#define pHsensorPin A3          //pH meter Analog output to Arduino Analog Input 0
unsigned long int avgValueForPH;  //Store the average value of the ph sensor feedback
int pHbuffer[10],tempValueForPH;
float phValue;
void setup()
{
  pinMode(13,OUTPUT);  
  Serial.begin(9600);  
  Serial.println("Ready");     //Test the serial monitor
}
void phRead(){
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
//  if(Voltage <=1.3){
//    Voltage+=0.015;
//  }
  if(Voltage >= 1.8 && Voltage <=1.9){
    Voltage+=0.15;
  }
  Serial.print("    Voltage:");  
  Serial.print(Voltage,2);     //convert the analog into millivolt
  phValue=3.5*Voltage;         //convert the millivolt into pH value
  delay(800);
}
void sendLora(){
  phRead();
  Serial.print("    pH:");  
  Serial.print(phValue,2);
  Serial.println(" ");
}
void loop(){
 sendLora();
}
