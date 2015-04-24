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
void setHover(){
  int servo[4] = {1,2,3,4};
	int hover[4] = {50,50,hoverConst,50};
	
	for (int i = 0; i<4;i++){
//	  Set_Servo(servo[i],hover[i]);
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
//  Set_Servo(3, afterStep);
  sleep(4);
  printf("Klar med test, setter hover\n");
  //Hover
  setHover();
  return 0;
}

/*
Testcase2, Rotera ett varv
Sets hover, after 4 seconds sets a predetermined value of force to yaw, clockwise direction.
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
//  Set_Servo(4,50+yawSpeed);
  sleep(4);
  printf("Klar med clockwise, setter hover\n");
  setHover();
  sleep(4);
  printf("Initierar counter clockwise rotation\n");
//  Set_Servo(4, 50-yawSpeed);
  sleep(4);
  printf("Klar med test, setter hover\n");
  setHover();
  return 0;
}
