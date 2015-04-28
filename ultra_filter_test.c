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


// I funktionen/filtret kan man välja antal mätningar(val) i varje set (tot 3 set) 

int getCM(){
 int val = 5;
 int sum;
 
 // Första mätningen, val=antal mätvärden,
 // värden större än 400cm förkastas
 int m1[val];
 int tmp;
 int medel1 = 0;
 int length1 = 0;
 
  for(int i = 0; i<val; i++){
   tmp = getUltra();         // Gör en mätning
   if(tmp < 400){            // Kollar om värdet är rimligt (kortare än 400cm)
    m3[length1] = tmp;       // Sparar undan värdet
    length1 = length1 + 1;   // Ökar indexeringen
    medel1 = tmp + medel1;   // Summerar för att kunna göra medelvärde
    }
   delay(100);               // Fördröjning för att det ska fungera, tror 30 är lagom
  }
  
  // Mätningarnas medelvärde 
  int medel = medel1  / length1;

 // Mätvärdenas individuella differens från medelvärdet (diff)
 // och medelvärde av avvikelssen (diff_medel)

 int diff[length1];    // Vektor att fylla med individuella skillnader
 int diff_medel = 0;

 for(int i = 0; i<length; i++){
 diff[i] = abs(medel1 - m1[i]);        // Differens i längd
 diff_medel = diff_medel + diff[i];  // Summerar alla differenser för att kunna ta medelvärde
 }
 diff_medel = diff_medel/length1;      // Tar medevärdet

// Filtrerar bort de mätningar som är vars avvikelse är 
// längre bort än medelavvikelsen + 5 cm
 int count = 0;                      // Räknare för indexering då vektorn blir kortare

 for(int i = 0; i<length1; i++){
  if(diff[i] < diff_medel + 5 ){     // Krav för att bli godkänd
   res[count] = m1[i];                // Skrivs in i vektorn
   count = count + 1;                // Indexering ökar
   }
 }
 
 
 // De mätvärden som är godkända summeras och medelvärdes tas
 int ans = 0;                       
 for(int i = 0; i<count; i++){ 
  ans = ans + res[i];             
 }
 ans = ans/count;
 
 
  // Skriver till fil
    FILE *fp;
fp=fopen("data.txt","r+");
//fprintf(fp, "%i", ans, "\n");
fclose(fp);
  
  return ans;
  
  
}
