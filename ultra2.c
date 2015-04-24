#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "wiring_pulse.c"

// #define TRUE 1
 
#define TRIG 4
#define ECHO 5
 
void ultraSetup() {
        wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO, INPUT);
 
        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
}


int getUltra() {
        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(20);
        digitalWrite(TRIG, LOW);
 
        int distance = pulseIn(ECHO, HIGH);
        //Get distance in cm
        int distance = distance / 58;
 
        return distance;
}

int getCM(){
 int val = 10;
 int tmp=0;
 
  for(int i = 0; i<val; i++){
   tmp = tmp + getUltra();
  }
  
  return tmp/val;
}
