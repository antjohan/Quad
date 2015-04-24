#include <stdio.h>			// Used for printf() statements
#include <wiringPi.h> 		 // Include WiringPi library!
#include <time.h>		    	    // Include time
// Pin number declarations. We're using the Broadcom chip pin numbers.

const int trig = 23; // Broadcom pin 23 (GPIO 23)
const int echo = 24; // Broadcom pin 24 (GPIO 24)
long length;
clock_t start, echotime;


int getUltra()
{
    // Setup stuff:
    wiringPiSetupGpio(); // Initialize wiringPi -- using Broadcom pin numbers

    pinMode(trig, OUTPUT);     // Set trig as output
    pinMode(echo, INPUT);      // Set echo as INPUT

// Ev. ta bort   pullUpDnControl(butPin, PUD_UP); // Enable pull-up resistor on button

// Loop

//    while(1) {
    	     digitalWrite(trig, LOW);     // trig off
	     delay(0.002);
	     digitalWrite(trig, HIGH);     // trig on
	     delay(0.005);
	     digitalWrite(trig, LOW);     // trig off

	     while(echo != HIGH) {
	     }

	     start = clock();

	     while(echo != LOW) {
	     }

	     echotime = (clock() - start)/CLOCKS_PER_SEC;
	     length = (echotime / 58.138);
	     
	     printf("%l", length);
//	}

	 return length;
}



