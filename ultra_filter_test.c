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


// I funktionen kan man välja antal mätningar i varje set (tot 3 set) 
int getCM(){
 int val = 5;
 int sum;
 
 // Första mätningen, val=antal mätvärden,
 int m1[val];
 int tmp;
 int length1 = 0;
 
  for(int i = 0; i<val; i++){
   tmp = getUltra();
   
   if(tmp < 400){
    m1[length1] = tmp;
    length1 = length1 + 1;
    }
   delay(100);
  }
  
  
 // Andra mätningen, val=antal mätvärden,
  // värden större än 400cm förkastas
 int m2[val];
 int tmp;
 int length2 = 0;
 
  for(int i = 0; i<val; i++){
   tmp = getUltra();
   
   if(tmp < 400){
    m2[length2] = tmp;
    length2 = length2 + 1;
    }
   delay(100);
  }
 
 
 // Tredje mätningen, val=antal mätvärden, 
 // värden större än 400cm förkastas
 int m3[val];
 int tmp;
 int length3 = 0;
 
  for(int i = 0; i<val; i++){
   tmp = getUltra();
   
   if(tmp < 400){
    m3[length3] = tmp;
    length3 = length3 + 1;
    }
   delay(100);
  }
  
  
  










  
  // De tre mätningarnas medelvärde (medelvärde av 3*val mätvärden)
  int medel_tot = (medel_1 + medel_2 + medel_3) / 3;

 // Mätvärdenas individuella differens från medelvärdet
 
 // Värden större än 4 meter plockas bort
 int m[3 * val] = {tmp_1, tmp_2, tmp_3};
 for(int i = 0; i<3*val; i++){
  if( m[i] )
  
 }
 
 int diff[3 * val];
 int diff_tot = 0;
 int count_diff = 0;
 
  for(int i = 0; i<3*val; i++){
   
   
   
   
   diff[i] = abs(medel_tot-m[i]);
   
   if(m[i] > 400){
    m[i] = 0;
    
    }
   
  
  
  diff = diff/count_diff;
}

  for(int i = 0; i<3*val; i++){
   if
   
   
   
  }
  
  
  // Skriver till fil
    FILE *fp;
fp=fopen("data.txt","r+");
//fprintf(fp, "%i", res, "\n");
fclose(fp);
  
  
  return res;
  
  
}
