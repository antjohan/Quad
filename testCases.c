#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
const int hoverConst = 70;
/*
Pinout FlightController -> Servo_Num -> Raspberry Pi
----------------------------------------------------	
	AIL -> 1 -> P1-11
	ELE -> 2 -> P1-12
	THR -> 3 -> P1-13
	RUD -> 4 -> P1-15
----------------------------------------------------
*/
//Functions for motorcontroll

void Set_Serv(int num, int pos){

	if((num <= 4 || num >= 1) && (pos <= 100 || pos >= 0)){
		char* echo = "echo";
		char* destination = " > /dev/servoblaster \n";
		char* equal = "=";
		char* procent = "%";
		char cmd[256];
		snprintf(cmd, sizeof cmd, "%s %i%s%i%s %s", echo, num, equal,pos, procent, destination);
		//system(cmd);
		//printf("%s\n",cmd);
	}else{
		printf("Invalid!");
	}
}
void setHover(){
	int servo[4] = {1,2,3,4};
	int hover[4] = {50,50,hoverConst,50};
	
	for (int i = 0; i<4;i++){
	  Set_Serv(servo[i],hover[i]);
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

int testHoverToStep(void){
	printf("Startar hover to step test, setter hover\n");
  int afterStep = 75;
  setHover();
  sleep(4);
  printf("Initierar steg\n");
  //Initiate step
  Set_Serv(3, afterStep);
  sleep(4);
  printf("Klar med test, setter hover\n");
  //Hover
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
int testOneRotation(void){
  const int yawSpeed = 30;
 
  printf("Startar yaw rotations test, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar clockwise rotation\n");
  Set_Serv(4,50+yawSpeed);
  sleep(4);
  printf("Klar med clockwise, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar counter clockwise rotation\n");
  Set_Serv(4, 50-yawSpeed);
  sleep(4);
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
  Set_Serv(1,50+pitchSpeed);
  sleep(4);
  printf("Klar med framåt, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar bakåt\n");
  Set_Serv(1, 50-pitchSpeed);
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
  Set_Serv(2,50+rollSpeed);
  sleep(4);
  printf("Klar med höger, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar vänster\n");
  Set_Serv(2, 50-rollSpeed);
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
  printf("Startar PID height test, setter hover\n");
  setHover();
  for(int i = 0; i<100; i++){
    printf("Ny Iteration\n");
    int currentHeight = getHeight(); //Use the ultra sensor to get height
    int reqThrust = getThrust(refHeight-currentHeight);//PID function
    Set_Serv(3, reqThrust);
    delay(100);//0.1 second
  }//Iterate 10 seconds
  printf("Klar med PID height test, borde hovra nu\n");
  setHover();//Should not be needed if the PID works nicely
  return 0;
}



