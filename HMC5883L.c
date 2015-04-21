 #include <wiringPiI2C.h>
 #include <wiringPi.h>
 #include <inttypes.h>
 #include <stdbool.h>

#define HMC5883L_Address 0x1E
 #define ModeRegister_Address 0x02
 #define ContinuosMeasurementMode 0x00
#define XData_MSB 0x03
#define XData_LSB 0x04
#define ZData_MSB 0x05
#define ZData_LSB 0x06
#define YData_MSB 0x07
#define YData_LSB 0x08

int HMC5883L_Sensor;


void HMC5883L_init(){
   wiringPiSetupSys();
   HMC5883L_Sensor =  wiringPiI2CSetup (HMC5883L_Address);
   wiringPiI2CWriteReg8(HMC5883L_Sensor, ModeRegister_Address, ContinuosMeasurementMode);
}

int GetX(){
   uint8_t MSB = wiringPiI2CReadReg8(XData_MSB);
   uint8_t LSB = wiringPiI2CReadReg8(XData_LSB);

   int x = (MSB << 8) | LSB;
   return x;
}

int GetY(){
   uint8_t MSB = wiringPiI2CReadReg8(YData_MSB);
   uint8_t LSB = wiringPiI2CReadReg8(YData_LSB);

   int y = (MSB << 8) | LSB;
   return y;
}

int GetX(){
   uint8_t MSB = wiringPiI2CReadReg8(ZData_MSB);
   uint8_t LSB = wiringPiI2CReadReg8(ZData_LSB);

   int z = (MSB << 8) | LSB;
   return z;
}

int main(){
   HMC5883L_init();
   while(1){
      delay(100);
      printf("%d\n", GetX());
   }
}