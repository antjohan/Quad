#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
 
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
 
        //Wait for echo start
        while(digitalRead(ECHO) == LOW);
 
        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        int distance = travelTime / 58;
 
        return distance;
}

int getCM(){
 int val = 5;
 int tmp=0;
 int sum=0;
 
  for(int i = 0; i<val; i++){
 //  while(1){
   tmp = getUltra();
   sum = sum+tmp;
   printf("%i\n", tmp);
   delay(100);
  }
  int res = sum/val;
  
    FILE *fp;
fp=fopen("data.txt","r+");
//fprintf(fp, "%i", res, "\n");
fclose(fp);
  
  
  return res;
  
  
}







 
