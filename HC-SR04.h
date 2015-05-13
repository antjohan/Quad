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
 
#define TRIG 10
#define ECHO 11
#define MAX_BUF 256


int sampling=1; //ultra will loop and report sampled data
double currentHeight;
int avgsamples=10;
int latestdata[avgsamples]={0};


//functions
void sample();
double movingAvg(int newvalue);
int getCM();
void connectFifos();
void checkPipe();
void getCMloop();
void HCSR04_init();
int getUltra();
void getCMloop();

int from_ultra_fd;
int to_ultra_fd;