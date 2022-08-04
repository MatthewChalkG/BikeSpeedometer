
#include <Wire.h>

/* Display */
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* Magnetometer */
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <time.h>


#define WIRE Wire

Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);

sensors_event_t event;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  mag.enableAutoRange(true);
  if(!mag.begin())
  {
    while(1);
  }
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);
  display.clearDisplay();
 
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Start");
  display.display();
  Serial.println("setup complete");
  mag.getEvent(&event);
  delay(1000);
}

void stallUntilMagDrop(float mini, float diff){
  mag.getEvent(&event); 
  float bound = (mini + 3.f/5.f*diff);
  while (bound < event.magnetic.z && event.magnetic.z != 0){
    mag.getEvent(&event);
    delay(25);
}
  /*Serial.println("wait end threshold");
  Serial.println((mini+3/5*diff));
  Serial.println(bound);
  Serial.println("wait concluded");
  Serial.println(event.magnetic.z); */
}


void loop() {

  float start_t;
  start_t = millis();
  float current_t;
  current_t = millis();
  
  mag.getEvent(&event);
  float mini;
  float maxi;
  mini = event.magnetic.z;
  maxi = event.magnetic.z;

  float n = 0;
  while (1000 > n) {
    mag.getEvent(&event);
 
    if (mini > event.magnetic.z && event.magnetic.z != 0){
      mini = event.magnetic.z;
    }
    else if (maxi < event.magnetic.z && event.magnetic.z != 0){
      maxi = event.magnetic.z;
    }
   
   
    current_t = millis();
    n = current_t - start_t;
    delay(25);
  }
   Serial.println("min, max");
  Serial.println(mini);
  Serial.println(maxi);
  
  float diff = maxi-mini;
  stallUntilMagDrop(mini,diff);
  mag.getEvent(&event);

  while ((mini + 4.f/5.f*diff) > event.magnetic.z && event.magnetic.z != 0){
    mag.getEvent(&event);
    delay(25);
  }

  start_t = millis();
  current_t = millis();
 
  int numRotations = 0;

  n = 0;
  while ( 1000 > n){
    /* 4/5s marker for the max magn */
    stallUntilMagDrop(mini, diff);
    mag.getEvent(&event);
    while ((mini + 4.f/5.f*diff) > event.magnetic.z && event.magnetic.z != 0){
      mag.getEvent(&event);
      delay(25);
     }
    /*Serial.println("rot hit");
    Serial.println(event.magnetic.z);*/
    
    numRotations += 1;
    current_t = millis();
    n = current_t - start_t;
  }
  
  float timeTime = (current_t- start_t)/1000.f;

  float speedOfBike = numRotations/timeTime *91.106 * .0568;
 /* Serial.println("rot, time");
  Serial.println(numRotations);
  Serial.println(timeTime);*/
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(speedOfBike);
  display.display();
  delay(1000);
  /*Serial.println("speed:");*/
  Serial.println(speedOfBike);
}
