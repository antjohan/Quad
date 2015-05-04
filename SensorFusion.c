#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>


//#include <libconfig.h>

#include <unistd.h>

#define MAX_BUF 56 //Amount of characters that can be sent through the fifos
 
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
double heading; //heading from magnetometer
double coordinate[3]; //lat, long, quality from gps
double speed; //info from gps

clock_t t1;


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
	

	char* to_log_file = "/home/pi/logs/fusionlog.txt";

	//Clear all to-fifos, should they exist!
	unlink(to_baro_fifo);
	unlink(to_ultra_fifo);
	unlink(to_mag_fifo);
	unlink(to_gps_fifo);
//  	unlink(from_mag_fifo);
  //	unlink(from_gps_fifo);
  	delay (1000);

	if (mkfifo(to_ultra_fifo,0666)==-1){
 		printf("sf_make_to_ultra=error: %s\n",strerror(errno));
 	}
	if (mkfifo(to_baro_fifo,0666)==-1){
		printf("sf_to_baro_fifo=error: %s\n",strerror(errno));
	}
	if (mkfifo(to_mag_fifo,0666)==-1){
		printf("sf_to_mag_fifo=error: %s\n",strerror(errno));
	}
//initialize all sensors and corresponding fifos
	//run sensors manually or script them to start!

//initialize all rdonly- fifos
	printf("Initializing fifos...\n");

	from_ultra_fd=open(from_ultra_fifo, O_RDONLY);
 	if (from_ultra_fd==-1){
		printf("sf_from_ultra_fifo=error: %s\n",strerror(errno));
	} else {
		printf("sf_from_ultra_fifo=open\n");
	}

	from_baro_fd=open(from_baro_fifo, O_RDONLY);
	if (from_baro_fd==-1){
    	printf("sf_from_baro_fifo=error: %s\n",strerror(errno));
 	} else {
 		printf("sf_from_baro_fifo=open\n");
 	}
 	
 	from_mag_fd=open(from_mag_fifo, O_RDONLY);
	if (from_mag_fd==-1){
    	printf("sf_from_mag_fifo=error: %s\n",strerror(errno));
 	} else {
 		printf("sf_from_mag_fifo=open\n");
 	}

 	////from fifos connected
 	printf("from_fifos=connected!\n");

 	delay(300);
 	to_ultra_fd=open(to_ultra_fifo, O_WRONLY);
 	if (to_ultra_fd==-1){
		printf("sf_to_ultra_fifo=error: %s\n",strerror(errno));
	} else {
		printf("sf_to_ultra_fifo=open\n");
	}
	delay(200);
	to_baro_fd=open(to_baro_fifo, O_WRONLY);
	if (to_baro_fd==-1){
    	printf("sf_to_baro_fifo=error: %s\n",strerror(errno));
 	} else {
 		printf("sf_to_baro_fifo=open\n");
 	}
 	delay(200);
	to_mag_fd=open(to_mag_fifo, O_WRONLY);
	if (to_mag_fd==-1){
    	printf("sf_to_mag_fifo=error: %s\n",strerror(errno));
 	} else {
 		printf("sf_to_mag_fifo=open\n");
 	}

 	/*


	*/
 	/*setting non-block
 	fcntl(from_baro_fd, F_SETFL, O_NONBLOCK);
 	fcntl(from_ultra_fd, F_SETFL, O_NONBLOCK);
 	//fcntl(from_mag_fd, F_SETFL, O_NONBLOCK);
 	//fcntl(from_gps_fd, F_SETFL, O_NONBLOCK);
 	fcntl(to_baro_fd, F_SETFL, O_NONBLOCK);
 	fcntl(to_ultra_fd, F_SETFL, O_NONBLOCK);
 	//fcntl(to_mag_fd, F_SETFL, O_NONBLOCK);
 	//fcntl(to_gps_fd, F_SETFL, O_NONBLOCK);
 	*/
 	printf("to_fifos=connected!\nFIFOS CONNECTED SUCCESSFULLY\n");
//initialize flight log
	to_log_file=open(to_log_file, O_RDWR);
	t1=clock();
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
	commandSensor("baro", "read");
	double bh; //barometer height
	char barobuffer[MAX_BUF];
	int a = read(from_baro_fd,barobuffer,MAX_BUF);
	sscanf(barobuffer, "%lf", &bh);
	return bh;

}
double getUHeight(){
	commandSensor("ultra", "read");
	long uh; //ultrasonic height
	char ultrabuffer[MAX_BUF];
	int a = read(from_ultra_fd,ultrabuffer,MAX_BUF);
	if (a==-1){
		printf("UHeightReadError: %s\n",strerror(errno));
	}	
	sscanf(ultrabuffer, "%ld", &uh);
	return (uh);
}

double getHeading(){ //returns current heading based on magnetometric sensor output
	commandSensor("mag", "read");
	double br;
	char magbuffer[MAX_BUF];
	read(from_mag_fd, magbuffer, MAX_BUF);
	sscanf(magbuffer, "%lf", &br);
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

	char str1[10];
	char str2[10];
	char str3[10];
	char str4[10];

	char sensorstr[10];
	char sendstr[10];

	strcpy(str1,"mag");
	strcpy(str2,"ultra");
	strcpy(str3,"gps");
	strcpy(str4,"baro");

	strcpy(sensorstr,sensor);
	sprintf(sendstr,"%s",command);


	if (strcmp(sensorstr, str1)==0){
		sensor_fd=to_mag_fd;
	} else if (strcmp(sensorstr, str2)==0){
		sensor_fd=to_ultra_fd;
	} else if (strcmp(sensorstr, str3)==0){
		sensor_fd=to_gps_fd;
	}else if (strcmp(sensorstr, str4)==0){
		sensor_fd=to_baro_fd;
	} else {
		printf("Incorrect sensor string\n");
	}
	int tmp = write(sensor_fd,sendstr,sizeof(sendstr));
}

void updateLog(){//enters all current sensor data into fusionlog
	char logstr[100];
	printf("cps: %f", CLOCKS_PER_SEC);
	sprintf(logstr,"T %lf B %lf U %lf M %lf\n",(float)t1/(float)CLOCKS_PER_SEC,getBHeight(),getUHeight(),getHeading());
	printf("log entry: %s\n",logstr);
	write(to_log_file,logstr,sizeof(logstr));
}