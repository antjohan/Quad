#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


//#include <libconfig.h>

#include <unistd.h>

#define MAX_BUF 256 //Amount of characters that can be sent through the fifos
 
 //char * configfile= "./config.txt"
 //config_t * config;


int barometerfifofd;
int ultrasonicfifofd;
int magnetometerfifofd;
int gpsfifofd;
int fusionlogfd;

//variables
double height; //height from ultrasonic/barometer
double bearing; //bearing from magnetometer
double coordinate[3]; //lat, long, quality from gps
double speed; //info from gps


void sfinit(){ 
	//name of all fifos, will need to be matching in respective sensor programs
	char* barometerfifo = "/tmp/barometerfifo";
	char* ultrasonicfifo = "/tmp/ultrasonicfifo";
	char* magnetometerfifo = "/tmp/magnetometerfifo";
	char* gpsfifo = "/tmp/gpsfifo";
	char* fusionlog = "/fusionlog.txt";



//initialize all sensors and wronly- fifos
	//run sensors manually or script them to start!



//initialize all rdonly- fifos
	//wait for created baro fifo
	//printf("connecting-baro-fifo...\n");
    char buf[14];
  	while(strcmp(buf,"baro-fifo-open")==0){
  		printf("Looking for: baro-fifo-open");
   		fgets(buf,14,stdin);
  	}
	barometerfifofd=open(barometerfifo, O_RDONLY);
	if (barometerfifofd==-1){
    	printf("barometerfifofoerror: %s\n",strerror(errno));
 	} else {
  		printf("baro-fifo-connected\n");
 	}

 	char buf[15];
  	while(strcmp(buf,"ultra-fifo-open")==0){
  		printf("Looking for: ultra-fifo-open");
   		fgets(buf,15,stdin);
   		printf("baro-fifo-connected\n");
  	}
		ultrasonicfifofd=open(ultrasonicfifo, O_RDONLY);
 	if (ultrasonicfifofd==-1){
		printf("ultrasonicfifofoerror: %s\n",strerror(errno));
	} else {
		printf("ultra-fifo-connected\n");
	}
	//magnetometerfifofd=open(magnetometerfifo, O_RDONLY);
	//gpsfifofd=open(gpsfifo,O_RDONLY);	

//initialize flight log
	//fusionlogfd=open(fusionlog, O_WRONLY);
}

double getHeight(){ //returns the best value for height, using both barometer/ultrasonic sensor input
	double uh; //ultrasonic height
	double bh; //barometer height
	//char ultrasonicbuffer[MAX_BUF];
	char barometerbuffer[MAX_BUF];
	//read(ultrasonicfifofd,ultrasonicbuffer,MAX_BUF);
	read(barometerfifofd,barometerbuffer,MAX_BUF);

	//sscanf(ultrasonicbuffer, "%lf", &uh);
	sscanf(barometerbuffer, "%lf", &bh);

	//logic for which value to return

/*
	if (uh<4){//easiest possible...
		return (uh);
	} else {
		return (bh);
	}
*/
	height = bh;
	return bh;

}
double getBHeight(){
	printf("hej11\n");
	double bh; //barometer height
	char barometerbuffer[MAX_BUF];
	read(barometerfifofd,barometerbuffer,MAX_BUF);
	sscanf(barometerbuffer, "%lf", &bh);
	return bh;

}
double getUHeight(){
	printf("hej10\n");
	long uh; //ultrasonic height
	char ultrasonicbuffer[MAX_BUF];
	read(ultrasonicfifofd,ultrasonicbuffer,MAX_BUF);
	sscanf(ultrasonicbuffer, "%lf", &uh);
	return (uh);
}

double getBearing(){ //returns current bearing based on magnetometric sensor output
	double br;
	char magnetometerbuffer[MAX_BUF];
	read(magnetometerfifofd, magnetometerbuffer, MAX_BUF);
	sscanf(magnetometerbuffer, "%lf", &br);
	return(br);

}
double * getCoordinate(){
	char gpsbuffer[MAX_BUF];
	read(gpsfifofd, gpsbuffer, MAX_BUF);
	sscanf(gpsbuffer, "%lf, %lf, %lf", &coordinate);
	return(coordinate);
}

void updateLog(){//enters all current sensor data into fusionlog


}