#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiI2C.h>
#include <time.h>

int HoverOffset = 0;
int ThrottleStep = 0;
int YawSpeed = 0;
int PitchSpeed = 0;
int RollSpeed = 0;
int Debug = 0;
char str_time[24];

double refHeightOne;
double refHeightTwo;

void test_cfg_init(){
	config_t cfg;
    config_setting_t *test;
    const char *str;
    config_init(&cfg);
    
    if(! config_read_file(&cfg, "c.cfg"))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
        config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        //return(EXIT_FAILURE);
    }
    test = config_lookup(&cfg, "Test");
    
    config_setting_lookup_bool(test, "Debug", &Debug);
    config_setting_lookup_int(test, "HoverOffset", &HoverOffset);
    config_setting_lookup_int(test, "ThrottleStep", &ThrottleStep);
    config_setting_lookup_int(test, "YawSpeed", &YawSpeed);
    config_setting_lookup_int(test, "PitchSpeed", &PitchSpeed);
    config_setting_lookup_int(test, "RollSpeed", &RollSpeed);

    config_setting_lookup_float(test, "refHeightOne", &refHeightOne);
    config_setting_lookup_float(test, "refHeightTwo", &refHeightTwo);
 
    config_destroy(&cfg);
}

void test_cfg_print(){
	if(Debug){
    	printf("--------Test Configuration-------\n\n");
		printf(" HoverOffset: %d", HoverOffset);
		printf(" ThrottleStep: %d", ThrottleStep);
		printf(" YawSpeed: %d", YawSpeed);
		printf(" PitchSpeed: %d", PitchSpeed);
		printf(" RollSpeed: %d", RollSpeed);
		printf(" refHeightOne: %lf", refHeightOne);
		printf(" refHeightTwo: %lf", refHeightTwo);
    	printf("---------------------------------\n\n");
	}
}


void get_time(){
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	sprintf(str_time, "%s",asctime(timeinfo));
}

//Functions for motorcontroll
void setHover(){
	int servo[4] = {1,2,3,4};
	int hover[4] = {50,50,HoverOffset,50};
	
	for (int i = 0; i<4;i++){
		Set_Servo(servo[i],hover[i]);
	}
}

/*
Testcase1, Step from hover
Idea:	Test roll capabilities of the quadcopter without sensordata.
Action:	When the function is called. Hover is triggered. After 4 seconds a step in the thrust is given.
	Four seconds later hover is set again, and the loop is exited.
Setup: 	Doubled inputs from RC reciever to the MUX on all ports except from thrust
	which is controlled by the rPI in one of the sets of the inputs. For safety; all other servos are set 
	to idle from the rPI, so that if they are controlled through the mux aswell it will stay stationary, hovering.
*/

void testHoverToStep(){
  printf("Startar hover to step test, setter hover\n");

  get_time();
  char fname[50];
  char file_cmd[128];
  char dir[128];
  sprintf(fname, "%s offset=%d step=%d.txt", str_time, HoverOffset, ThrottleStep);
  sprintf(dir, "/home/pi/logs/ThrottleStep/\"%s\"", fname);
  //sprintf(file_cmd, "touch /home/pi/logs/ThrottleStep/\"%s\"", fname);
  //system(file_cmd);
  
  FILE *fp;
  fp=fopen(dir,"w");
  
  setHover();
  sleep(4);
  
  printf("Initierar steg och skriver ner värden\n");
  printf("Set_servo\n");
  
  Set_Servo(3, ThrottleStep);
  double start_time = millis();
  printf("servo satt\n");

  for (int i = 0; i<40;i++){
    double Uh = getUHeight();
    double Bh = getBHeight();
    double currentHeight = getHeight(Uh, Bh); //Use the ultra sensor to get height
    
    fprintf(fp, "Time = %lf, Höjd = %lf, Uh = %lf, Bh = %lf\n", millis()-start_time, currentHeight, Uh, Bh);
    printf("Time = %lf, Höjd = %lf, Uh = %lf, Bh = %lf\n", millis()-start_time, currentHeight, Uh, Bh);
    delay(100);
  }
  printf("Klar med test, setter hover\n");

  fclose(fp);
  setHover();
}

/*
Testcase2, Rotera ett varv
Idea:	Test yaw capabilities of the quadcopter without magnetometer sensordata.
Action:	Sets hover, after 4 seconds sets a predetermined value to yaw, clockwise direction.
	After 4 seconds, sets hover for 4 seconds. Then sets a counter clockwise rotation for 4 seconds.
	Initially it is controlled by time and not the magnetometer.
Setup: 	Doubled inputs from RC reciever to the MUX on all ports except from yaw
	which is controlled by the rPI in one of the sets of the inputs. For safety; all other servos are set 
	to idle from the rPI, so that if they are controlled through the mux aswell it will stay stationary, hovering.
*/

void testRotation(){

//get_time();
  char fname[50];
  char file_cmd[128];
  char dir[128];
  sprintf(fname, "%s speed=%d.txt", str_time, YawSpeed);
  sprintf(dir, "/home/pi/logs/Rotation/\"%s\"", fname);

  FILE *fp;
  fp=fopen(dir,"w");
 
  printf("Startar yaw rotations test, setter hover\n");
  
  setHover();
  sleep(4);
  
  printf("Initierar clockwise rotation\n"); 
  
  double start_time = millis();
  Set_Servo(4,50+YawSpeed);

  
  for(int i = 0; i < 60; i++){
    double head=getHeading();
  	fprintf(fp, "Time = %lf, Grader = %lf\n", millis()-start_time, head);
  	printf("Time = %lf, Grader = %lf\n", millis()-start_time, head);
  	delay(100);
  }

  fprintf(fp, "Klar med en rotation, börjar nästa\n");
  setHover();
  printf("Klar med clockwise, setter hover och mäter bromsning\n");
  for(int i = 0; i<20; i++){
  	double head = getHeading();
	fprintf(fp, "Time = %lf, Grader = %lf\n", millis()-start_time, head);
	printf("Time = %lf, Grader = %lf\n", millis()-start_time, head);
  	delay(100);
  }
  sleep(3);
 
  printf("Initierar counter clockwise rotation\n");
 
  start_time = millis();
  Set_Servo(4, 50-YawSpeed);
 
  for(int i = 0; i < 60; i++){
  	double head = getHeading();
  	fprintf(fp, "Time = %lf, Grader = %lf\n", millis()-start_time, head);
  	printf("Time = %lf, Grader = %lf\n", millis()-start_time, head);
  	delay(100);
  }
  
  printf("Klar med test, setter hover och mäter bromsning\n");
  for(int i = 0; i<20; i++){
  	double head = getHeading();
	fprintf(fp, "Time = %lf, Grader = %lf\n", millis()-start_time, head);
	printf("Time = %lf, Grader = %lf\n", millis()-start_time, head);
  	delay(100);
  }
  fclose(fp);
  setHover();

}

/*
Testcase3, pitch
Idea:	Test pitch capabilities of the quadcopter without magnetometer sensordata.
Action:	Sets hover, after 4 seconds sets a predetermined value of to pitch, forward direction.
	After 4 seconds, sets hover for 4 seconds. Then sets a backwards direction for 4 seconds.
	Initially it is controlled by force and not the magnetometer.
Setup: 	Doubled inputs from RC reciever to the MUX on all ports except from pitch
	which is controlled by the rPI in one of the sets of the inputs. For safety; all other servos are set 
	to idle from the rPI, so that if they are controlled through the mux aswell it will stay stationary, hovering.
*/

void pitchTest(){
	get_time();
  char fname[50];
  char file_cmd[128];
  char dir[128];
  sprintf(fname, "%s speed=%d.txt", str_time, PitchSpeed);
  sprintf(dir, "/home/pi/logs/Pitch/\"%s\"", fname);

  FILE *fp;
  fp=fopen(dir,"w");
  printf("Startar pitch test, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar framåt\n");
  Set_Servo(1,50+PitchSpeed);
  sleep(4);
  printf("Klar med framåt, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar bakåt\n");
  Set_Servo(1, 50-PitchSpeed);
  sleep(4);
  printf("Klar med pitch test, setter hover\n");
  fclose(fp);
  setHover();
}
/*
Testcase4, roll
Idea:	Test roll capabilities of the quadcopter without magnetometer sensordata.
Action:	Sets hover, after 4 seconds sets a predetermined value of to roll, right direction.
	After 4 seconds, sets hover for 4 seconds. Then sets a left direction for 4 seconds.
	Initially it is controlled by force and not the magnetometer.
Setup:	Doubled inputs from RC reciever to the MUX on all ports except from roll
	which is controlled by the rPI in one of the sets of the inputs. For safety; all other servos are set 
	to idle from the rPI, so that if they are controlled through the mux aswell it will stay stationary, hovering.
*/

void rollTest(){
	get_time();
  char fname[50];
  char file_cmd[128];
  char dir[128];
  sprintf(fname, "%s speed=%d.txt", str_time, PitchSpeed);
  sprintf(dir, "/home/pi/logs/Roll/\"%s\"", fname);

  FILE *fp;
  fp=fopen(dir,"w");  

  printf("Startar roll test, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar höger\n");
  Set_Servo(2,50+RollSpeed);
  sleep(4);
  printf("Klar med höger, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar vänster\n");
  Set_Servo(2, 50-RollSpeed);
  sleep(4);
  printf("Klar med roll test, setter hover\n");
  fclose(fp);
  setHover();
}

/* 
Testcase 5, height regulated by ultra sensor
Idea:	Motorforce is controlled by a PID controller using height given by the ultra sensor as input,
	and the output being a number between 0-100, setting motorforce. The goal is to hover at a given
	height, inside an acceptable range. The range for the ultra sensor to work is 5 cm to 300 cm.
Action:	The motor is first given an offset to achieve hover. The pilot controlls the copter to a height 
	between 20 cm and 300 cm. A goal is set as an argument to the function and the program is initiated.
	Current height and goal height is sent into the PID and a motorforce is calculated. This is then
	sent to the thrust servo and a new value is calculated shortly later.
Setup:	The ultra sensor should be placed facing downwards. Doubled signals from the RC reciever is sent
	to the MUX, except for the thrust which is sent from the rPI on one set of inputs.
*/

void pidHeightTest(){
//  get_time();
  char fname[50];
  char file_cmd[128];
  char dir[128];
  sprintf(fname, "%s.txt", str_time, PitchSpeed);
  sprintf(dir, "/home/pi/logs/Roll/\"%s\"", fname);

  FILE *fp;
  fp=fopen(dir,"w");  
  fprintf(fp, "Startar PID height test, setter hover\n");
  printf("Startar PID height test, setter hover\n");
  setHover();
 double start_time = millis();
//  for(int i = 0; i<1000; i++){
  while(1){
    double currentHeight = getHeight(getUHeight(), getBHeight()); //Use the ultra sensor to get height
    double diff = refHeightOne-currentHeight;
    printf("Ref: %d, Cur: %d\n", refHeightOne, currentHeight, diff);
    int reqThrust = PIDcal(refHeightOne-currentHeight);//PID function
    printf("Höjd: %lf\n", currentHeight);
    fprintf(fp, "Time = %lf, Höjd = %lf, Hastighet ut från PID = %i\n",  millis()-start_time, currentHeight, reqThrust);
    printf("Time = %lf, Höjd = %lf, Hastighet ut från PID = %i\n",  millis()-start_time, currentHeight, reqThrust);
    Set_Servo(3, reqThrust);
    delay(100);//0.1 second
  }//Iterate 10 seconds
  printf("Klar med PID height test, borde hovra nu\n");
  setHover();//Should not be needed if the PID works nicely
  fclose(fp);
}

void pidHeightTestTwo(){
//  get_time();
  char fname[50];
  char file_cmd[128];
  char dir[128];
  sprintf(fname, "%s.txt", str_time, PitchSpeed);
  sprintf(dir, "/home/pi/logs/Roll/\"%s\"", fname);

  FILE *fp;
  fp=fopen(dir,"w");  
  fprintf(fp, "Startar PID height test, setter hover\n");
  printf("Startar PID height test, setter hover\n");
  setHover();
 double start_time = millis();
//  for(int i = 0; i<1000; i++){
  for(int i = 0; i<200; i++){
    double currentHeight = getHeight(getUHeight(), getBHeight()); //Use the ultra sensor to get height
    double diff = refHeightOne-currentHeight;
    printf("Ref: %d, Cur: %d\n", refHeightOne, currentHeight, diff);
    int reqThrust = PIDcal(refHeightOne-currentHeight);//PID function
    printf("Höjd: %lf\n", currentHeight);
    fprintf(fp, "Time = %lf, Höjd = %lf, Hastighet ut från PID = %i\n",  millis()-start_time, currentHeight, reqThrust);
    printf("Time = %lf, Höjd = %lf, Hastighet ut från PID = %i\n",  millis()-start_time, currentHeight, reqThrust);
    Set_Servo(3, reqThrust);
    delay(100);//0.1 second
  }//Iterate 20 seconds
  for(int i = 0; i<200; i++){
    double currentHeight = getHeight(getUHeight(), getBHeight()); //Use the ultra sensor to get height
    double diff = refHeightTwo-currentHeight;
    printf("Ref: %d, Cur: %d\n", refHeightTwo, currentHeight, diff);
    int reqThrust = PIDcal(refHeightTwo-currentHeight);//PID function
    printf("Höjd: %lf\n", currentHeight);
    fprintf(fp, "Time = %lf, Höjd = %lf, Hastighet ut från PID = %i\n",  millis()-start_time, currentHeight, reqThrust);
    printf("Time = %lf, Höjd = %lf, Hastighet ut från PID = %i\n",  millis()-start_time, currentHeight, reqThrust);
    Set_Servo(3, reqThrust);
    delay(100);//0.1 second
  }//Iterate 20 seconds
  for(int i = 0; i<200; i++){
    double currentHeight = getHeight(getUHeight(), getBHeight()); //Use the ultra sensor to get height
    double diff = refHeightOne-currentHeight;
    printf("Ref: %d, Cur: %d\n", refHeightOne, currentHeight, diff);
    int reqThrust = PIDcal(refHeightOne-currentHeight);//PID function
    printf("Höjd: %lf\n", currentHeight);
    fprintf(fp, "Time = %lf, Höjd = %lf, Hastighet ut från PID = %i\n",  millis()-start_time, currentHeight, reqThrust);
    printf("Time = %lf, Höjd = %lf, Hastighet ut från PID = %i\n",  millis()-start_time, currentHeight, reqThrust);
    Set_Servo(3, reqThrust);
    delay(100);//0.1 second
  }//Iterate 20 seconds
  printf("Klar med PID height test, borde hovra nu\n");
  setHover();//Should not be needed if the PID works nicely
  fclose(fp);
}
/*
void loopMagnetometer(){
	while(1){
		int x = getX();
		int y = getY();
		int z = getZ();
		printf("X: %i, Y: %i, Z: %i\n", x,y,z);
	}
}*/
/*
int main(){
	wiringPiSetup();
	while(1){
		printf("---------------------------------\n");
		printf("Make a choice:\n");
		printf("---------------------------------\n");
		printf("[1]	Arm FlightController\n");
		printf("[2]	Disrm FlightController\n");
		printf("[3]	Hover\n");
		printf("[4]	testHoverToStep\n");
		printf("[5]	testRotation\n");
		printf("[6]	pitchTest\n");	
		printf("[7]	rollTest\n");	
		printf("[8]	pidHeightTest\n");	
		printf("[9]	Quit\n");
		printf("---------------------------------\n");

		int val;

		scanf("%d", &val);
		if(val == 1){
			printf("Arming...\n");
			Arm_FlightController();
			printf("Done!\n");


		}else if( val == 2){
			printf("Disarming...\n");
			Disarm_FlightController();
			printf("Done!\n");

		}else if(val == 3){
			printf("Hovering\n");
			setHover();
			printf("Done!\n");
			
		}else if(val == 4){
			printf("Test hover to step\n");
			testHoverToStep();
			printf("Done!\n");
		}else if(val == 5){
			printf("Test rotation\n");
			testRotation();
			printf("Done!\n");
		}else if(val == 6){
			printf("Pitch test\n");
			pitchTest();
			printf("Done!\n");
		}else if(val == 7){
			printf("Roll test\n");
			rollTest();
			printf("Done!\n");
		}else if(val == 8){
			printf("Pid regulated height test\n");
			pidHeightTest(100);
			printf("Done!\n");
		}
		else{
			printf("Invalid\n");
		}
	}
	///////////////////
	return 1;
}
*/
