#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/*
 Pinout FlightController -> Servo_Num -> Raspberry Pi
 ----------------------------------------------------
	AIL -> 1 -> P1-11
	ELE -> 2 -> P1-12
	THR -> 3 -> P1-13
	RUD -> 4 -> P1-15
 ----------------------------------------------------
 */
int hoverConst = 71;

void Set_Servo(int num, int pos){
    
    if((num <= 4 || num >= 1) && (pos <= 100 || pos >= 0)){
        char* echo = "echo";
        char* destination = " > /dev/servoblaster \n";
        char* equal = "=";
        char* procent = "%";
        char cmd[256];
        snprintf(cmd, sizeof cmd, "%s %i%s%i%s %s", echo, num, equal,pos, procent, destination);
        system(cmd);
        //printf("%s\n",cmd);
    }else{
        printf("Invalid!");
    }
}


void Arm_FlightController(){
    int servo[4] = {1,2,3,4};
    int arm[4] = {0,0,0,0};
    int normal[4] = {50,50,0,50};
    
    for(int i = 0; i < 4; i++){
        Set_Servo(servo[i],arm[i]);
    }
    sleep(2);
    
    for(int i = 0; i < 4; i++){
        Set_Servo(servo[i],normal[i]);
    }
}

void Disarm_FlightController(){
    int servo[4] = {1,2,3,4};
    int disarm[4] = {0,0,0,100};
    int normal[4] = {50,50,0,50};
    
    for(int i = 0; i < 4; i++){
        Set_Servo(servo[i],disarm[i]);
    }
    sleep(2);
    
    for(int i = 0; i < 4; i++){
        Set_Servo(servo[i],normal[i]);
    }
}

static float pre_error = 0; 
static float integral = 0;  
float PIDcal(float diff) {   
//	printf("Diff: %lf \n", diff);
	float epsilon = 0.01;
	float dt = 0.1; //100ms loop time 
	float MAX = hoverConst+5;  //for Current Saturation 
	float MIN = hoverConst-5; //hoverconst-..
	float Kp = 18;
	float Kd = 0.005;
	float Ki = 0.0002;
	float error = diff; 
	float derivative;
	float output = 0;
	printf("Error: %lf \n", error);
	 //Calculate P,I,D    
  	integral = integral + error*dt;    
	derivative = (error - pre_error)/dt;
//	printf("Derivative: %lf \n", derivative);
//	printf("Integral: %lf \n", integral);

	output = hoverConst+(Kp*error + Ki*integral + Kd*derivative);
	//Saturation Filter    
	if(output > MAX)    {        
	  	output = MAX;    
	}    
	else if(output < MIN)    {        
  	 	output = MIN;    
  	}        //Update error        
	pre_error = error;
	output = round(output);
 	printf("Output (post max/min/floor) : %lf \n", output);
//   }
   return output;
}


