//Libraries
#include <movingAvg.h>  //Source - https://github.com/JChristensen/movingAvg

// Global Variables

//Plant Preferences
int minimummoisture = 70; //Plant likes soil at least 70% wet
int amountoflight = 500; //Shade Required to activate grow light

//Water Resevoir
float watercapacity = 0.4; //Amount of initial water in Liters
float remainingwater;
boolean refilrequired = 0;

//Relay Variables
int lightrelay = 10;
int pumprelay = 11;

//Light Variables
boolean lightstate;
int lightavg;

//Pump Variables
boolean pumpstate;

//Soil Sensor Variables
int soilsensorpin = A0;
int soilreading;
int moisturep;
int lowsoilcal = 501; //SS reading in air
int highsoilcal = 242; //SS reading fully immersed in water

//Flow Meter Variables
int flowsensor = 3; //Can only be pins 2 or 3 on Arduino UNO
float flowrate = 0;
volatile int pulsecount; //Must be a volatile int to work with interupts
float flowtot=0;

//Light Sensor Setup
int lightsensorpin = A1;
movingAvg lightsensor(10);
int lightreading;

void setup() {

//Global Setup
Serial.begin(9600);

//Relay Setup
pinMode(lightrelay, OUTPUT);
pinMode(pumprelay, OUTPUT);
digitalWrite(lightrelay, LOW);
digitalWrite(pumprelay, LOW);

//Flow Sensor Setup
pinMode(pulsecount, INPUT);
attachInterrupt(digitalPinToInterrupt(flowsensor), flowcalc, RISING); //Required for counting the pullses from the hall effect flow sensor

//Light Sensor Setup
lightsensor.begin(); initializes the moving mean library

}

void loop() {

interrupts(); //required to ensure delays work incase the counter function gets interupted mid cycle
  
  //Check Sensors
  moisturep=readmoisture();
  lightavg=measurelight();
  remainingwater=float(watercapacity)-float(flowtot);


  //Compare Conditions

if(moisturep < minimummoisture){
  pumpstate=turnonpump();
}
else{
  pumpstate=turnoffpump();
}

if(lightavg < amountoflight){
  lightstate=turnonlight();
}
if(lightavg > amountoflight){
  lightstate=turnofflight();
}


 //Pump Check

if(pumpstate){
  flowtot=measureflow();
}

//Resevoir Check

if(remainingwater <= 0.5){
  refilrequired=1;
}


//Serial Monitor Reporting to the User

String conditions= String(lightstate) + "\t" + String(pumpstate) + "\t" + String(moisturep) + "\t" + String(lightavg) + "\t" + String(remainingwater) + "\t" + String(refilrequired) + "\t" + String(flowtot);
Serial.println(conditions);


//Delay Time

delay(1000);

}

//Control Functions

//Turn on light and set light state to on
float turnonlight(){
  digitalWrite(lightrelay, HIGH);
  boolean lightstateresult = 0;
  return lightstateresult;
}

//Turn off light and set light state to off
float turnofflight(){
  digitalWrite(lightrelay, LOW);
  boolean lightstateresult = 1;
  return lightstateresult;
}

//Turn on pump and set pump state to on 
float turnonpump(){
  digitalWrite(pumprelay, LOW);
  boolean pumpstateresult = 1;
  return pumpstateresult;
}

//Turn off pump and set pump state to off
float turnoffpump(){
  digitalWrite(pumprelay, HIGH);
  boolean pumpstateresult = 0;
  return pumpstateresult;
}

//Read the analog value from the moisture sensor and map it to a percent scale from calibrated values above
float readmoisture(){
  soilreading=analogRead(soilsensorpin);
  int moistureresult = map(soilreading, lowsoilcal, highsoilcal, 0, 100);
  return moistureresult;
}

//Sub-function of the flowmetering function
void flowcalc(){
  pulsecount++;
}

//Measures flow by counting the pulses in the wave form from the hall effect flow sensor. Pulses are then converted to liters from the calibration sheet included with the sensor
float measureflow(){
  interrupts();
  delay(500);
  noInterrupts();
  float flowtotresult=2.5*(pulsecount/23)*(0.5/60); //Outputs in Liters
  return flowtotresult;
}

//Measures the amount of ambient light with a moving mean to ensure no rapid transitions turning on the grow light
float measurelight(){
  lightreading=analogRead(lightsensorpin);
  int lightresult = lightsensor.reading(lightreading);
  return lightresult;
}
