#include <stdio.h>			// Used for printf() statements
#include <wiringPi.h> 		 // Include WiringPi library!
#include <time.h>		    	    // Include time
// Pin number declarations. We're using the Broadcom chip pin numbers.

const int trig = 23; // Broadcom pin 23 (GPIO 23)
const int echo = 24; // Broadcom pin 24 (GPIO 24)
long length;
clock_t start, echotime, testtime;


int getUltra()
{
    // Setup stuff:
    printf("getUltra\n");
 //   wiringPiSetupGpio(); // Initialize wiringPi -- using Broadcom pin numbers

    pinMode(trig, OUTPUT);     // Set trig as output
    pinMode(echo, INPUT);      // Set echo as INPUT
    printf("getUltra 2\n");
// Ev. ta bort   pullUpDnControl(butPin, PUD_UP); // Enable pull-up resistor on button

// Loop

//    while(1) {
    	 digitalWrite(trig, LOW);     // trig off
	     delay(0.002);
	     digitalWrite(trig, HIGH);     // trig on
	     delay(0.005);
	     digitalWrite(trig, LOW);     // trig off
	     printf("getUltra 3\n");
	     testtime = clock();
	     for(int i = 0; i< 200; i++){
	     	
	     }
	     
	    // while(echo != HIGH) {
	     //}
	     printf("getUltra Mellan\n");
	     start = clock();
	     printf("getUltra 4\n");
	     //while(echo != LOW) {
	     //}
	     echotime = (long)(clock() - testtime)/CLOCKS_PER_SEC;
		printf("%l", echotime);
	     length = (echotime / 58.138);
	     
	     printf("%l", length);
//	}
    printf("getUltra 6\n");
	 return length;
}



