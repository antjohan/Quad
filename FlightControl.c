#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <libconfig.h>


/*
 Pinout FlightController -> Servo_Num -> Raspberry Pi
 ----------------------------------------------------
	AIL -> 1 -> P1-11
	ELE -> 2 -> P1-12
	THR -> 3 -> P1-13
	RUD -> 4 -> P1-15
 ----------------------------------------------------
 */
//int hoverConst = 71;
float kp;
float ki;
float kd;

int HoverOffset;

void PID_cfg_init(){

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
    PID = config_lookup(&cfg, "PID");
    
    config_setting_lookup_bool(PID, "Debug", &Debug);
    config_setting_lookup_int(PID, "HoverOffset", &HoverOffset);
    config_setting_lookup_float(PID, "kp", &kp);
    config_setting_lookup_float(PID, "ki", &ki);
    config_setting_lookup_float(PID, "kd", &kd);
 
    config_destroy(&cfg);
}

void PID_cfg_print(){

    printf("kp: %f", kp);
    printf("ki: %f", ki);
    printf("kd: %f", kd);

    printf("offset: %d", HoverOffset);
}


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
	float MAX = HoverOffset+5;  //for Current Saturation 
	float MIN = HoverOffset-5; //hoverconst-..
	//float Kp = 8;
	//float Kd = 0.5;
	//float Ki = 0.02;
	float error = diff; 
	float derivative;
	float output = 0;
	printf("Error: %lf \n", error);
	 //Calculate P,I,D    
  	integral = integral + error*dt;    
	derivative = (error - pre_error)/dt;
//	printf("Derivative: %lf \n", derivative);
//	printf("Integral: %lf \n", integral);

	output = HoverOffset+(Kp*error + Ki*integral + Kd*derivative);
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


