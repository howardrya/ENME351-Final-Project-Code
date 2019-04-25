//Libraries
#include <movingAvg.h>  //Source - https://github.com/JChristensen/movingAvg

// Global Variables


//Plant Preferences
int minimummoisture = 80; //Plant likes soil at least 80% wet
int amountoflight = 350;

//Water Resevoir
int watercapacity = 1; //Amount of initial water in Liters
float remainingwater;
boolean refilrequired = 0;

//Relay Variables
int lightrelay = 10;
int pumprelay = 11;

//Light Variables
boolean lightstate;
int lightavg=0;

//Pump Variables
boolean pumpstate;

//Soil Sensor Variables
int soilsensorpin = A0;
movingAvg soilsensor(10);
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

//Soil Sensor Setup
soilsensor.begin();

//Flow Sensor Setup
pinMode(pulsecount, INPUT);
attachInterrupt(digitalPinToInterrupt(flowsensor), flowcalc, RISING);

//Light Sensor Setup
lightsensor.begin();

}

void loop() {

  //Check Sensors
  moisturep=readmoisture();
  lightavg=measurelight();
  remainingwater=float(watercapacity)-float(flowtot);


  //Compare Conditions

if(moisturep < minimummoisture){
  pumpstate=turnonpump();
}

if(moisturep > minimummoisture){
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
  measureflow();
}

//Resevoir Check
if(remainingwater < 0.1){
  refilrequired=1;
}


//Serial Monitor Reporting
String conditions= String(lightstate) + "\t" + String(pumpstate) + "\t" + String(moisturep) + "\t" + String(lightavg) + "\t" + String(remainingwater) + "\t" + String(refilrequired) + "\t" + String(flowtot);
Serial.println(conditions);


//Delay Time

delay(1000);

}

//Control Functions

float turnonlight(){
  digitalWrite(lightrelay, HIGH);
  boolean lightstateresult = 0;
  return lightstateresult;
}

float turnofflight(){
  digitalWrite(lightrelay, LOW);
  boolean lightstateresult = 1;
  return lightstateresult;
}

float turnonpump(){
  digitalWrite(pumprelay, LOW);
  boolean pumpstateresult = 1;
  return pumpstateresult;
}

float turnoffpump(){
  digitalWrite(pumprelay, HIGH);
  boolean pumpstateresult = 0;
  return pumpstateresult;
}

float readmoisture(){
  soilreading=analogRead(soilsensorpin);
  int soilavg = soilsensor.reading(soilreading);
  int moistureresult = map(soilavg, lowsoilcal, highsoilcal, 0, 100);
  return moistureresult;
}

void flowcalc(){
  pulsecount++;
}

float measureflow(){
  interrupts();
  delay(500);
  noInterrupts();
  flowtot=(pulsecount/23)*(0.5/60); //Outputs in Liters
  return flowtot;
}

float measurelight(){
  lightreading=analogRead(lightsensorpin);
  int lightresult = lightsensor.reading(lightreading);
  return lightresult;
}
