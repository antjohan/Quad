#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiI2C.h>
//#include "BMP180.c"
//include "ultra.c"
const int hoverConst = 70;
//Functions for motorcontroll
void setHover(){
	int servo[4] = {1,2,3,4};
	int hover[4] = {50,50,hoverConst,50};
	
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
static float pre_error = 0; 
static float integral = 0;  
float PIDcal(float diff) {   
	printf("Diff: %lf \n", diff);
	float epsilon = 0.01;
	float dt = 0.01; //100ms loop time 
	float MAX = 5;  //for Current Saturation 
	float MIN = -5; //hoverconst-..
	float Kp = 5;
	float Kd = 0.2;
	float Ki = 10;
	float error = diff; 
	float derivative;    
	float output = 0;
   for (int i = 0; i < 50; i++){
   	printf("Iteration\n");
   	printf("Gammal error: %lf \n", error);
	error = error - output*0.02;//output*0.02 tillsvarar 2 cm per iteration per motorkraft över hover
	printf("Ny error: %lf \n", error);
	 //Caculate P,I,D    
  	  //hur gör man detta om till fel i hastighet?
  	//In case of error too small then stop integration    
  //	if(abs(error) > epsilon)    {        
  		integral = integral + error*dt;    
	//}    
	derivative = (error - pre_error)/dt;    
	printf("Derivative: %lf \n", derivative);

	printf("Integral: %lf \n", integral);

	output = (Kp*error + Ki*integral + Kd*derivative);//+hoverConst
	//Saturation Filter    
	if(output > MAX)    {        
	  	output = MAX;    
	}    
	else if(output < MIN)    {        
  	 	output = MIN;    
  	}        //Update error        
	pre_error = error; //pre error måste lagras samma plats som denna kod används
	output = round(output);
 	printf("Output (post max/min/floor) : %lf \n", output);
   }
   return output;
}


int testHoverToStep(void){
  printf("Startar hover to step test, setter hover\n");
  
  int afterStep = 75;
  FILE *fp;
  fp=fopen("/home/pi/logs/hoverToStepTest.txt","w");
  setHover();
  sleep(4);
  printf("Initierar steg och skriver ner värden\n");
  fprintf(fp, "Nytt test, hover to step\n");
  //Initiate step
  printf("Set_servo\n");
  Set_Servo(3, afterStep);
  double start_time = millis();
  printf("servo satt\n");
//  ultraSetup();//Hårdkod, ska bort senare
  for (int i = 0; i<80;i++){
    double currentHeight = getHeight(); //Use the ultra sensor to get height
    
    fprintf(fp, "Time = %lf, Höjd = %lf\n", millis()-start_time, currentHeight);
   // fprintf(fp, "#Iteration = %i, Höjd = %lf\n", i, currentHeight);
    printf("Time = %lf, Höjd = %lf\n", millis()-start_time, currentHeight);
    delay(50);
  }
  printf("Klar med test, setter hover\n");
  //Hover
  fclose(fp);
  setHover();
  return 0;
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
int testRotation(void){
  const int yawSpeed = 30;
  FILE *fp;
  fp=fopen("/home/pi/logs/testRotation.txt","w");
  fprintf(fp, "Nytt test, testRotation\n");
 
  printf("Startar yaw rotations test, setter hover\n");
  setHover();
  sleep(4);
  double start_time = millis();
  printf("Initierar clockwise rotation\n");
  Set_Servo(4,50+yawSpeed);
  for(int i = 0; i < 60; i++){
  	fprintf(fp, "Time = %lf, Grader = %lf\n", millis()-start_time, getHeading());
  	delay(50);
  }
  fprintf(fp, "Klar med en rotation, börjar nästa\n");
  printf("Klar med clockwise, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar counter clockwise rotation\n");
  start_time = millis();
  Set_Servo(4, 50-yawSpeed);
  for(int i = 0; i < 60; i++){
  	fprintf(fp, "Time = %lf, Grader = %lf\n", millis()-start_time, getHeading());
  	delay(50);
  }
  fclose(fp);
  printf("Klar med test, setter hover\n");
  setHover();
  
  return 0;
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
int pitchTest(void){
  const int pitchSpeed = 30;
  printf("Startar pitch test, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar framåt\n");
  Set_Servo(1,50+pitchSpeed);
  sleep(4);
  printf("Klar med framåt, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar bakåt\n");
  Set_Servo(1, 50-pitchSpeed);
  sleep(4);
  printf("Klar med pitch test, setter hover\n");
  setHover();
  return 0;
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
int rollTest(void){
	const int rollSpeed = 30;
  printf("Startar roll test, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar höger\n");
  Set_Servo(2,50+rollSpeed);
  sleep(4);
  printf("Klar med höger, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar vänster\n");
  Set_Servo(2, 50-rollSpeed);
  sleep(4);
  printf("Klar med roll test, setter hover\n");
  setHover();
  return 0;
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
int pidHeightTest(int refHeight){
  const int hoverOffset = 70;
  FILE *fp;
  fp=fopen("heightAndSpeed.txt","r+");
  printf("Startar PID height test, setter hover\n");
  setHover();
  
  for(int i = 0; i<100; i++){
    printf("Ny Iteration\n");
  	// int currentHeight = getHeight(); //Use the ultra sensor to get height
  	// int reqThrust = PIDcal(float(refHeight-currentHeight));//PID function
  	// fprintf(fp, "#Iteration = %i, Höjd = %i, Hastighet ut från PID = %i", i, currentHeight, reqThrust, "\n");
  	// Set_Serv(3, reqThrust);
    delay(10);//0.01 second
  }//Iterate 10 seconds
  printf("Klar med PID height test, borde hovra nu\n");
  setHover();//Should not be needed if the PID works nicely
  fclose(fp);
  return 0;
}
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
