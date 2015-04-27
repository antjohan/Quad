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

int InitialPressurePa;
int barometerfifofd;

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

//Functions
void Initialize();
void sample();
void Write();
uint8_t Read(int address);
uint8_t SetResolution(uint8_t sampleResolution, bool oversample);
void PrintCalibrationData();
int GetUncompensatedTemperature();
long GetUncompensatedPressure();
float CompensateTemperature(int uncompensatedTemperature);
float GetTemperature();
long CompensatePressure(long uncompensatedPressure);
long GetPressure();
float GetAltitude(float currentSeaLevelPressureInPa);
char* GetErrorText(int errorCore);