#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
 
#define TRUE 1
 
#define TRIG 23
#define ECHO 24
 
void ultraSetup() {
 //       wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO, INPUT);
 
        //TRIG pin must start LOW
        digitalWrite(TRIG, 0);
        delay(30);
}
 
int getCM() {
        //Send trig pulse
        digitalWrite(TRIG, 1);
        sleep(300000);
        /*
        delayMicroseconds(20);
        digitalWrite(TRIG, 0);
        //Wait for echo start
        while(digitalRead(ECHO) == 0);
        printf("1a loop\n");
        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == 1);
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        int distance = travelTime / 58;
 
        return distance; */
        return 0;
}
 
//int main(void) {
//        setup();
// 
//        printf("Distance: %dcm\n", getCM());
// 
//        return 0;
//}
