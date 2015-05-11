#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <libconfig.h>

#include "SensorFusion.c"
#include "FlightControl.c"
#include "testCases.c"

int main(){

	system("./UGPS &");
	system("./HMC5883L &");
	system("./BMP180 &");
	system("./ultra &");

    wiringPiSetupSys();
    sfinit();
    while(1){
        
        system("clear");
        printf("---------------------------------\n");
        printf("Main Menu\n");
        printf("---------------------------------\n");
        printf("[1]	Arm FlightController\n");
        printf("[2]	Disarm FlightController\n");
        printf("[3]	Servo menu\n");
        printf("[4]	Test menu\n");
        printf("[5]	Quit\n");
        printf("---------------------------------\n");
        
        int main_menu;
        scanf("%d", &main_menu);
        
        system("clear");
        
        if(main_menu == 1){
            printf("Arming...\n");
            Arm_FlightController();
            printf("Done!\n");
            
            
        }else if( main_menu == 2){
            printf("Disarming...\n");
            Disarm_FlightController();
            printf("Done!\n");
            
        }else if(main_menu == 3){
            int servo;
            int speed;
            
            system("clear");
            printf("---------------------------------\n");
            printf("Servo Menu\n");
            printf("---------------------------------\n");
            printf("[1] Set Aileron\n");
            printf("[2] Set Elevation\n");
            printf("[3] Set Throttle\n");
            printf("[4] Set Ruder\n");
            printf("---------------------------------\n");
            
            scanf("%d",&servo);
            
            system("clear");
            printf("---------------------------------\n");
            printf("Set Speed (0-100)\n");
            printf("---------------------------------\n");
            
            scanf("%d", &speed);
            
            Set_Servo(servo, speed);
            
        }else if(main_menu == 4){
            int test_menu;
            
            system("clear");
            printf("---------------------------------\n");
            printf("Test Menu\n");
            printf("---------------------------------\n");
            printf("[1]	ping...\n");
            printf("[2]	recieve data\n");
            printf("[3]	Testa hoverToStep\n");
            printf("[4]	Testa ett varv rotation\n");
            printf("[5]	Calibrate magnetometer\n");
            printf("[6]	Test PID regulator\n");
            printf("[7]	Set hover\n");
            printf("[8]	PIDtest, 0.3m\n");
            printf("[9]	Back\n");
            printf("---------------------------------\n");
            
            scanf("%d",&test_menu);
            //barometertest
            
            if (test_menu == 1){
                int pingprompt=0;
                while (pingprompt>=0){
                    
                    system("clear");
                    printf("---------------------------------\n");
                    printf("Ping Menu\n");
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
            else if(test_menu == 2){
                
                int recdataprompt=0;
                while (1){
                    
                    system("clear");
                    printf("---------------------------------\n");
                    printf("Data Menu\n");
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
                        while (1){
                            double uh=getUHeight();
                            double bh=getBHeight();
                            double h=getHeight(uh,bh);
                            double hd=getHeading();
                            
                            updateLog(bh, uh, h, hd,0,0,0,0,0,0);
                            //double bheight, double uheight, double height, double heading, double latitude, double longitude, double satquality, double nsat, double sdn, double sde
                            printf("Barometer: %lf   Ultrasonic: %lf  Height: %lf  Magnetometer: %lf\n",bh,uh,h, hd);
                            delay(100);
                        }
                        
                    } else if (recdataprompt==7){
                       
                        while(1){
                            printf("H: %lf\n",getHeight(getUHeight(),getBHeight()));
                            delay(200);
                        }
                        
                    }else if (recdataprompt==8){
                        break;
                    }
                }
            }
            
            //Test hoverToStep
            else if(test_menu == 3){
                testHoverToStep();
                
            }else if(test_menu == 4){
                testRotation();
                
            }else if(test_menu == 5){
                commandSensor("mag", "calibrate");
                
            }else if(test_menu == 6){
                PIDcal(1.2);
                printf("Test klart");
                
            }else if(test_menu == 7){
                setHover();
                
            }else if(test_menu == 8){
                pidInsideTest(0.3);
                
            }else{
                printf("Invalid\n");
            }
            
            
        }else if (main_menu == 5){

        	system("pkill ultra");
			system("pkill BMP180");
			system("pkill HMC5883L");
			system("pkill UGPS");
            break;
            
        }
        else{
            printf("Invalid\n");
        }
    }
    printf("Program stopping\n");
    return 1;
}
