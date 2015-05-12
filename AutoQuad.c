#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <libconfig.h>
#include <termios.h>
#include <fcntl.h>

#include "SensorFusion.c"
#include "FlightControl.c"
#include "testCases.c"

int input_fd;

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}


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
            
            printf("---------------------------------\n");
            printf("Servo Menu\n");
            printf("---------------------------------\n");
            printf("[1] Set Aileron\n");
            printf("[2] Set Elevation\n");
            printf("[3] Set Throttle\n");
            printf("[4] Set Ruder\n");
            printf("---------------------------------\n");
            
            scanf("%d",&servo);
            
            printf("Set Speed (0-100)\n");
            printf("---------------------------------\n");
            
            scanf("%d", &speed);
            
            Set_Servo(servo, speed);

            system("clear");

            
        }else if(main_menu == 4){
            test_cfg_init();
            test_cfg_print();
            get_time();

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
            system("clear");

            //barometertest
            
            if (test_menu == 1){
                int pingprompt=0;
                while (pingprompt>=0){
                    
                    printf("---------------------------------\n");
                    printf("Ping Menu\n");
                    printf("---------------------------------\n");
                    //printf("ping to see if program->sensor communication (pipe) is working\n");
                    printf("[1]	Barometer\n");
                    printf("[2]	Ultrasonic sensor\n");
                    printf("[3]	Magnetometer\n");
                    printf("[4]	GPS\n");
                    printf("[5]	All\n");
                    printf("[6]	Back\n");
                    printf("---------------------------------\n");
                    scanf("%d",&pingprompt);
					system("clear");


                    
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
                    
                    printf("---------------------------------\n");
                    printf("Data Menu\n");
                    printf("---------------------------------\n");
                    //printf("recieve 1 data to see if sensor->program communication (pipe) is working\n");
                    printf("[1]	Barometer\n");
                    printf("[2]	Ultrasonic sensor\n");
                    printf("[3]	Magnetometer\n");
                    printf("[4]	GPS\n");
                    printf("[5]	All\n");
                    printf("[6]	Loop all sensors and log data\n");
                    printf("[7]	Loop height fast\n");
                    printf("[8]	Back\n");
                    printf("---------------------------------\n");
                    scanf("%d",&recdataprompt);
                    system("clear");

                    
                    if(recdataprompt==1){
                        printf("Barometer height(m): %lf\n", getBHeight());
                        
                    } else if (recdataprompt==2){
                        printf("Ultrasonic height(cm): %lf\n", getUHeight());
                        
                    } else if (recdataprompt==3){
                        printf("Magnetometer heading(deg): %lf\n", getHeading());
                        
                    } else if (recdataprompt==4){
                    	double * gpsdata = getCoordinate();
                        printf("lat: %lf long: %lf quality: %lf nsat: %lf sdn: %lf sde: %lf\n", *(gpsdata+1),*(gpsdata+2),*(gpsdata+3), *(gpsdata+4), gpsdata[5], gpsdata[6]);
                        
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
                            printf("1\n");
                            double * gpsdata=getCoordinate();
                            double latitude=*(gpsdata+1);
                            double longitude=*(gpsdata+2);
                            double quality=*(gpsdata+3);
                            double nsat=*(gpsdata+4);
                            double sdn=*(gpsdata+5);
                            double sde=*(gpsdata+6);

                            printf("2\n");
                            updateLog(bh,uh,h,hd,latitude,longitude,quality,nsat,sdn,sde);
                            //double bheight, double uheight, double height, double heading, double latitude, double longitude, double satquality, double nsat, double sdn, double sde
                            printf("Barometer: %.2lf   Ultrasonic: %.2lf  Height: %.2lf  Magnetometer: %.2lf Lat: %lf Long: %lf Q: %lf nsat: %lf sdn: %lf sde: %lf\n",bh,uh,h,hd,latitude,longitude,quality,nsat,sdn,sde);
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

void linkInputFifo(){
	  char* input_fifo = "/home/pi/tmp/input_fifo";
      input_fd=open(input_fifo, O_RDONLY);
      if (input_fd==-1){ 
          printf("AQ_input_pipe=error: %s\n",strerror(errno));
      } 
      fcntl(input_fd, F_SETFL, O_NONBLOCK); //set non blocking
}
int checkForInput(){
	 char buffer[8];
	 int input=-1;

      if (read(input_fd, buffer, sizeof(buffer))>0){
          input=atoi(buffer);
      }
	return(input);
}