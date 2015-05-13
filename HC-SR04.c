#include "HC-SR04.h"

int main(){
  HCSR04_init();
  sample();

}
void sample(){
   while(sampling==1){
      checkPipe();
      currentHeight=movingAvg(getUltra());
      //printf("UH-int: %s\n", WriteBuf);
      delay(7);
   }
}
 
void HCSR04_init() {
   wiringPiSetup();
   connectFifos();
   pinMode(TRIG, OUTPUT);
   pinMode(ECHO, INPUT);
   //TRIG pin must start LOW
   digitalWrite(TRIG, LOW);
   delay(20);
}

double movingAvg(int newvalue){
  double tmp=0.0;
  for (int i=0;i<avgsamples-1;++i){
    latestdata[i]=latestdata[i+1];
    tmp=tmp+latestdata[i+1];
  }
    latestdata[avgsamples]=newvalue;
    tmp=tmp+newvalue;
    return((double)tmp/(double)avgsamples);
}

void connectFifos(){
   char* from_ultra_fifo = "/home/pi/tmp/from_ultra_fifo";
   char* to_ultra_fifo = "/home/pi/tmp/to_ultra_fifo";
  //delete in case it already exists
   unlink(from_ultra_fifo);
   delay(200);
  if (mkfifo(from_ultra_fifo,0666)==-1){
    printf("u_make_from_ultra=error: %s\n",strerror(errno));
  } 
  from_ultra_fd=open(from_ultra_fifo, O_WRONLY);
  if (from_ultra_fd==-1){ 
      printf("pipe_from_ultra=error: %s\n",strerror(errno));
  } else {
      printf("pipe_from_ultra=connected\n");
  }


  to_ultra_fd=open(to_ultra_fifo, O_RDONLY);
  if (to_ultra_fd==-1){
       printf("pipe_to_ultra=error: %s\n",strerror(errno));
  } else {
      //printf("pipe_to_ultra=connected\n");
  }

}

void writeOutput(){
   char WriteBuf[128];
   sprintf(WriteBuf,"%ld",currentHeight);
   write(from_ultra_fd,WriteBuf,sizeof(WriteBuf));
}
void checkPipe(){
   char buffer[10];

   char str1[10];
   char str2[10];

   strcpy(str1,"ping");
   strcpy(str2, "read");

 if (read(to_ultra_fd, buffer, 10)>0){
          if (strcmp(buffer,str1)==0){
            printf("Ultrasonic says hi :>!\n");
          } else if(strcmp(buffer,str2)==0){
            writeOutput();
          }
        }
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
        while(digitalRead(ECHO) == HIGH){
         if (micros()-startTime>30000){
            return(-1);
         }
        }
        long travelTime = micros() - startTime;
        //Get distance in cm
        int distance = travelTime / 58;
 
        return distance;
}


// I funktionen/filtret kan man välja antal mätningar(val) 

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
      if(tmp < 400 && tmp > 0){            // Kollar om värdet är rimligt (kortare än 400cm)
         m1[length1] = tmp;       // Sparar undan värdet
         length1 = length1 + 1;   // Ökar indexeringen
         medel1 = tmp + medel1;   // Summerar för att kunna göra medelvärde
      }
      if (length1 == 0){
      //printf("Avståndet är längre än 4m\n");
      return ((int)-1);
      }
      //delay(50);               // Fördröjning för att det ska fungera, tror 30 är lagom
   }
   // Mätningarnas medelvärde 
   int medel = medel1  / length1;
   // Mätvärdenas individuella differens från medelvärdet (diff)
   // och medelvärde av avvikelssen (diff_medel)
   int diff[length1];    // Vektor att fylla med individuella skillnader
   int diff_medel = 0;
   for(int i = 0; i<length1; i++){
      diff[i] = abs(medel1 - m1[i]);        // Differens i längd
      diff_medel = diff_medel + diff[i];  // Summerar alla differenser för att kunna ta medelvärde
   }
   diff_medel = diff_medel/length1;      // Tar medevärdet

   // Filtrerar bort de mätningar som är vars avvikelse är 
   // längre bort än medelavvikelsen + 5 cm
   int count = 0;                      // Räknare för indexering då vektorn blir kortare
   int res[val];
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


   /*  // Skriver till fil
   FILE *fp;
   fp=fopen("data.txt","r+");
   //fprintf(fp, "%ld\n", ans);
   fclose(fp);
   */

   //printf("%ld\n", ans);
   return ((int)ans);
}

void getCMloop(){
 int l;
 while(1){
 l = getCM();
 printf("%ld\n", l);
 }
}






