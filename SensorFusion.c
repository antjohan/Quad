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

	#define MAX_BUF 128 //Amount of characters that can be sent through the fifos
	 
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

	FILE *to_log_file;
	char* log_path = "/home/pi/logs/fusionlog.txt";


	//variables
	double BaroInitialHeight=0;
	double gpsdata[6];

	clock_t t1;

	void sfinit();
	void InitPipes();
	void updateSensorValues();
	double getHeight();
	double getBHeight();
	double getUHeight();
	double getHeading();
	//double * getCoordinate();
	void refreshGPS();
	double latitude();
	double longitude();
	int quality();
	int nsat();
	double sdn();
	double sde();
	void commandSensor(char * sensor, char * command);
	void updateLog();

	void sfinit(){ 
		InitPipes();
		BaroInitialHeight=getBHeight();
	}

	void InitPipes(){
			//name of all fifos, will need to be matching in respective sensor programs
		char* from_baro_fifo = "/home/pi/tmp/from_baro_fifo";
		char* from_ultra_fifo = "/home/pi/tmp/from_ultra_fifo";
		char* from_mag_fifo = "/home/pi/tmp/from_mag_fifo";
		char* from_gps_fifo = "/home/pi/tmp/from_gps_fifo";

		char* to_baro_fifo = "/home/pi/tmp/to_baro_fifo";
		char* to_ultra_fifo = "/home/pi/tmp/to_ultra_fifo";
		char* to_mag_fifo = "/home/pi/tmp/to_mag_fifo";
		char* to_gps_fifo = "/home/pi/tmp/to_gps_fifo";
		
		//Clear all to-fifos, should they exist!
		unlink(to_baro_fifo);
		unlink(to_ultra_fifo);
		unlink(to_mag_fifo);
		unlink(to_gps_fifo);

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
		if (mkfifo(to_gps_fifo,0666)==-1){
			printf("sf_to_gps_fifo=error: %s\n",strerror(errno));
		}
	//initialize all sensors and corresponding fifos
		//run sensors manually or script them to start!

	//initialize all rdonly- fifos
		printf("Initializing fifos...\n");

		from_ultra_fd=open(from_ultra_fifo, O_RDONLY);
	 	if (from_ultra_fd==-1){
			printf("pipe_from_ultra=error: %s\n",strerror(errno));
		} else {
			//printf("pipe_from_ultra=connected\n");
		}

		from_baro_fd=open(from_baro_fifo, O_RDONLY);
		if (from_baro_fd==-1){
	    	printf("pipe_from_baro=error: %s\n",strerror(errno));
		} else {
			//printf("pipe_from_baro=connected\n");
		}
	 	
	 	from_mag_fd=open(from_mag_fifo, O_RDONLY);
		if (from_mag_fd==-1){
	    	printf("pipe_from_mag=error: %s\n",strerror(errno));
		} else {
			//printf("pipe_from_mag=connected\n");
		}

		from_gps_fd=open(from_gps_fifo, O_RDONLY);
		if (from_gps_fd==-1){
	    	printf("pipe_from_gps=error: %s\n",strerror(errno));
		} else {
			//printf("pipe_from_gps=connected\n");
		}

	 	////from fifos connected
	 	//printf("from_fifos=connected!\n");

	 	delay(300);
	 	to_ultra_fd=open(to_ultra_fifo, O_WRONLY);
	 	if (to_ultra_fd==-1){
			printf("pipe_to_ultra=error: %s\n",strerror(errno));
		} else {
			printf("pipe_to_ultra=connected\n");
		}
		delay(200);
		to_baro_fd=open(to_baro_fifo, O_WRONLY);
		if (to_baro_fd==-1){
	    	printf("pipe_to_baro=error: %s\n",strerror(errno));
		} else {
			printf("pipe_to_baro=connected\n");
		}
	 	delay(200);
		to_mag_fd=open(to_mag_fifo, O_WRONLY);
		if (to_mag_fd==-1){
	    	printf("pipe_to_mag=error: %s\n",strerror(errno));
		} else {
			printf("pipe_to_mag=connected\n");
		}
		delay(200);
		to_gps_fd=open(to_gps_fifo, O_WRONLY);
		if (to_gps_fd==-1){
	    	printf("pipe_to_gps=error: %s\n",strerror(errno));
		} else {
			printf("pipe_to_gps=connected\n");
		}


	 	//setting non-block
	 	fcntl(from_baro_fd, F_SETFL, O_NONBLOCK);
	 	fcntl(from_ultra_fd, F_SETFL, O_NONBLOCK);
	 	fcntl(from_mag_fd, F_SETFL, O_NONBLOCK);
	 	fcntl(from_gps_fd, F_SETFL, O_NONBLOCK);
	 	fcntl(to_baro_fd, F_SETFL, O_NONBLOCK);
	 	fcntl(to_ultra_fd, F_SETFL, O_NONBLOCK);
	 	fcntl(to_mag_fd, F_SETFL, O_NONBLOCK);
	 	fcntl(to_gps_fd, F_SETFL, O_NONBLOCK);
	 	//*/
	 	printf("PIPES CONNECTED SUCCESSFULLY\n");
	//initialize flight log
		to_log_file=fopen(log_path, "w+");
		delay(200);
		if (fprintf(to_log_file,"Time: Barometer: Ultrasonic: Height: Magnetometer: Latitude: Longitude: SatQuality: NumSat: SDN: SDE:\n")==-1){
			printf("initialize_fusion_log=error: %s\n",strerror(errno));
		}
		fclose(to_log_file);
	}

	double getHeight(double uh, double bh){ //returns the best value for height, using both barometer/ultrasonic sensor input
		//double uh=getUHeight(); //ultrasonic height
		//double bh=getBHeight(); //barometer height
		double offset=0.0;
		if (uh<450 && uh>0){
			BaroInitialHeight=bh-(uh/100.0);
			return((uh-offset)/100.0);
		} else {
			return (bh-BaroInitialHeight);
		} 

	}
	double getBHeight(){ //take a value directly from barometer
		commandSensor("baro", "read");
		double bh=0; //barometer height
		char barobuffer[MAX_BUF];
		int a = read(from_baro_fd,barobuffer,MAX_BUF);
		sscanf(barobuffer, "%lf", &bh);
		return bh;

	}
	double getUHeight(){ //take a value directly from ultrasonic sensor
		commandSensor("ultra", "read");
		double uh=-1; //ultrasonic height
		char ultrabuffer[MAX_BUF];
		int a = read(from_ultra_fd,ultrabuffer,MAX_BUF);
		if (a==-1){
			//printf("UHeightReadError: %s\n",strerror(errno));
		}	
		sscanf(ultrabuffer, "%lf", &uh);
		return (uh);
	}

	double getHeading(){ //take a value directly from magnetometer
		commandSensor("mag", "read");
		double br=-1;
		char magbuffer[MAX_BUF];
		read(from_mag_fd, magbuffer, MAX_BUF);
		sscanf(magbuffer, "%lf", &br);
		return(br);
	}
	/*
	return array gpsdata[] constists of (in the following order)
	latitude(deg) longitude(deg)  height(m)   Q  ns   sdn(m)   sde(m)   sdu(m)  sdne(m)  sdeu(m)  sdun(m) age(s)  ratio
	refresh only contains lat,long,qual,nsat,sdn,sde
	*/
	void refreshGPS(){ 
		commandSensor("gps", "read");
		double newgpsdata[6]={0};
		char gpsbuffer[MAX_BUF];
		char * str=&gpsbuffer;
	 	if(read (from_gps_fd, gpsbuffer, MAX_BUF)>0){  // read up to 100 characters if ready to read
	        printf("String read from GPS: %s\n", gpsbuffer);
	        char * end;
			for (int i =0;i<6;++i){
		        newgpsdata[i] = strtod(str, &end);
		        str=end;
		    }
		}
		for (int i=0;i<6;++i){
			gpsdata[i]=newgpsdata[i];
		}
	}
	double latitude(){
		return(gpsdata[1]);
	}
	double longitude(){
		return(gpsdata[2]);
	}
	int quality(){
		return((int)gpsdata[3]);
	}
	int nsat(){
		return((int)gpsdata[4]);
	}
	double sdn(){
		return(gpsdata[5]);
	}
	double sde(){
		return(gpsdata[6]);
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
		} else if (strcmp(sensorstr, str4)==0){
			sensor_fd=to_baro_fd;
		} else {
			printf("Incorrect sensor string\n");
		}
		int tmp = write(sensor_fd,sendstr,sizeof(sendstr));
	}

	void updateLog(double bheight, double uheight, double height, double heading, double latitude, double longitude, double satquality, double nsat, double sdn, double sde){//enters all current sensor data into fusionlog
		double current_time_seconds=millis()/1000.0;
		to_log_file=fopen(log_path, "a");
		if (fprintf(to_log_file,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",current_time_seconds,bheight,uheight,height,heading,latitude,longitude,satquality,nsat,sdn,sde)==-1){
			printf("write_to_log=error: %s\n",strerror(errno));
		}
		fclose(to_log_file);
	}