 
int turbidity_sensorPin = A0;
float turbidity_volt;
float ntu;
 
void setup()
{
  Serial.begin(9600);

}

void loop()
{
    TurbidityRead();

}
void TurbidityRead(){
  turbidity_volt = 0;
  for(int i=0; i<800; i++)
  {
    turbidity_volt += ((float)analogRead(turbidity_sensorPin)/1023)*5;
//    turbidity_volt += ((float)analogRead(turbidity_sensorPin)/875)*5;
  }
  turbidity_volt = turbidity_volt/800;
  turbidity_volt = round_to_dp(turbidity_volt,2);
  if(turbidity_volt < 2.5){
    ntu = 3000;
  }else{
//    ntu = -1120.4*square(turbidity_volt)+5770.3*turbidity_volt-4352.2; 
      ntu = -1120.4*square(turbidity_volt)+5742.3*turbidity_volt-4352.9; 
  }
  Serial.print(turbidity_volt);
  Serial.print(" V");
  Serial.println();
  
  Serial.print(ntu);
  Serial.print(" NTU");
  Serial.println();
  delay(500);
}
float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
//void setup() {
//  Serial.begin(9600); //Baud rate: 9600
//}
//void loop() {
//  int sensorValue = analogRead(A0);// read the input on analog pin 0:
//  float voltage = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
//  Serial.println(voltage); // print out the value you read:
//  delay(500);
//}
