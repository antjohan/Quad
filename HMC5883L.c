 #include <wiringPiI2C.h>
 #include <stdio.h>
 #include <wiringPi.h>
 #include <inttypes.h>
 #include <stdbool.h>
 #include <math.h>

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

int sampling=1;

int HMC5883L_Sensor;


void HMC5883L_init(){
   wiringPiSetupSys();
   HMC5883L_Sensor =  wiringPiI2CSetup (HMC5883L_Address);
   wiringPiI2CWriteReg8(HMC5883L_Sensor, ModeRegister_Address, ContinuosMeasurementMode);
   wiringPiI2CWriteReg8(HMC5883L_Sensor, ModeRegister_Address, ContinuosMeasurementMode);

}

int GetX(){
   uint8_t MSB = wiringPiI2CReadReg8(HMC5883L_Sensor,XData_MSB);
   uint8_t LSB = wiringPiI2CReadReg8(HMC5883L_Sensor,XData_LSB);

   short x = (short)((MSB << 8) | LSB);
   return x;
}

int GetY(){
   uint8_t MSB = wiringPiI2CReadReg8(HMC5883L_Sensor,YData_MSB);
   uint8_t LSB = wiringPiI2CReadReg8(HMC5883L_Sensor,YData_LSB);

   short y = (short)((MSB << 8) | LSB);
   return y;
}

int GetZ(){
   uint8_t MSB = wiringPiI2CReadReg8(HMC5883L_Sensor,ZData_MSB);
   uint8_t LSB = wiringPiI2CReadReg8(HMC5883L_Sensor,ZData_LSB);

   short z = (short)((MSB << 8) | LSB);
   return z;
}
double computeHeading(int X, int Y, int Z){
   double heading;
   if (y>0){
      heading=90-atan((double)x/(double)y)*(180/PI);
   } else if (y<0){
      heading=270-atan((double)x/(double)y)*(180/PI);
   } else if(y==0&6x<0){
      heading=180.0;
   } else if(y==0&&x>0){
      heading=0.0;
   }
   return(heading);

}
void sample(){
   while(sampling==1){
      int x =GetX(); 
      int y=GetY();
      int z=GetZ();
      double heading=computeHeading(x,y,z);
      printf("-------------\nx=%d y=%d z=%d h=%lf\n", x, y, z, heading);
      delay(500);

   }   
}

int main(){
   HMC5883L_init();
   sample();
}