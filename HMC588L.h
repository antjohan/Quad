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
int debug;
int refreshrate;

int offset_x;
int offset_y;
int offset_z;

char* to_mag_fifo = "/home/pi/tmp/to_mag_fifo";
char* from_mag_fifo = "/home/pi/tmp/from_mag_fifo";
int to_mag_fd;
int from_mag_fd;

//functions
void HMC5883L_init();
void cfg_init();
void printOut();
void connectFifos();
void writeOutput();
void checkPipe();
int GetX();
int GetY();
int GetZ();
double computeHeading(int x, int y, int z);
void sample();
void calibrate();
