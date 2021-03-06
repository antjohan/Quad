#include "BMP180.h"
#include <libconfig.h>

int main(){
    cfg_init();
    BMP180_init();
    connectFifos();
    printOut();
    sample();
}

void cfg_init(){
    config_t cfg;
    config_setting_t *bmp180, *root;
    const char *str;
    config_init(&cfg);
    
    
    if(! config_read_file(&cfg, "c.cfg"))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        //return(EXIT_FAILURE);
    }
    bmp180 = config_lookup(&cfg, "BMP180");
    
    int os = 0;
    int oss = 0;
    
    config_setting_lookup_bool(bmp180, "Oversample", &os);
    
    if(os){
        Oversample = true;
    }else{
        Oversample = false;
    }
    
    config_setting_lookup_bool(bmp180, "Debug", &debug);
    config_setting_lookup_int(bmp180, "OversamplingSetting", &oss);
    config_setting_lookup_int(bmp180, "refreshrate", &refreshrate);
    config_setting_lookup_int(bmp180, "InitialPressurePa", &InitialPressurePa);
    OversamplingSetting = (uint8_t)oss;
    
    config_destroy(&cfg);
}

void printOut(){
    if(debug){
        
        printf("\n----BMP180 Configuration data----\n");
        printf("OversamplingSetting: %d\n", OversamplingSetting);
        printf("refreshrate: %d\n", refreshrate);
        printf("InitialPressurePa: %d\n", InitialPressurePa);
        //if(Oversample)
        //printf("Oversampel = true\n");
        //else
        //printf("Oversampel = false\n");
        
        printf("\n");
        
        printf("Calibration_AC1: %d\n", Calibration_AC1);
        printf("Calibration_AC2: %d\n", Calibration_AC2);
        printf("Calibration_AC3: %d\n", Calibration_AC3);
        printf("Calibration_AC4: %d\n", Calibration_AC4);
        printf("Calibration_AC5: %d\n", Calibration_AC5);
        printf("Calibration_AC6: %d\n", Calibration_AC6);
        printf("Calibration_B1: %d\n", Calibration_B1);
        printf("Calibration_B2: %d\n", Calibration_B2);
        printf("Calibration_MB: %d\n", Calibration_MB);
        printf("Calibration_MC: %d\n", Calibration_MC);
        printf("Calibration_MD: %d\n", Calibration_MD);
        
        printf("---------------------------------\n\n");
        
    }
    
}
void BMP180_init(){
    wiringPiSetupSys();
    ConversionWaitTimeMs = 5;
    //OversamplingSetting = 0;
    //Oversample = false;
    LastTemperatureTime = -1000;
    LastTemperatureData = 0;
    calibrate();
    
    AcceptableTemperatureLatencyForPressure = 1000;
    //InitialPressurePa=GetPressure();
}
void connectFifos(){
    //open fifo
    char* from_baro_fifo = "/home/pi/tmp/from_baro_fifo";
    char* to_baro_fifo = "/home/pi/tmp/to_baro_fifo";
    //delete in case it already exists
    unlink(from_baro_fifo);
    delay(200);
    if (mkfifo(from_baro_fifo,0666)==-1){
        printf("b_make_from_baro=error: %s\n",strerror(errno));
    }
    
    from_baro_fd=open(from_baro_fifo, O_WRONLY);
    if (from_baro_fd==-1){
        printf("pipe_from_baro=error: %s\n",strerror(errno));
    } else {
        printf("pipe_from_baro=connected\n");
    }
    
    delay(2000);
    to_baro_fd=open(to_baro_fifo, O_RDONLY);
    if (to_baro_fd==-1){
        printf("pipe_to_baro=error: %s\n",strerror(errno));
    } else {
        // printf("pipe_to_baro=connected\n");
    }
    
}
void calibrate(){
    
    config_t cfg;
    config_setting_t *AC1, *AC2, *AC3, *AC4, *AC5, *AC6, *B1, *B2, *MB, *MC, *MD;
    const char *str;
    config_init(&cfg);
    
    
    if(! config_read_file(&cfg, "c.cfg")){
        
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        //return(EXIT_FAILURE);
    }
    
    BMP180_Sensor =  wiringPiI2CSetup (BMP180_Address);
    SetResolution(BMP180_Mode_UltraHighResolution, Oversample);
    
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
    
    AC1 = config_lookup(&cfg, "BMP180.Calibration_AC1");
    AC2 = config_lookup(&cfg, "BMP180.Calibration_AC2");
    AC3 = config_lookup(&cfg, "BMP180.Calibration_AC3");
    AC4 = config_lookup(&cfg, "BMP180.Calibration_AC4");
    AC5 = config_lookup(&cfg, "BMP180.Calibration_AC5");
    AC6 = config_lookup(&cfg, "BMP180.Calibration_AC6");
    B1 = config_lookup(&cfg, "BMP180.Calibration_B1");
    B2 = config_lookup(&cfg, "BMP180.Calibration_B2");
    MB = config_lookup(&cfg, "BMP180.Calibration_MB");
    MC = config_lookup(&cfg, "BMP180.Calibration_MC");
    MD = config_lookup(&cfg, "BMP180.Calibration_MD");
    
    config_setting_set_int(AC1, Calibration_AC1);
    config_setting_set_int(AC2, Calibration_AC2);
    config_setting_set_int(AC3, Calibration_AC3);
    config_setting_set_int(AC4, Calibration_AC4);
    config_setting_set_int(AC5, Calibration_AC5);
    config_setting_set_int(AC6, Calibration_AC6);
    config_setting_set_int(B1, Calibration_B1);
    config_setting_set_int(B2, Calibration_B2);
    config_setting_set_int(MB, Calibration_MB);
    config_setting_set_int(MC, Calibration_MC);
    config_setting_set_int(MD, Calibration_MD);
    
    config_write_file(&cfg, "c.cfg");
    config_destroy(&cfg);
}

void sample(){
    while(sampling==1){
        checkPipe();
        AbsoluteAltitude=movingAvg(GetAltitude(InitialPressurePa));
        //printf("ABSOLUTE ALTITUDE", RelativeAltitude);
    }
}

double movingAvg(float newvalue){
  double tmp=0.0;
  for (int i=0;i<avgsamples-1;++i){
    latestdata[i]=latestdata[i+1];
    tmp=tmp+latestdata[i+1];
    //printf("i:%d sum:%lf new:%d latesti:%lf\n",i,tmp,newvalue,latestdata[i]);
  }
    latestdata[avgsamples-1]=newvalue;
    tmp=tmp+(double)newvalue;
    //printf("filtered: %lf\n", tmp);
    return((double)tmp/(double)avgsamples);
}

void writeOutput(){
    char WriteBuf[MAX_BUF];
    sprintf(WriteBuf,"%lf",AbsoluteAltitude);
    write(from_baro_fd,WriteBuf,sizeof(WriteBuf));
}

void checkPipe(){
    char buffer[10];
    char str1[10];
    char str2[10];
    
    strcpy(str1,"ping");
    strcpy(str2,"read");
 if (read(to_baro_fd, buffer, 10)>0){
        if (strcmp(buffer,str1)==0){ //ping
            printf("Barometer says hi! :D\n");
        } else if(strcmp(buffer,str2)==0) { //read
            writeOutput();
        }
    }
}

void Write(int address, int data){
    wiringPiI2CWriteReg8(BMP180_Sensor,address, data);
}

uint8_t Read(int address){
    return wiringPiI2CReadReg8(BMP180_Sensor, address);
}

uint8_t SetResolution(uint8_t sampleResolution, bool oversample){
    OversamplingSetting = sampleResolution;
    Oversample = oversample;
    switch (sampleResolution){
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
    
    for (int i = 0; i < loops; i++){
        // Instruct device to perform a conversion, including the oversampling data.
        uint8_t CtrlByte = ControlInstruction_MeasurePressure + (OversamplingSetting << 6);
        Write(Reg_Control, CtrlByte);
        // Wait for the conversion
        delay(ConversionWaitTimeMs);
        // Read the conversion data.
        // uint8_t buffer[3];
        //Read2(Reg_AnalogConverterOutMSB, 3, buffer);
        
        // Collect the data (and push back the LSB if we are not sampling them).
        pressure =pressure+ ((((long)Read(0xF6) <<16) | ((long)Read(0xF7) <<8) | ((long)Read(0xF8))) >> (8-OversamplingSetting));
    }
    return pressure / loops;
}

float CompensateTemperature(int uncompensatedTemperature){
    int temperature;
    int x2;
    long x1;
    x1 = (((long)uncompensatedTemperature - (long)Calibration_AC6) * (long)Calibration_AC5) >> 15;
    x2 = ((long)Calibration_MC << 11) / (x1 + Calibration_MD);
    int param_b5 = x1 + x2;
    temperature = (int)((param_b5 + 8) >> 4);  /* temperature in 0.1 deg C*/
    float fTemperature = temperature;
    fTemperature = fTemperature/10.0;
    
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
    float altitude = 44330.0 * (1.0 - pow(pressure / currentSeaLevelPressureInPa, 0.19029495718363465568));
    return altitude;
}

char* GetErrorText(int errorCode){
    if(ErrorCode_1_Num == 1)
        return ErrorCode_1;
    
    return "Error not defined.";
}