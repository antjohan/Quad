 #include <stdio.h>
#include <stdlib.h>
 #include <wiringPiI2C.h>
 #include <wiringPi.h>
 #include <inttypes.h>
 #include <stdbool.h>
 #include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
 
// #define TRUE 1
 
#define TRIG 4
#define ECHO 5

//functions
void sample();
long getCM();
void getCMloop();
void ultraSetup();
long getUltra();
void getCMloop();


int ultrasonicfifofd;

int main(){
  ultraSetup();
  printf("hej7\n");
  sample();

}
void sample(){
  long currentHeight;
  char WriteBuf[10];
  printf("hej8\n");
  while(1){
    currentHeight=getCM();
    sprintf(WriteBuf,"%ld",currentHeight);
    printf("UH-int: %s", WriteBuf);
    write(ultrasonicfifofd,WriteBuf,sizeof(WriteBuf));
  }
}
 
void ultraSetup() {
   wiringPiSetup();
  //open fifo
  char* ultrasonicfifo = "/home/pi/tmp/ultrasonicfifo";
  //delete in case it already exists
  printf("hej1\n");
  unlink(ultrasonicfifo);
  delay (500);
  printf("hej2\n");
  int a = mkfifo(ultrasonicfifo,0666);
  printf("hej3\n");
  if (a==-1){
    printf("mkfifoerror-ultra: %s\n",strerror(errno));
  } else {
    printf("ultra-fifo-created");
  }
  delay(200);
  printf("hej4\n");
  ultrasonicfifofd=open(ultrasonicfifo, O_WRONLY);
  if (ultrasonicfifofd==-1){ 
    printf("openfifoerror-ultra: %s\n",strerror(errno));
  } else {
   printf("ultra-fifo-open\n");
  }
  printf("hej5\n");
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO, INPUT);
   printf("hej6\n");
        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
   printf("hej6\n");
}

long getUltra() {
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
        long distance = travelTime / 58;
 
        return distance;
}


// I funktionen/filtret kan man välja antal mätningar(val) 

long getCM(){
   long val = 10;
   long sum;


   // Första mätningen, val=antal mätvärden,
   // värden större än 400cm förkastas
   long m1[val];
   long tmp;
   long medel1 = 0;
   long length1 = 0;
   for(int i = 0; i<val; i++){
      tmp = getUltra();         // Gör en mätning
      if(tmp < 400 && tmp > 0){            // Kollar om värdet är rimligt (kortare än 400cm)
         m1[length1] = tmp;       // Sparar undan värdet
         length1 = length1 + 1;   // Ökar indexeringen
         medel1 = tmp + medel1;   // Summerar för att kunna göra medelvärde
      }
      if (length1 == 0){
      printf("Avståndet är längre än 4m\n");
      return ((long)-1);
      }
      delay(50);               // Fördröjning för att det ska fungera, tror 30 är lagom
   }
   // Mätningarnas medelvärde 
   long medel = medel1  / length1;
   // Mätvärdenas individuella differens från medelvärdet (diff)
   // och medelvärde av avvikelssen (diff_medel)
   long diff[length1];    // Vektor att fylla med individuella skillnader
   long diff_medel = 0;
   for(int i = 0; i<length1; i++){
      diff[i] = abs(medel1 - m1[i]);        // Differens i längd
      diff_medel = diff_medel + diff[i];  // Summerar alla differenser för att kunna ta medelvärde
   }
   diff_medel = diff_medel/length1;      // Tar medevärdet

   // Filtrerar bort de mätningar som är vars avvikelse är 
   // längre bort än medelavvikelsen + 5 cm
   long count = 0;                      // Räknare för indexering då vektorn blir kortare
   long res[val];
   for(int i = 0; i<length1; i++){
      if(diff[i] < diff_medel + 5 ){     // Krav för att bli godkänd
         res[count] = m1[i];                // Skrivs in i vektorn
         count = count + 1;                // Indexering ökar
      }
   }


   // De mätvärden som är godkända summeras och medelvärdes tas
   long ans = 0;                       
   for(int i = 0; i<count; i++){ 
      ans = ans + res[i];             
   }
   ans = ans/count;


   /*  // Skriver till fil
   FILE *fp;
   fp=fopen("data.txt","r+");
   //fprintf(fp, "%ld\n", ans);
   fclose(fp);
   */

   printf("%ld\n", ans);
   return ((long)ans);
}

void getCMloop(){
 long l;
 while(1){
 l = getCM();
 printf("%ld\n", l);
 }
}






