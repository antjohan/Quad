#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 256 //Amount of characters that can be sent through the fifos
 
int barometerfifofd;
int ultrasonicfifofd;
int magnetometerfifofd;
int gpsfifofd;
int flightlogfd;

//variables
double height; //height form ultrasonic/barometer
double bearing; //bearing from magnetometer
double[3] coordinate; //lat, long, quality from gps
double speed; //info from gps



void init(){ 
	//name of all fifos, will need to be matching in respective sensor programs
	char * barometerfifo = "./tmp/barometerfifo";
	char * ultrasonicfifo = "./tmp/ultrasonicfifo"
	char * magnetometerfifo = "./tmp/magnetometerfifo";
	char * gpsfifo = "./tmp/gpsfifo";
	char * flightlog = "./fusionlog.txt";



//initialize all sensors and wronly- fifos
	//run sensors manually or script them to start!



//initialize all rdonly- fifos
	barometerfifofd=open(barometerfifo, O_RDONLY);
	ultrasonicfifofd=open(ultrasonicfifo, O_RDONLY);
	magnetometerfifofd=open(magnetometerfifo, O_RDONLY);
	gpsfifofd=open(gpsfifo,O_RDONLY);	

//initialize flight log
	fusionlogfd=open(fusionlog, O_WRONLY);
}

double getHeight(){ //returns the best value for height, using both barometer/ultrasonic sensor input
	double uh; //ultrasonic height
	double bh; //barometer height

	char ultrasonicbuffer[MAX_BUF];
	char barometerbuffer[MAX_BUF];
	read(ultrasonicfifofd,ultrasonicbuffer,MAX_BUF);
	read(barometerfifofd,barometerbuffer,MAX_BUF);

	sscanf(ultrasonicbuffer, "%lf", &uh);
	sscanf(barometerbuffer, "%lf", &bh);

	//logic for which value to return

	if (uh<4){//easiest possible...
		return (uh);
	} else {
		return (bh);
	}


}

double getBearing(){ //returns current bearing based on magnetometric sensor output


}
void updateLog(){//enters all current sensor data into fusionlog

}