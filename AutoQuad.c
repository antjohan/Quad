#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>
//#include "BMP180.c"
//#include "ultra.c"
#include "SensorFusion.c"

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
		system(cmd);
		//printf("%s\n",cmd);
	}else{
		printf("Invalid!");
	}
}

#include "testCases.c" //Test!

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
	wiringPiSetupSys();
	sfinit();
	while(1){
		printf("---------------------------------\n");
		printf("Make a choice:\n");
		printf("---------------------------------\n");
		printf("[1]	Arm FlightController\n");
		printf("[2]	Disarm FlightController\n");
		printf("[3]	Set Servo\n");
		printf("[4]	Test prog\n");
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

		}
		else if(val == 4){
			int test;
			printf("---------------------------------\n");
			printf("[1]	ping...\n");
			printf("[2]	recieve data\n");
			printf("[3]	Testa hoverToStep\n");
			printf("[4]	Testa ett varv rotation\n");
			printf("[5]	Calibrate magnetometer\n");
			printf("[6]	Test PID regulator\n");
			printf("[7]	Set hover\n");
			printf("[8]	Tilbaks\n");
			printf("---------------------------------\n");
			scanf("%d",&test);
			//barometertest
			if (test == 1){
				int pingprompt=0;
				while (pingprompt>=0){
					printf("---------------------------------\n");
					printf("ping to see if program->sensor communication (pipe) is working\n");
					printf("[1]	Barometer\n");
					printf("[2]	Ultrasonic sensor\n");
					printf("[3]	Magnetometer\n");
					printf("[4]	GPS\n");
					printf("[5]	All\n");
					printf("[6] Back\n");
					printf("---------------------------------\n");
					scanf("%d",&pingprompt);
					if(pingprompt==1){
						commandSensor("baro\0", "ping\0");
					} else if (pingprompt==2){
						commandSensor("ultra\0", "ping\0");
					} else if (pingprompt==3){
						commandSensor("mag\0", "ping\0");
					} else if (pingprompt==4){
						commandSensor("gps\0", "ping\0");
					} else if (pingprompt==5){
						commandSensor("baro\0", "ping\0");
						commandSensor("ultra\0", "ping\0");
						commandSensor("mag\0", "ping\0");
						commandSensor("gps\0", "ping\0");
					} else if (pingprompt==6){
						pingprompt=-1;
					}
				}
			}
			//Ultra sensor
			else if(test == 2){
				int recdataprompt=0;
				while (recdataprompt>=0){
					printf("---------------------------------\n");					
					printf("recieve 1 data to see if sensor->program communication (pipe) is working\n");
					printf("[1]	Barometer\n");
					printf("[2]	Ultrasonic sensor\n");
					printf("[3]	Magnetometer\n");
					printf("[4]	GPS\n");
					printf("[5]	All\n");
					printf("[6]	Loop all sensors and log data\n");
					printf("[7] Loop height fast\n");
					printf("[8] Back\n");
					printf("---------------------------------\n");
					scanf("%d",&recdataprompt);
					if(recdataprompt==1){
						printf("Barometer height(m): %lf\n", getBHeight());
					} else if (recdataprompt==2){
						printf("Ultrasonic height(cm): %lf\n", getUHeight());
					} else if (recdataprompt==3){
						printf("Magnetometer heading(deg): %lf\n", getHeading());
					} else if (recdataprompt==4){
						printf("GPS coordinates(lat/long/quality): %lf\n", getBHeight());
					} else if (recdataprompt==5){
						printf("Barometer height(m): %lf\n", getBHeight());
						printf("Ultrasonic height(cm): %lf\n", getUHeight());
						printf("Magnetometer heading(deg): %lf\n", getHeading());
						printf("GPS coordinates(lat/long/quality): %lf\n", getBHeight());
					} else if (recdataprompt==6){
						while(1){
								double uh=getUHeight();
								double bh=getBHeight();
								double h=getHeight(uh,bh);
								double hd=getHeading();

								updateLog(bh, uh, h, hd);

							printf("Barometer: %lf   Ultrasonic: %lf  Height: %lf  Magnetometer: %lf\n",bh,uh,h, hd);
							delay(50);
						}
					} else if (recdataprompt==7){
						while(1){
							printf("H: %lf\n",getHeight());
							delay(200);
						}
					}else if (recdataprompt==8){
						recdataprompt=-1;
					}
				}
			}
			//Test hoverToStep
			else if(test == 3){
				testHoverToStep();
			}else if(test == 4){
				testRotation();
			}
			else if(test == 5){
				commandSensor("mag", "calibrate");
			}else if(test == 6){
				PIDcal(1.2);	
				printf("Test klart");
			}else if(test == 7){
				setHover();
			}else{
				printf("Invalid\n");				
			}
			
		}else if (val == 5){
			break;
		}else if(val==6){//recieve barometer data
			//while(1){
				//printf("Barometer: %lf   Ultrasonic: %lf\n",getBHeight(),getUHeight());
				//delay(100);
			//}

		}
		else{
			printf("Invalid\n");
		}
	}
	///////////////////
	printf("Program stopping\n");
	return 1;
}
