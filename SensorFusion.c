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


int from_baro_fd;
int from_ultra_fd;
int from_mag_fd;
int from_gps_fd;

int to_baro_fd;
int to_ultra_fd;
int to_mag_fd;
int to_gps_fd;

int to_log_file;

//variables
double height; //height from ultrasonic/barometer
double bearing; //bearing from magnetometer
double coordinate[3]; //lat, long, quality from gps
double speed; //info from gps


void sfinit(){ 
	//name of all fifos, will need to be matching in respective sensor programs
	char* from_baro_fifo = "/home/pi/tmp/from_baro_fifo";
	char* from_ultra_fifo = "/home/pi/tmp/from_ultra_fifo";
	char* from_mag_fifo = "/home/pi/tmp/from_mag_fifo";
	char* from_gps_fifo = "/home/pi/tmp/from_gps_fifo";

	char* to_baro_fifo = "/home/pi/tmp/to_baro_fifo";
	char* to_ultra_fifo = "/home/pi/tmp/to_ultra_fifo";
	char* to_mag_fifo = "/home/pi/tmp/to_mag_fifo";
	char* to_gps_fifo = "/home/pi/tmp/to_gps_fifo";
	

	char* to_log_file = "/home/pi/fusionlog.txt";

	//Clear all to-fifos, should they exist!
	unlink(to_baro_fifo);
	delay(200);
	unlink(to_ultra_fifo);
	delay(200);
	unlink(to_mag_fifo);
	delay(200);
	unlink(to_gps_fifo);
	delay(200);

//initialize all sensors and corresponding fifos
	//run sensors manually or script them to start!

//initialize all rdonly- fifos
	printf("Initializing fifos...\n");

	from_ultra_fd=open(from_ultra_fifo, O_RDONLY);
 	if (from_ultra_fd==-1){
		printf("from_ultra_fifo=error: %s\n",strerror(errno));
	} else {
		printf("from_ultra_fifo=open\n");
	}

	from_baro_fd=open(from_baro_fifo, O_RDONLY);
	if (from_baro_fd==-1){
    	printf("from_baro_fifo=error: %s\n",strerror(errno));
 	} else {
 		printf("from_baro_fifo=open\n");
 	}
 	/*
 	from_mag_fd=open(from_mag_fifo, O_RDONLY);
 	if (from_mag_fd==-1){
		printf("from_mag_fifo_error: %s\n",strerror(errno));
	} else {
		printf("from_mag_fifo=open\n");
	}

	from_gps_fd=open(from_gps_fifo, O_RDONLY);
	if (from_gps_fd==-1){
    	printf("from_gps_fifo_error: %s\n",strerror(errno));
 	} else {
 		printf("from_gps_fifo=open\n");
 	}
	*/
 	printf("from_fifos=connected!\n");
 	if (mkfifo(to_ultra_fifo,0666)==-1){
 		printf("sf_make_to_ultra=error: %s\n",strerror(errno));
 	}
 	delay(300);
 	to_ultra_fd=open(to_ultra_fifo, O_WRONLY);
 	if (to_ultra_fd==-1){
		printf("sf_to_ultra_fifo=error: %s\n",strerror(errno));
	} else {
		printf("sf_to_ultra_fifo=open\n");
	}
	delay(200);
	if (mkfifo(to_baro_fifo,0666)==-1){
		printf("sf_to_baro_fifo=error: %s\n",strerror(errno));
	}
 	delay(300);
	to_baro_fd=open(to_baro_fifo, O_WRONLY);
	if (to_baro_fd==-1){
    	printf("sf_to_baro_fifo=error: %s\n",strerror(errno));
 	} else {
 		printf("sf_to_baro_fifo=open\n");
 	}
 	/*

 	mkfifo(to_mag_fifo,0666);
 	delay(200);
 	to_mag_fd=open(to_mag_fifo, O_WRONLY);
 	if (to_mag_fd==-1){
		printf("to_mag_fifo=error: %s\n",strerror(errno));
	} else {
		printf("to_mag_fifo=open\n");
	}

	mkfifo(to_gps_fifo,0666);
 	delay(200);
	to_gps_fd=open(to_gps_fifo, O_WRONLY);
	if (to_gps_fd==-1){
    	printf("from_gps_fifo=error: %s\n",strerror(errno));
 	} else {
 		printf("from_gps_fifo=open\n");
 	}
	*/

 	printf("to_fifos=connected!\nFIFOS CONNECTED SUCCESSFULLY\n");

//initialize flight log
	//fusionlogfd=open(fusionlog, O_WRONLY);

	//initialize wronly fifos

}

double getHeight(){ //returns the best value for height, using both barometer/ultrasonic sensor input
	double uh; //ultrasonic height
	double bh; //barometer height
	//char ultrasonicbuffer[MAX_BUF];
	char barometerbuffer[MAX_BUF];
	//read(ultrasonicfifofd,ultrasonicbuffer,MAX_BUF);
	read(from_baro_fd,barometerbuffer,MAX_BUF);

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
	double bh; //barometer height
	char barometerbuffer[MAX_BUF];
	read(from_baro_fd,barometerbuffer,MAX_BUF);
	sscanf(barometerbuffer, "%lf", &bh);
	printf("bh: %lf\n",bh);
	return bh;

}
double getUHeight(){
	long uh; //ultrasonic height
	char ultrasonicbuffer[MAX_BUF];
	int a = read(from_ultra_fd,ultrasonicbuffer,MAX_BUF);
	if (a==-1){
		printf("UHeightReadError: %s\n",strerror(errno));
	}	
	sscanf(ultrasonicbuffer, "%ld", &uh);
	//printf("uh: %ld\n",uh);
	return (uh);
}

double getBearing(){ //returns current bearing based on magnetometric sensor output
	double br;
	char magnetometerbuffer[MAX_BUF];
	read(from_mag_fd, magnetometerbuffer, MAX_BUF);
	sscanf(magnetometerbuffer, "%lf", &br);
	return(br);

}
double * getCoordinate(){
	char gpsbuffer[MAX_BUF];
	read(from_gps_fd, gpsbuffer, MAX_BUF);
	sscanf(gpsbuffer, "%lf, %lf, %lf", &coordinate);
	return(coordinate);
}

void commandSensor(char * sensor, char * command){//sensor = ultra, baro, mag or gps
	int sensor_fd;
	printf("%s%s\n",sensor,command);
	if (strcmp(sensor, "mag")==0){
		sensor_fd=to_mag_fd;
	} else if (strcmp(sensor, "ultra")==0){
		sensor_fd=to_ultra_fd;
	} else if (strcmp(sensor, "gps")==0){
		sensor_fd=to_gps_fd;
	}else if (strcmp(sensor, "baro")==0){
		sensor_fd=to_baro_fd;
	}
}

void updateLog(){//enters all current sensor data into fusionlog


}