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
When the function is called. Hover is triggered. After 4 seconds a step in the thrust is given.
Four seconds later hover is set again, and the loop is exited.
Setup: Doubled inputs from RC reciever to the MUX on all ports except from thrust
which is controlled by the rPI in one of the sets of the inputs. For safety; all servos are set 
to idle, so that if they are controlled through the mux aswell it will stay stationary, hovering.
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
Sets hover, after 4 seconds sets a predetermined value to yaw, clockwise direction.
After 4 seconds, sets hover for 4 seconds. Then sets a counter clockwise rotation for 4 seconds.
Initially it is controlled by time and not the magnetometer (TODO).
Setup: Doubled inputs from RC reciever to the MUX on all ports except from yaw
which is controlled by the rPI in one of the sets of the inputs. For safety; all servos are set 
to idle, so that if they are controlled through the mux aswell it will stay stationary, hovering.
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
	Sets hover, after 4 seconds sets a predetermined value of to pitch, forward direction.
	After 4 seconds, sets hover for 4 seconds. Then sets a backwards direction for 4 seconds.
	Initially it is controlled by force and not the magnetometer (TODO?).
	Setup: Doubled inputs from RC reciever to the MUX on all ports except from pitch
	which is controlled by the rPI in one of the sets of the inputs. For safety; all servos are set 
	to idle, so that if they are controlled through the mux aswell it will stay stationary, hovering.
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
	Sets hover, after 4 seconds sets a predetermined value of to roll, right direction.
	After 4 seconds, sets hover for 4 seconds. Then sets a left direction for 4 seconds.
	Initially it is controlled by force and not the magnetometer (TODO?).
	Setup: Doubled inputs from RC reciever to the MUX on all ports except from roll
	which is controlled by the rPI in one of the sets of the inputs. For safety; all servos are set 
	to idle, so that if they are controlled through the mux aswell it will stay stationary, hovering.
*/
int rollTest(void){
	const int rollSpeed = 30;
  printf("Startar roll test, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar höger\n");
  Set_Serv(1,50+rollSpeed);
  sleep(4);
  printf("Klar med höger, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar vänster\n");
  Set_Serv(1, 50-rollSpeed);
  sleep(4);
  printf("Klar med roll test, setter hover\n");
  setHover();
  return 0;
}
