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

#define HMC5883L_Address 0x1E
 #define ModeRegister_Address 0x02
 #define ContinuosMeasurementMode 0x00
#define CRA 0x00 //configuration register a
#define CRB 0x01 //configuration register b
#define XData_MSB 0x03
#define XData_LSB 0x04
#define ZData_MSB 0x05
#define ZData_LSB 0x06
#define YData_MSB 0x07
#define YData_LSB 0x08

#define PI 3.14159265358979323846

int sampling=1;
int HMC5883L_Sensor;
double heading;

double offset_x=0;
double offset_y=0;
double offset_z=0;

   char* to_mag_fifo = "/home/pi/tmp/to_mag_fifo";
   char* from_mag_fifo = "/home/pi/tmp/from_mag_fifo";
   int to_mag_fd;
   int from_mag_fd;

   //functions
   void HMC5883L_init();
   void connectFifos();
   void writeOutput();
   void checkPipe();
   int GetX();
   int GetY();
   int GetZ();
   double computeHeading(int x, int y, int z);
   void sample();
   void calibrate();

int main(){
   HMC5883L_init();
   connectFifos();
   sample();
}

void HMC5883L_init(){
   wiringPiSetupSys();
   HMC5883L_Sensor =  wiringPiI2CSetup (HMC5883L_Address);
   wiringPiI2CWriteReg8(HMC5883L_Sensor, ModeRegister_Address, ContinuosMeasurementMode);
   wiringPiI2CWriteReg8(HMC5883L_Sensor, ModeRegister_Address, ContinuosMeasurementMode);
}

void connectFifos(){
   char* from_mag_fifo = "/home/pi/tmp/from_mag_fifo";
   char* to_mag_fifo = "/home/pi/tmp/to_mag_fifo";
  //delete in case it already exists
   unlink(from_mag_fifo);
   delay(200);
  if (mkfifo(from_mag_fifo,0666)==-1){
    printf("m_make_from_mag=error: %s\n",strerror(errno));
  } 
  from_mag_fd=open(from_mag_fifo, O_WRONLY);
  if (from_mag_fd==-1){ 
    printf("m_from_mag_fifo=error: %s\n",strerror(errno));
  } else {
   printf("m_from_mag_fifo=open\n");
  }

  to_mag_fd=open(to_mag_fifo, O_RDONLY);
  if (to_mag_fd==-1){
       printf("m_to_mag_fifo=error: %s\n",strerror(errno));
  } else {
         printf("m_to_mag_fifo=open\n");
  }
}
void writeOutput(){
   char WriteBuf[56];
   sprintf(WriteBuf,"%lf",heading);
   write(from_mag_fd,WriteBuf,sizeof(WriteBuf));
}
void checkPipe(){
   char buffer[10];

   char str1[10];
   char str2[10];
   char str3[10];

   strcpy(str1,"ping");
   strcpy(str2, "read");
   strcpy(str3,"calibrate");

   if (read(to_mag_fd, buffer, 10)>0){
       if (strcmp(buffer,str1)==0){
         printf("Magnetometer says hi! :)\n");
       } else if(strcmp(buffer,str2)==0){
         writeOutput();
       } else if(strcmp(buffer,str3)==0){
         calibrate();
       }
   }
}

int GetX(){
   uint8_t MSB = wiringPiI2CReadReg8(HMC5883L_Sensor,XData_MSB);
   uint8_t LSB = wiringPiI2CReadReg8(HMC5883L_Sensor,XData_LSB);
   short x = (short)((MSB << 8) | LSB);
   return x-(int)offset_x;
}

int GetY(){
   uint8_t MSB = wiringPiI2CReadReg8(HMC5883L_Sensor,YData_MSB);
   uint8_t LSB = wiringPiI2CReadReg8(HMC5883L_Sensor,YData_LSB);

   short y = (short)((MSB << 8) | LSB);
   return y-(int)offset_y;
}

int GetZ(){
   uint8_t MSB = wiringPiI2CReadReg8(HMC5883L_Sensor,ZData_MSB);
   uint8_t LSB = wiringPiI2CReadReg8(HMC5883L_Sensor,ZData_LSB);

   short z = (short)((MSB << 8) | LSB);
   return z-(int)offset_z;
}

double computeHeading(int x, int y, int z){
   double hd;
   if (y>0){
      hd=90-atan((double)x/(double)y)*(180/PI);
   } else if (y<0){
      hd=270-atan((double)x/(double)y)*(180/PI);
   } else if(y==0&&x<0){
      hd=180.0;
   } else if(y==0&&x>0){
      hd=0.0;
   }
   return(hd);
}

void sample(){
   int x;
   int y;
   int z;
   while(sampling==1){
      checkPipe();
      x = GetX(); 
      y = GetY();
      z = GetZ();
      heading=computeHeading(x,y,z);
     // printf("Heading: %lf\n", heading);
      delay(100);
   }   
}

void calibrate(){
   int calsamp=2000;
   int xvalues[calsamp];
   int yvalues[calsamp];
   int zvalues[calsamp];

   int max_x;
   int max_y;
   int max_z;
   int min_x;
   int min_y;
   int min_z;

   printf("Move magnetometer around in circles! All axes\n");
   for (int i=0; i<calsamp;++i){
      xvalues[i]=GetX();
      yvalues[i]=GetY();
      zvalues[i]=GetZ();

      if (xvalues[i]>max_x){
         max_x=xvalues[i];
      }
      if (yvalues[i]>max_y){
         max_y=yvalues[i];
      }
      if (zvalues[i]>max_z){
         max_z=zvalues[i];
      }
      if (xvalues[i]<min_x){
         min_x=xvalues[i];
      }
      if (yvalues[i]<min_y){
         min_y=yvalues[i];
      }
      if (zvalues[i]<min_z){
         min_z=zvalues[i];
      }

      if (i%(calsamp/10)==0){
         printf("Calibration %d%% done\n", i*100/calsamp);
      }
   }
   printf("DONE moving..\n");
   offset_x=(min_x+max_x)/2.0;
   offset_y=(min_y+max_y)/2.0;
   offset_z=(min_z+max_z)/2.0;
   printf("Magnetometer hard-iron compensated!\n");
}

