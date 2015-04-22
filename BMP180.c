 #include <wiringPiI2C.h>
 #include <wiringPi.h>
 #include <inttypes.h>
 #include <stdbool.h>
 #include <math.h>

 #define BMP180_Address 0x77

 #define ChipIdData 0x55
 #define ControlInstruction_MeasureTemperature 0x2E
 #define ControlInstruction_MeasurePressure 0x34

 #define Reg_ChipId 0xD0
 #define Reg_Control 0xF4
 #define Reg_CalibrationStart 0xAA
 #define Reg_CalibrationEnd 0xBE
 #define Reg_AnalogConverterOutMSB 0xF6
 #define Reg_SoftReset 0xE0
 #define SoftResetInstruction 0xB6

 #define ErrorCode_1 "Entered sample resolution was invalid. See datasheet for details."
 #define ErrorCode_1_Num 1

 #define BMP180_Mode_UltraLowPower   0
 #define BMP180_Mode_Standard      1
 #define BMP180_Mode_HighResolution    2
 #define BMP180_Mode_UltraHighResolution 3

 uint8_t OversamplingSetting;
 bool Oversample;
 int BMP180_Sensor;
 int ConversionWaitTimeMs;
 int LastTemperatureData;
 int LastTemperatureTime;
 int AcceptableTemperatureLatencyForPressure;

 int Calibration_AC1;
 int Calibration_AC2;
 int Calibration_AC3;
 unsigned int Calibration_AC4;
 unsigned int Calibration_AC5;
 unsigned int Calibration_AC6;
 int Calibration_B1;
 int Calibration_B2;
 int Calibration_MB;
 int Calibration_MC;
 int Calibration_MD;

void Write(int address, int data){
  wiringPiI2CWriteReg8(BMP180_Sensor,address, data);  
}

uint8_t Read(int address)
{
 // Wire.beginTransmission(BMP180_Address);
  //Wire.write(address);
  //Wire.endTransmission();
  
  //Wire.beginTransmission(BMP180_Address);
  //Wire.requestFrom(BMP180_Address, length);

  //uint8_t buffer[length];
  //while(Wire.available())
  //{
//   for(uint8_t i = 0; i < length; i++)
  // {
    //buffer[i] = wiringPiI2CReadReg8(BMP180_Sensor, address);
    //address = address + 0x01;
  //}
  //}
  //Wire.endTransmission();

  return wiringPiI2CReadReg8(BMP180_Sensor, address);
}

void Read2(int address, int length, uint8_t buffer[])
{
  /*
  Wire.beginTransmission(BMP180_Address);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.beginTransmission(BMP180_Address);
  Wire.requestFrom(BMP180_Address, length);

  while(Wire.available())
  {
    for(uint8_t i = 0; i < length; i++)
    {
      buffer[i] = Wire.read();
    }
  }
  Wire.endTransmission();
  */
}

uint8_t SetResolution(uint8_t sampleResolution, bool oversample){
  OversamplingSetting = sampleResolution;
  Oversample = oversample;
  switch (sampleResolution)
  {
    case 0:
    ConversionWaitTimeMs = 5;
    break;
    case 1:
    ConversionWaitTimeMs = 8;
    break;
    case 2:
    ConversionWaitTimeMs = 14;
    break;
    case 3:
    ConversionWaitTimeMs = 26;
    break;
    default:
    return ErrorCode_1_Num;
  }
}

void Initialize(){
//printf("init");
 ConversionWaitTimeMs = 5;
 OversamplingSetting = 3;
 Oversample = false;

 LastTemperatureTime = -1000;
 LastTemperatureData = 0;

 AcceptableTemperatureLatencyForPressure = 1000;
 wiringPiSetupSys();
 //printf("init");

 BMP180_Sensor =  wiringPiI2CSetup (BMP180_Address);
 //printf("init");

 SetResolution(BMP180_Mode_Standard, false);
//printf("init");

 //uint8_t* buffer = Read(Reg_CalibrationStart, Reg_CalibrationEnd - Reg_CalibrationStart + 2);
 //printf("init");

	// This data is in Big Endian format from the BMP180.
  /*Calibration_AC1 = (buffer[0] << 8) | buffer[1];
  Calibration_AC2 = (buffer[2] << 8) | buffer[3];
  Calibration_AC3 = (buffer[4] << 8) | buffer[5];
  Calibration_AC4 = (buffer[6] << 8) | buffer[7];
  Calibration_AC5 = (buffer[8] << 8) | buffer[9];
  Calibration_AC6 = (buffer[10] << 8) | buffer[11];
  Calibration_B1 = (buffer[12] << 8) | buffer[13];
  Calibration_B2 = (buffer[14] << 8) | buffer[15];
  Calibration_MB = (buffer[16] << 8) | buffer[17];
  Calibration_MC = (buffer[18] << 8) | buffer[19];
  Calibration_MD = (buffer[20] << 8) | buffer[21];*/
  Calibration_AC1 = (short)((Read(0xAA) <<8) | Read(0xAB));
  Calibration_AC2 = (short)((Read(0xAC) <<8) | Read(0xAD));
  Calibration_AC3 = (short)((Read(0xAE) <<8) | Read(0xAF));
  Calibration_AC4 = (unsigned short)((Read(0xB0) <<8) | Read(0xB1));
  Calibration_AC5 = (unsigned short)((Read(0xB2) <<8) | Read(0xB3));
  Calibration_AC6 = (unsigned short)((Read(0xB4) <<8) | Read(0xB5));
  Calibration_B1 = (short)((Read(0xB6) <<8) | Read(0xB7));
  Calibration_B2 = (short)((Read(0xB8) <<8) | Read(0xB9));
  Calibration_MB = (short)((Read(0xBA) <<8) | Read(0xBB)); 
  Calibration_MC = (short)((Read(0xBC) <<8) | Read(0xBD));
  Calibration_MD = (short)((Read(0xBE) <<8) | Read(0xBF));

}

void PrintCalibrationData(){
	printf("AC1:\t"); printf("%d\n",Calibration_AC1);
	printf("AC2:\t"); printf("%d\n",Calibration_AC2);
	printf("AC3:\t"); printf("%d\n",Calibration_AC3);
	printf("AC4:\t"); printf("%d\n",Calibration_AC4);
	printf("AC5:\t"); printf("%d\n",Calibration_AC5);
	printf("AC6:\t"); printf("%d\n",Calibration_AC6);
	printf("B1:\t"); printf("%d\n",Calibration_B1);
	printf("B2:\t"); printf("%d\n",Calibration_B2);
	printf("MB:\t"); printf("%d\n",Calibration_MB);
	printf("MC:\t"); printf("%d\n",Calibration_MC);
	printf("MD:\t"); printf("%d\n",Calibration_MD);

}

int GetUncompensatedTemperature(){
    // Instruct device to perform a conversion.
    Write(Reg_Control, ControlInstruction_MeasureTemperature);
    // Wait for the conversion to complete.
    delay(5);
    //uint8_t* data = Read(Reg_AnalogConverterOutMSB, 2);

    int value = (Read(Reg_AnalogConverterOutMSB) << 8) | Read(Reg_AnalogConverterOutMSB+0x01);
    return value;
  }

  long GetUncompensatedPressure(){
    long pressure = 0;
    int loops = Oversample ? 3 : 1;

    for (int i = 0; i < loops; i++)
    {
        // Instruct device to perform a conversion, including the oversampling data.
        uint8_t CtrlByte = ControlInstruction_MeasurePressure + (OversamplingSetting << 6);
        Write(Reg_Control, CtrlByte);
        // Wait for the conversion
        delay(ConversionWaitTimeMs);
        // Read the conversion data.
       // uint8_t buffer[3];
        //Read2(Reg_AnalogConverterOutMSB, 3, buffer);

        // Collect the data (and push back the LSB if we are not sampling them).
        pressure = ((((long)Read(0xF6) <<16) | ((long)Read(0xF7) <<8) | ((long)Read(0xF8))) >> (8-OversamplingSetting));
      }
      return pressure / loops;
    }

    float CompensateTemperature(int uncompensatedTemperature){
      int temperature;
      int x2;
      long x1;
      x1 = (((long)uncompensatedTemperature - (long)Calibration_AC6) * (long)Calibration_AC5) >> 15;
      //printf("AC6: %d\n",Calibration_AC6);
      //printf("AC5: %d\n",Calibration_AC5);
      //printf("X1: %d\n",x1);
      x2 = ((long)Calibration_MC << 11) / (x1 + Calibration_MD);
      //printf("X2: %d\n",x2);

      int param_b5 = x1 + x2;
      //printf("b5: %d\n",param_b5);
      temperature = (int)((param_b5 + 8) >> 4);  /* temperature in 0.1 deg C*/
      float fTemperature = temperature;
      //printf("temperature: %d\n",temperature);
      fTemperature = fTemperature/10.0;
      //printf("fTemperature: %f\n",fTemperature);

    // Record this data because it is required by the pressure algorithem.
    LastTemperatureData = param_b5;
    LastTemperatureTime = millis();

    return fTemperature;
  }
  float GetTemperature(){
    return CompensateTemperature(GetUncompensatedTemperature());
  }

  long CompensatePressure(long uncompensatedPressure){
   //int msSinceLastTempReading = millis() - LastTemperatureTime;
    // Check to see if we have old temperature data.
    //if (msSinceLastTempReading > AcceptableTemperatureLatencyForPressure)
      //  GetTemperature(); // Refresh the temperature.

    // Data from the BMP180 datasheet to test algorithm.
    /*OversamplingSetting = 0;
    uncompensatedPressure = 23843;
    LastTemperatureData = 2399;
    Calibration_AC1 = 408;
    Calibration_AC2 = -72;
    Calibration_AC3 = -14383;
    Calibration_AC4 = 32741;
    Calibration_AC5 = 32757;
    Calibration_AC6 = 23153;
    Calibration_B1 = 6190;
    Calibration_B2 = 4;
    Calibration_MB = -32767;
    Calibration_MC = -8711;
    Calibration_MD = 2868;*/

    // Algorithm taken from BMP180 datasheet.
    long b6 = LastTemperatureData - 4000;
    long x1 = (Calibration_B2 * (b6 * b6) >> 12) >> 11;
    long x2 = (Calibration_AC2 * b6) >> 11;
    long x3 = x1 + x2;
    long b3 = (((((long)Calibration_AC1) * 4 + x3) << OversamplingSetting) + 2) >> 2;
    x1 = (Calibration_AC3 * b6) >> 13;
    x2 = (Calibration_B1 * (b6 * b6 >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    long b4 = (Calibration_AC4 * (unsigned long)((x3 + 32768))) >> 15;
    unsigned long b7 = (unsigned long)(((uncompensatedPressure - b3)) * (50000 >> OversamplingSetting));
    long p;
    if (b7 < 0x80000000)
    {
      p = ((b7 * 2) / b4);
    }
    else
    {
      p = ((b7 / b4) << 1);
    }

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p = p + ((x1 + x2 + 3791) >> 4);

    return p;
  }
  long GetPressure(){
    return CompensatePressure(GetUncompensatedPressure());
  }

  void SoftReset(){
    Write(Reg_SoftReset, SoftResetInstruction);
    delay(100);
  }


  float GetAltitude(float currentSeaLevelPressureInPa){
    // Get pressure in Pascals (Pa).
    float pressure = GetPressure();
    // Calculate altitude from sea level.
    float altitude = 44330.0 * (1.0 - pow(pressure / currentSeaLevelPressureInPa, 0.1902949571836346));
    return altitude;
  } 

  char* GetErrorText(int errorCode){
   if(ErrorCode_1_Num == 1)
   return ErrorCode_1;

   return "Error not defined.";
 }