#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include "BMP180.c"


/*
Pinout FlightController -> Servo_Num -> Raspberry Pi
----------------------------------------------------	
	AIL -> 1 -> P1-11
	ELE -> 2 -> P1-12
	THR -> 3 -> P1-13
	RUD -> 4 -> P1-15
----------------------------------------------------
*/ 
// HEEEEEJ

void Set_Servo(int num, int pos){

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

int main(){
	while(1){
		printf("---------------------------------\n");
		printf("Make a choice:\n");
		printf("---------------------------------\n");
		printf("[1]	Arm FlightController\n");
		printf("[2]	Disrm FlightController\n");
		printf("[3]	Set Servo\n");
		printf("[4]	i2c\n");
		printf("[5]	Quit\n");
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
			int servo;
			int speed;

			printf("[1] Set Aileron\n");
			printf("[2] Set Elevation\n");
			printf("[3] Set Throttle\n");
			printf("[4] Set Ruder\n");
			scanf("%d",&servo);

			printf("Set Speed (0-100)\n");
			scanf("%d", &speed);

			Set_Servo(servo, speed);

		}else if (val == 4){

			Initialize();

			PrintCalibrationData();
			printf("Uncompensated temperature: %d\n",GetUncompensatedTemperature());
			printf("Uncompensated pressure: %d\n",GetUncompensatedPressure());

			printf("Compensated temperature: %d\n", GetTemperature());
			printf("Compensated pressure: %d\n", GetPressure());

		
		}else if (val == 5){
			break;
		}else{
			printf("Invalid\n");
		}
	}
	///////////////////
	return 1;
}
