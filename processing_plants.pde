//Portions of Processing code borrowed from ENME351 Lab examples

import processing.serial.*; // add the serial library
Serial myPort; // define a serial port object to monitor

//Initialize Image variables
PImage bucket;
PImage light;
PImage plant;
PImage pump;
PImage sun;

//Time counter to refresh the processing window
int time=0;

void setup() {
  size(1000, 500); // set the window size
  println(Serial.list()); // list all available serial ports
  myPort = new Serial(this, Serial.list()[3], 9600); // define input port
  myPort.clear(); // clear the port of any initial junk
 
 //Load the images from file
 bucket = loadImage("bucket.png");
 light = loadImage("light.png");
 pump = loadImage("pump.png");
 sun = loadImage("sun.png");
 plant = loadImage("plant.png");

//Set initial bbackground color
background(255);
 
}
void draw () {
  
  //Display static info and pictures
  textSize(32);
  fill(0);
  text("Auotmated Plant Care", 330, 40);
  textSize(16);
  text("Remaining Resevoir Water", 25, 300);
  image(plant, 400, 300, 200, 200);
  textSize(20);
  text("Water Used", 760, 300);
  text("Soil Moisture", 752, 400);
  
while (myPort.available () > 0) { // make sure port is open
  String inString = myPort.readStringUntil('\n'); // read input string
  
if (inString != null) { // ignore null strings
inString = trim(inString); // trim off any whitespace
String[] xyzaRaw = splitTokens(inString, "\t"); // extract x & y into an array
// proceed only if correct # of values extracted from the string:

if (xyzaRaw.length == 7) {
  //extract the string data to independent variables
  int lightstate = int(xyzaRaw[0]);
  int pumpstate = int(xyzaRaw[1]);
  int moisturep = int(xyzaRaw[2]);
  int lightavg = int(xyzaRaw[3]);
  float remainingwater = float(xyzaRaw[4]);
  int refilrequired = int(xyzaRaw[5]);
  float flowtot = float(xyzaRaw[6]);
  
//Display images based on conditions

if(lightstate==1){
  image(light, 600, 200, 100, 100);
}
else{
  image(sun, 325, 200, 100, 100);
}
if(pumpstate==1){
  image(pump, 70, 150, 100, 100);
  textSize(20);
  text("Now Watering", 50, 135);
}
textSize(20);
text(moisturep + "%", 800, 425);
text(remainingwater + "Liter(s)", 80, 335);
text(flowtot + "Liter(s)", 775, 330);
time=time+1;

if(refilrequired==1){
  image(bucket, 70, 375, 100, 100);
  text("Please Refil Resevoir", 20, 365);
}
  
}
//Refreshes the processing screen after 4 updates from the arduino
if(time>4){
  background(255);
  time=0;
}
}
}
}
