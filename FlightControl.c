#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 Pinout FlightController -> Servo_Num -> Raspberry Pi
 ----------------------------------------------------
	AIL -> 1 -> P1-11
	ELE -> 2 -> P1-12
	THR -> 3 -> P1-13
	RUD -> 4 -> P1-15
 ----------------------------------------------------
 */

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


